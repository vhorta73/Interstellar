/**
 * @file io_tests.cpp
 * @brief End-to-end tests for Interstellar::IO (FileIO + Filesystems + Container + Serializers).
 *
 * What's covered:
 *  - Roundtrip with LocalFilesystem using container header (magic/version/CRC).
 *  - Version mismatch detection.
 *  - CRC corruption detection (flip a payload byte).
 *  - Raw payload mode (no container).
 *  - MemoryFilesystem roundtrip.
 *  - JSON serializer fast load via simdjson; pretty save with correct escaping and to_chars.
 *
 * Notes:
 *  - The binary serializer uses **little-endian** integers and IEEE-754 double bits (LE).
 *  - The container header is 20 bytes in the hardened implementation; tests use that constant.
 */

#include <gtest/gtest.h>

#include <filesystem>
#include <string>
#include <vector>
#include <span>
#include <cstring>
#include <charconv>
#include <bit>
#include <cmath>      // std::isfinite
#include <cstdint>

#include <simdjson.h>

#include "Interstellar/IO/FileIO.hpp"
#include "Interstellar/IO/IFilesystem.hpp"
#include "Interstellar/IO/Container.hpp"   // for ContainerHeader size constant equivalent if available

 // Avoid MSVC's ::unexpected clash
using Interstellar::IO::make_unexpected;

using namespace Interstellar::IO;
using Interstellar::IO::kHeaderSize;

// -----------------------------------------------------------------------------
// Test domain types
// -----------------------------------------------------------------------------
struct Element {
    int           AtomicNumber{};
    std::string   Symbol;
    std::string   Name;
    double        AtomicMass{};
};

// -----------------------------------------------------------------------------
// GTest helpers for expected<...>
// -----------------------------------------------------------------------------
template <class T>
static ::testing::AssertionResult AssertOk(const expected<T, Error>& r) {
    if (r) return ::testing::AssertionSuccess();
    return ::testing::AssertionFailure()
        << "expected<T,Error> not ok. code=" << static_cast<int>(r.error().code)
        << " msg=\"" << r.error().message << "\"";
}
static ::testing::AssertionResult AssertOk(const expected<void, Error>& r) {
    if (r) return ::testing::AssertionSuccess();
    return ::testing::AssertionFailure()
        << "expected<void,Error> not ok. code=" << static_cast<int>(r.error().code)
        << " msg=\"" << r.error().message << "\"";
}
#define ASSERT_OK(x) ASSERT_TRUE(AssertOk(x))
#define EXPECT_OK(x) EXPECT_TRUE(AssertOk(x))

// -----------------------------------------------------------------------------
// Little-endian helpers (mirrors container approach)
// -----------------------------------------------------------------------------
static inline void put_u32_le(std::vector<std::byte>& out, uint32_t v) {
    const size_t i = out.size(); out.resize(i + 4);
    std::byte* p = out.data() + i;
    p[0] = std::byte(v & 0xFFu); p[1] = std::byte((v >> 8) & 0xFFu);
    p[2] = std::byte((v >> 16) & 0xFFu); p[3] = std::byte((v >> 24) & 0xFFu);
}
static inline expected<uint32_t, Error> get_u32_le(const std::byte*& p, const std::byte* end) {
    if (end - p < 4) return make_unexpected(Error{ ErrorCode::InvalidData, "eof u32" });
    uint32_t v = 0;
    v |= uint32_t(std::to_integer<uint8_t>(p[0]));
    v |= uint32_t(std::to_integer<uint8_t>(p[1])) << 8;
    v |= uint32_t(std::to_integer<uint8_t>(p[2])) << 16;
    v |= uint32_t(std::to_integer<uint8_t>(p[3])) << 24;
    p += 4; return v;
}
static inline void put_f64_le(std::vector<std::byte>& out, double d) {
    static_assert(sizeof(double) == 8, "IEEE-754 64-bit required");
    uint64_t bits = std::bit_cast<uint64_t>(d);
    const size_t i = out.size(); out.resize(i + 8);
    std::byte* p = out.data() + i;
    for (int k = 0; k < 8; ++k) p[k] = std::byte((bits >> (8 * k)) & 0xFFu);
}
static inline expected<double, Error> get_f64_le(const std::byte*& p, const std::byte* end) {
    if (end - p < 8) return make_unexpected(Error{ ErrorCode::InvalidData, "eof f64" });
    uint64_t bits = 0;
    for (int k = 0; k < 8; ++k) bits |= uint64_t(std::to_integer<uint8_t>(p[k])) << (8 * k);
    p += 8;
    return std::bit_cast<double>(bits);
}

// -----------------------------------------------------------------------------
// Minimal binary serializer (test helper) - endian safe
// Layout per element:
//   [count:u32] {
//     [AtomicNumber:u32][Symbol: u32(len) | bytes][Name: u32(len) | bytes][AtomicMass: f64-LE]
//   } * count
// -----------------------------------------------------------------------------
class ElementBinSer : public ISerializer<Element> {
public:
    uint32_t schema_version() const override { return 1u; }
    uint32_t magic_u32()      const override { return 0x454C4D31u; /* 'ELM1' */ }

    expected<std::vector<std::byte>, Error>
        serialize(std::span<const Element> items) const override {
        std::vector<std::byte> out;
        out.reserve(items.size() * 64);

        if (items.size() > 0xFFFFFFFFu) {
            return make_unexpected(Error{ ErrorCode::InvalidArgument, "Too many elements" });
        }

        put_u32_le(out, static_cast<uint32_t>(items.size()));
        for (const auto& e : items) {
            put_u32_le(out, static_cast<uint32_t>(e.AtomicNumber));
            // Symbol
            if (e.Symbol.size() > 0xFFFFFFFFu) return make_unexpected(Error{ ErrorCode::InvalidData, "Symbol too long" });
            put_u32_le(out, static_cast<uint32_t>(e.Symbol.size()));
            out.insert(out.end(), reinterpret_cast<const std::byte*>(e.Symbol.data()),
                reinterpret_cast<const std::byte*>(e.Symbol.data() + e.Symbol.size()));
            // Name
            if (e.Name.size() > 0xFFFFFFFFu) return make_unexpected(Error{ ErrorCode::InvalidData, "Name too long" });
            put_u32_le(out, static_cast<uint32_t>(e.Name.size()));
            out.insert(out.end(), reinterpret_cast<const std::byte*>(e.Name.data()),
                reinterpret_cast<const std::byte*>(e.Name.data() + e.Name.size()));
            // Mass
            put_f64_le(out, e.AtomicMass);
        }
        return out;
    }

    expected<std::vector<Element>, Error>
        deserialize(std::span<const std::byte> bytes) const override {
        const std::byte* p = bytes.data();
        const std::byte* end = p + bytes.size();

        auto get_str = [&](std::string& s) -> expected<void, Error> {
            auto lenR = get_u32_le(p, end); if (!lenR) return make_unexpected(lenR.error());
            uint32_t len = lenR.value();
            if (static_cast<size_t>(end - p) < len) return make_unexpected(Error{ ErrorCode::InvalidData, "eof str" });
            s.assign(reinterpret_cast<const char*>(p), reinterpret_cast<const char*>(p) + len);
            p += len; return {};
            };

        auto nR = get_u32_le(p, end); if (!nR) return make_unexpected(nR.error());
        std::vector<Element> out; out.reserve(nR.value());
        for (uint32_t i = 0; i < nR.value(); ++i) {
            auto zR = get_u32_le(p, end); if (!zR) return make_unexpected(zR.error());
            Element e{};
            e.AtomicNumber = static_cast<int>(zR.value());
            if (auto ok = get_str(e.Symbol); !ok) return make_unexpected(ok.error());
            if (auto ok = get_str(e.Name);   !ok) return make_unexpected(ok.error());
            auto mR = get_f64_le(p, end);    if (!mR) return make_unexpected(mR.error());
            e.AtomicMass = mR.value();
            out.push_back(std::move(e));
        }
        if (p != end) return make_unexpected(Error{ ErrorCode::InvalidData, "trailing bytes" });
        return out;
    }
};

// -----------------------------------------------------------------------------
// JSON serializer (fast-load via simdjson, robust pretty-save)
// -----------------------------------------------------------------------------
class ElementJsonSer : public ISerializer<Element> {
public:
    uint32_t schema_version() const override { return 1u; }
    uint32_t magic_u32()      const override { return 0x4A534F4Eu; /* 'JSON' */ }

    expected<std::vector<std::byte>, Error>
        serialize(std::span<const Element> items) const override {
        std::string s;
        s.reserve(items.size() * 64);

        auto escape = [](std::string_view in) {
            std::string out; out.reserve(in.size() + 8);
            for (unsigned char ch : in) {
                switch (ch) {
                case '\"': out += "\\\""; break;
                case '\\': out += "\\\\"; break;
                case '\b': out += "\\b";  break;
                case '\f': out += "\\f";  break;
                case '\n': out += "\\n";  break;
                case '\r': out += "\\r";  break;
                case '\t': out += "\\t";  break;
                default:
                    if (ch < 0x20) {
                        char buf[7];
                        std::snprintf(buf, sizeof(buf), "\\u%04X", ch);
                        out += buf;
                    }
                    else {
                        out.push_back(static_cast<char>(ch));
                    }
                }
            }
            return out;
            };

        auto append_number = [&](double v) -> bool {
            if (!std::isfinite(v)) return false; // JSON: no NaN/Inf
            char buf[64];
            auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), v, std::chars_format::general, 15);
            if (ec != std::errc{}) return false;
            s.append(buf, ptr);
            return true;
            };

        s.push_back('[');
        for (size_t i = 0; i < items.size(); ++i) {
            const auto& e = items[i];
            s.push_back('{');
            s += "\"Symbol\":\""; s += escape(e.Symbol); s += "\",";
            s += "\"Name\":\"";   s += escape(e.Name);   s += "\",";
            s += "\"AtomicNumber\":"; s += std::to_string(e.AtomicNumber); s += ",";
            s += "\"AtomicMass\":";
            if (!append_number(e.AtomicMass)) {
                return make_unexpected(Error{ ErrorCode::InvalidData, "AtomicMass not finite or to_chars failed" });
            }
            s.push_back('}');
            if (i + 1 < items.size()) s.push_back(',');
        }
        s.push_back(']');

        std::vector<std::byte> out(s.size());
        std::memcpy(out.data(), s.data(), s.size());
        return out;
    }

    expected<std::vector<Element>, Error>
        deserialize(std::span<const std::byte> bytes) const override {
        // 1) Skip BOM and leading whitespace
        const uint8_t* b = reinterpret_cast<const uint8_t*>(bytes.data());
        size_t n = bytes.size();
        if (n >= 3 && b[0] == 0xEF && b[1] == 0xBB && b[2] == 0xBF) { b += 3; n -= 3; }
        while (n && (*b == ' ' || *b == '\t' || *b == '\r' || *b == '\n')) { ++b; --n; }
        if (!n) return make_unexpected(Error{ ErrorCode::InvalidData, "Empty JSON after trimming" });

        // 2) Padded copy as required by simdjson
        simdjson::padded_string ps(n);
        std::memcpy(ps.data(), b, n);

        simdjson::ondemand::parser parser;
        auto doc_result = parser.iterate(ps);
        if (doc_result.error()) {
            return make_unexpected(Error{ ErrorCode::InvalidData,
                                          std::string("Invalid JSON: ") +
                                          simdjson::error_message(doc_result.error()) });
        }
        simdjson::ondemand::document doc = std::move(doc_result.value());

        std::vector<Element> out;
        auto t_res = doc.type();
        if (t_res.error()) return make_unexpected(Error{ ErrorCode::InvalidData, "Invalid JSON: type()" });

        if (t_res.value() == simdjson::ondemand::json_type::array) {
            auto arr_res = doc.get_array();
            if (arr_res.error()) return make_unexpected(Error{ ErrorCode::InvalidData, "Invalid JSON array" });
            for (auto v : arr_res.value()) {
                auto obj_res = v.get_object();
                if (obj_res.error()) return make_unexpected(Error{ ErrorCode::InvalidData, "Array element not object" });
                auto eR = parse_one(obj_res.value());
                if (!eR) return make_unexpected(eR.error());
                out.push_back(std::move(eR.value()));
            }
        }
        else if (t_res.value() == simdjson::ondemand::json_type::object) {
            auto obj_res = doc.get_object();
            if (obj_res.error()) return make_unexpected(Error{ ErrorCode::InvalidData, "Invalid JSON object" });
            auto eR = parse_one(obj_res.value());
            if (!eR) return make_unexpected(eR.error());
            out.push_back(std::move(eR.value()));
        }
        else {
            return make_unexpected(Error{ ErrorCode::InvalidData, "JSON must be object or array" });
        }
        return out;
    }

private:
    static expected<Element, Error> parse_one(simdjson::ondemand::object obj) {
        Element e{};

        auto sym = obj["Symbol"].get_string();
        if (sym.error()) return make_unexpected(Error{ ErrorCode::InvalidData, "Missing Symbol" });
        e.Symbol = std::string(sym.value());

        auto name = obj["Name"].get_string();
        if (name.error()) return make_unexpected(Error{ ErrorCode::InvalidData, "Missing Name" });
        e.Name = std::string(name.value());

        auto z = obj["AtomicNumber"].get_int64();
        if (z.error()) return make_unexpected(Error{ ErrorCode::InvalidData, "Missing AtomicNumber" });
        e.AtomicNumber = static_cast<int>(z.value());

        auto m = obj["AtomicMass"].get_double();
        if (m.error()) return make_unexpected(Error{ ErrorCode::InvalidData, "Missing AtomicMass" });
        e.AtomicMass = m.value();

        return e;
    }
};

// -----------------------------------------------------------------------------
// Test fixture: creates a unique temp dir per test and cleans it up
// -----------------------------------------------------------------------------
class IoTempDir : public ::testing::Test {
protected:
    std::filesystem::path dir_;
    void SetUp() override {
        auto base = std::filesystem::temp_directory_path() / "io_test";
        dir_ = base / std::to_string(reinterpret_cast<std::uintptr_t>(this));
        std::error_code ec;
        std::filesystem::create_directories(dir_, ec);
    }
    void TearDown() override {
        std::error_code ec;
        std::filesystem::remove_all(dir_, ec);
    }
};

// -----------------------------------------------------------------------------
// Tests
// -----------------------------------------------------------------------------

TEST_F(IoTempDir, Roundtrip_Local_Binary_With_Container) {
    LocalFilesystem lfs;
    FileIO io(lfs);
    ElementBinSer ser;

    std::vector<Element> elems{ {1,"H","Hydrogen",1.008},{2,"He","Helium",4.0026} };
    auto path = dir_ / "elements.bin";

    SaveOptions sopt{};
    sopt.wrap_with_container = true;
    sopt.ensure_directories = true;

    LoadOptions lopt{};
    lopt.expect_container = true;

    ASSERT_OK(io.save<Element>(path, elems, ser, sopt));
    auto loaded = io.load(path, ser, lopt);
    ASSERT_OK(loaded);
    const auto& items = loaded.value();
    EXPECT_EQ(items.size(), elems.size());
    EXPECT_EQ(items.at(0).Symbol, "H");
}

TEST_F(IoTempDir, VersionMismatch) {
    LocalFilesystem lfs; FileIO io(lfs);
    struct V2Ser final : ElementBinSer { uint32_t schema_version() const override { return 2u; } } v2;
    ElementBinSer v1;

    auto path = dir_ / "v2.bin";
    std::vector<Element> elems{ {1,"H","Hydrogen",1.008} };

    SaveOptions sopt{};
    sopt.wrap_with_container = true;
    sopt.ensure_directories = true;

    ASSERT_OK(io.save<Element>(path, elems, v2, sopt));

    LoadOptions lopt{};
    lopt.expect_container = true;

    auto ld = io.load(path, v1, lopt);
    ASSERT_FALSE(ld.has_value());
    EXPECT_EQ(ld.error().code, ErrorCode::VersionMismatch);
}

TEST_F(IoTempDir, CRC_Corruption_Detected) {
    LocalFilesystem lfs; FileIO io(lfs); ElementBinSer ser;
    auto path = dir_ / "corrupt.bin";
    std::vector<Element> elems{ {1,"H","Hydrogen",1.008},{2,"He","Helium",4.0026} };

    SaveOptions sopt{};
    sopt.wrap_with_container = true;
    sopt.ensure_directories = true;

    ASSERT_OK(io.save<Element>(path, elems, ser, sopt));

    auto bytes = lfs.read_all_bytes(path);
    ASSERT_OK(bytes);

    // Flip a byte in payload (after fixed 20-byte header)
    auto& buf = bytes.value();
    buf.at(kHeaderSize + 5) = std::byte{
        static_cast<unsigned char>(~std::to_integer<unsigned char>(buf.at(kHeaderSize + 5)))
    };
    ASSERT_OK(lfs.write_all_bytes_atomic(path, buf, true));

    LoadOptions lopt{};
    lopt.expect_container = true;

    auto ld = io.load(path, ser, lopt);
    EXPECT_FALSE(ld.has_value());
    EXPECT_EQ(ld.error().code, ErrorCode::Corrupted);
}

TEST_F(IoTempDir, RawPayload_Mode) {
    LocalFilesystem lfs; FileIO io(lfs); ElementBinSer ser;
    auto path = dir_ / "raw.bin";
    std::vector<Element> elems{ {3,"Li","Lithium",6.94} };

    SaveOptions sopt{};
    sopt.wrap_with_container = false;
    sopt.ensure_directories = true;

    ASSERT_OK(io.save<Element>(path, elems, ser, sopt));

    LoadOptions lopt{};
    lopt.expect_container = false;

    auto ld = io.load(path, ser, lopt);
    ASSERT_OK(ld);
    const auto& data = ld.value();
    EXPECT_EQ(data.size(), 1u);
    EXPECT_EQ(data.at(0).Symbol, "Li");
}

TEST(IO, MemoryFilesystem) {
    MemoryFilesystem mfs; FileIO io(mfs); ElementBinSer ser;
    std::vector<Element> elems{ {10,"Ne","Neon",20.1797} };
    ASSERT_OK(io.save<Element>("/mem/elements.bin", elems, ser));

    auto ld = io.load("/mem/elements.bin", ser);
    ASSERT_OK(ld);
    const auto& data = ld.value();
    EXPECT_EQ(data.at(0).Symbol, "Ne");
}

TEST_F(IoTempDir, JsonFastLoad_With_Container) {
    // Minimal subset of JSON used by ElementJsonSer
    LocalFilesystem lfs;
    FileIO io(lfs);
    ElementJsonSer jser;

    const char* hydrogen = R"JSON(
{
  "Symbol":"H",
  "Name":"Hydrogen",
  "AtomicNumber":1,
  "AtomicMass":1.008
}
)JSON";

    // Save as-container (exercise header path)
    {
        std::vector<Element> elems{ {1,"H","Hydrogen",1.008} };
        auto path = dir_ / "elements.jsonc";

        SaveOptions sopt{};
        sopt.wrap_with_container = true;
        sopt.ensure_directories = true;

        LoadOptions lopt{};
        lopt.expect_container = true;

        ASSERT_OK(io.save<Element>(path, elems, jser, sopt));

        auto out = io.load(path, jser, lopt);
        if (!out) {
            ADD_FAILURE() << "load() failed. code=" << static_cast<int>(out.error().code)
                << " msg=" << out.error().message;
        }
        ASSERT_OK(out);

        const auto& data = out.value();
        EXPECT_EQ(data.size(), 1u);
        EXPECT_EQ(data.at(0).Symbol, "H");
    }

    // Direct-load from raw (no container), feeding the object string
    {
        auto path = dir_ / "one_obj.json";
        std::vector<std::byte> raw(std::strlen(hydrogen));
        std::memcpy(raw.data(), hydrogen, raw.size());
        ASSERT_OK(lfs.write_all_bytes_atomic(path, raw, true));

        LoadOptions lopt{};
        lopt.expect_container = false;

        auto out = io.load(path, jser, lopt);
        ASSERT_OK(out);
        const auto& data = out.value();
        EXPECT_EQ(data.size(), 1u);
        EXPECT_EQ(data.at(0).AtomicNumber, 1);
    }
}

TEST(IO, CRC32_KnownVectors) {
    using namespace Interstellar::IO;
    std::vector<std::byte> empty;
    EXPECT_EQ(crc32(empty), 0x00000000u);               // std CRC-32 of empty with 0xFFFFFFFF init/xor

    const char* s = "123456789";
    std::span<const std::byte> bytes{
        reinterpret_cast<const std::byte*>(s), std::strlen(s)
    };
    EXPECT_EQ(crc32(bytes), 0xCBF43926u);               // well-known check vector
}


TEST(IO, Container_EmptyPayload) {
    using namespace Interstellar::IO;
    const uint32_t magic = 0xABCD1234, ver = 7;
    auto buf = pack_container(magic, ver, {});          // 20-byte header only
    ASSERT_EQ(buf.size(), kHeaderSize);

    ContainerHeader hdr{};
    auto out = unpack_container(buf, magic, ver, &hdr);
    ASSERT_TRUE(out.has_value());
    EXPECT_EQ(out->size(), 0u);
    EXPECT_EQ(hdr.magic, magic);
    EXPECT_EQ(hdr.version, ver);
    EXPECT_EQ(hdr.payload_size, 0u);
}

TEST(IO, Container_HeaderTooSmall) {
    using namespace Interstellar::IO;
    std::vector<std::byte> tiny(5); // < 20
    auto r = unpack_container(tiny, 0, 0);
    ASSERT_FALSE(r);
    EXPECT_EQ(r.error().code, ErrorCode::InvalidData);
}

TEST(IO, Container_TruncatedPayload) {
    using namespace Interstellar::IO;
    const uint32_t magic = 0x11112222, ver = 1;
    std::vector<std::byte> payload(10, std::byte{ 0xAA });
    auto buf = pack_container(magic, ver, payload);
    // Chop off last 3 payload bytes
    buf.resize(kHeaderSize + 7);

    auto r = unpack_container(buf, magic, ver);
    ASSERT_FALSE(r);
    EXPECT_EQ(r.error().code, ErrorCode::Corrupted);
}

TEST(IO, Container_MagicMismatchVsVersionMismatch) {
    using namespace Interstellar::IO;
    const uint32_t magic = 0xDEADBEEF, ver = 3;
    auto buf = pack_container(magic, ver, {});

    // Magic mismatch
    auto m = unpack_container(buf, /*expect_magic*/ 0xFEEDBEEF, /*expect_version*/ ver);
    ASSERT_FALSE(m);
    EXPECT_EQ(m.error().code, ErrorCode::VersionMismatch);

    // Version mismatch
    auto v = unpack_container(buf, /*expect_magic*/ magic, /*expect_version*/ ver + 1);
    ASSERT_FALSE(v);
    EXPECT_EQ(v.error().code, ErrorCode::VersionMismatch);
}

TEST(IO, Container_OutHeaderFilled) {
    using namespace Interstellar::IO;
    const uint32_t magic = 0xA1B2C3D4, ver = 42;
    std::vector<std::byte> payload = { std::byte{1}, std::byte{2}, std::byte{3} };
    auto buf = pack_container(magic, ver, payload);
    ContainerHeader hdr{};
    auto r = unpack_container(buf, magic, ver, &hdr);
    ASSERT_TRUE(r);
    EXPECT_EQ(hdr.magic, magic);
    EXPECT_EQ(hdr.version, ver);
    EXPECT_EQ(hdr.payload_size, payload.size());
    EXPECT_EQ(hdr.crc32, crc32(payload));
}

TEST_F(IoTempDir, Save_FailsWithoutEnsureDirectories) {
    LocalFilesystem lfs; FileIO io(lfs); ElementBinSer ser;
    std::vector<Element> elems{ {1,"H","Hydrogen",1.008} };
    auto path = dir_ / "nested" / "elements.bin";

    SaveOptions sopt{}; sopt.wrap_with_container = true; sopt.ensure_directories = false;
    auto r = io.save<Element>(path, elems, ser, sopt);
    ASSERT_FALSE(r);
    EXPECT_EQ(r.error().code, ErrorCode::IOError);
}

TEST_F(IoTempDir, LocalFilesystem_OverwriteAtomic) {
    LocalFilesystem lfs; FileIO io(lfs); ElementBinSer ser;
    auto path = dir_ / "elements.bin";

    const std::array<Element, 1> v1{ Element{1,"H","Hydrogen",1.008} };
    ASSERT_OK(io.save<Element>(path, std::span<const Element>(v1), ser));

    const std::array<Element, 1> v2{ Element{2,"He","Helium",4.0026} };
    ASSERT_OK(io.save<Element>(path, std::span<const Element>(v2), ser));

    auto out = io.load(path, ser);
    ASSERT_OK(out);
    ASSERT_EQ(out->size(), 1u);
    EXPECT_EQ(out->at(0).Symbol, "He");
}

TEST_F(IoTempDir, SaveLoad_EmptyCollection) {
    LocalFilesystem lfs; FileIO io(lfs); ElementBinSer ser;
    auto path = dir_ / "empty.bin";
    std::vector<Element> empty;

    ASSERT_OK(io.save<Element>(path, empty, ser));  // default: with container
    auto out = io.load(path, ser);
    ASSERT_OK(out);
    EXPECT_TRUE(out->empty());
}

TEST_F(IoTempDir, LocalFilesystem_ReadMissing) {
    LocalFilesystem lfs; FileIO io(lfs); ElementBinSer ser;
    auto out = io.load(dir_ / "nope.bin", ser);
    ASSERT_FALSE(out);
    EXPECT_EQ(out.error().code, ErrorCode::NotFound);
}

TEST(IO, MemoryFilesystem_ExistsAndRemoveAbsent) {
    MemoryFilesystem mfs;
    EXPECT_FALSE(mfs.exists("/mem/none.bin"));
    auto r = mfs.remove_file("/mem/none.bin");
    EXPECT_OK(r); // removing non-existent is success per contract
}

TEST(IO, MemoryFilesystem_EmptyPathErrors) {
    MemoryFilesystem mfs;
    std::vector<std::byte> b;
    auto w = mfs.write_all_bytes_atomic({}, b, true);
    ASSERT_FALSE(w);
    EXPECT_EQ(w.error().code, ErrorCode::InvalidArgument);

    auto r = mfs.read_all_bytes({});
    ASSERT_FALSE(r);
    EXPECT_EQ(r.error().code, ErrorCode::InvalidArgument);
}

TEST(IO, ElementBinSer_TrailingBytes) {
    ElementBinSer ser;
    // Encode one element, then append junk
    const std::array<Element, 1> one{ Element{1,"H","Hydrogen",1.008} };
    auto enc = ser.serialize(std::span<const Element>(one));
    ASSERT_OK(enc);
    auto buf = std::move(enc.value());
    buf.push_back(std::byte{ 0xFF }); // trailing

    auto dec = ser.deserialize(buf);
    ASSERT_FALSE(dec);
    EXPECT_EQ(dec.error().code, ErrorCode::InvalidData);
}

TEST(IO, ElementJsonSer_EscapesAndNonFinite) {
    ElementJsonSer ser;

    // Escaping check (quotes & backslash)
    std::vector<Element> elems{ {1, "H\"\\", "Hydro\n\tgen", 1.25} };
    auto enc = ser.serialize(elems);
    ASSERT_OK(enc);
    auto dec = ser.deserialize(enc.value());
    ASSERT_OK(dec);
    ASSERT_EQ(dec->size(), 1u);
    EXPECT_EQ(dec->at(0).Symbol, "H\"\\");
    EXPECT_EQ(dec->at(0).Name, "Hydro\n\tgen");

    // Non-finite number should fail
    std::vector<Element> bad{ {1, "X", "Bad", std::numeric_limits<double>::infinity()} };
    auto enc_bad = ser.serialize(bad);
    ASSERT_FALSE(enc_bad);
    EXPECT_EQ(enc_bad.error().code, ErrorCode::InvalidData);
}

TEST(IO, ElementJsonSer_UTF8_BOM) {
    ElementJsonSer ser;
    const char json[] = "\xEF\xBB\xBF{\"Symbol\":\"H\",\"Name\":\"Hydrogen\",\"AtomicNumber\":1,\"AtomicMass\":1.008}";
    std::span<const std::byte> s{ reinterpret_cast<const std::byte*>(json), sizeof(json) - 1 };
    auto dec = ser.deserialize(s);
    ASSERT_OK(dec);
    ASSERT_EQ(dec->size(), 1u);
    EXPECT_EQ(dec->at(0).Symbol, "H");
}

TEST(IO, ElementJsonSer_MissingFields) {
    ElementJsonSer ser;
    const char* bad = R"({"Symbol":"H","AtomicMass":1.0})"; // missing Name, AtomicNumber
    std::span<const std::byte> s{ reinterpret_cast<const std::byte*>(bad), std::strlen(bad) };
    auto dec = ser.deserialize(s);
    ASSERT_FALSE(dec);
    EXPECT_EQ(dec.error().code, ErrorCode::InvalidData);
}

TEST_F(IoTempDir, Save_WithInitializerList_Container) {
    LocalFilesystem lfs; FileIO io(lfs); ElementBinSer ser;
    auto path = dir_ / "initlist.bin";

    ASSERT_OK(io.save<Element>(path, { {1,"H","Hydrogen",1.008}, {2,"He","Helium",4.0026} }, ser));
    auto out = io.load(path, ser);
    ASSERT_OK(out);
    ASSERT_EQ(out->size(), 2u);
    EXPECT_EQ(out->at(0).Symbol, "H");
    EXPECT_EQ(out->at(1).Symbol, "He");
}

TEST_F(IoTempDir, Save_WithInitializerList_Raw) {
    LocalFilesystem lfs; FileIO io(lfs); ElementBinSer ser;
    auto path = dir_ / "initlist_raw.bin";

    SaveOptions sopt{}; sopt.wrap_with_container = false;
    ASSERT_OK(io.save<Element>(path, { {3,"Li","Lithium",6.94} }, ser, sopt));

    LoadOptions lopt{}; lopt.expect_container = false;
    auto out = io.load(path, ser, lopt);
    ASSERT_OK(out);
    ASSERT_EQ(out->size(), 1u);
    EXPECT_EQ(out->at(0).Symbol, "Li");
}

TEST_F(IoTempDir, Save_WithInitializerList_Empty) {
    LocalFilesystem lfs; FileIO io(lfs); ElementBinSer ser;
    auto path = dir_ / "empty_initlist.bin";

    ASSERT_OK(io.save<Element>(path, {}, ser)); // container on (default)

    auto bytes = lfs.read_all_bytes(path);
    ASSERT_OK(bytes);

    // Compute expected payload size from the serializer itself
    const std::array<Element, 0> none{};
    auto enc = ser.serialize(std::span<const Element>(none));
    ASSERT_OK(enc);

    EXPECT_EQ(bytes->size(), Interstellar::IO::kHeaderSize + enc->size()); // 20 + 4 for ElementBinSer

    auto out = io.load(path, ser);
    ASSERT_OK(out);
    EXPECT_TRUE(out->empty());
}

TEST_F(IoTempDir, Save_WithInitializerList_JSON_NonFiniteFails) {
    LocalFilesystem lfs; FileIO io(lfs); ElementJsonSer jser;
    auto path = dir_ / "bad.jsonc";

    std::vector<Element> ok{ {1,"H","Hydrogen",1.008} };
    ASSERT_OK(io.save<Element>(path, ok, jser)); // sanity

    // Now try to save bad data through the init-list overload
    auto r = io.save<Element>(path, { Element{2,"X","Bad", std::numeric_limits<double>::infinity()} }, jser);
    ASSERT_FALSE(r);
    EXPECT_EQ(r.error().code, ErrorCode::InvalidData);
}

TEST(IO, MemoryFilesystem_InitList) {
    MemoryFilesystem mfs; FileIO io(mfs); ElementBinSer ser;
    ASSERT_OK(io.save<Element>("/mem/elms.bin", { {10,"Ne","Neon",20.1797} }, ser));

    auto out = io.load("/mem/elms.bin", ser);
    ASSERT_OK(out);
    ASSERT_EQ(out->size(), 1u);
    EXPECT_EQ(out->at(0).Symbol, "Ne");
}

TEST_F(IoTempDir, LocalFilesystem_OverwriteAtomic_InitList) {
    LocalFilesystem lfs; FileIO io(lfs); ElementBinSer ser;
    auto path = dir_ / "elements.bin";

    const std::array<Element, 1> v1{ Element{1,"H","Hydrogen",1.008} };
    ASSERT_OK(io.save<Element>(path, std::span<const Element>(v1), ser));

    // exercise the init-list overload on the second write
    ASSERT_OK(io.save<Element>(path, { {2,"He","Helium",4.0026} }, ser));

    auto out = io.load(path, ser);
    ASSERT_OK(out);
    ASSERT_EQ(out->size(), 1u);
    EXPECT_EQ(out->at(0).Symbol, "He");
}

TEST(IO, Container_HeaderOnly_EmptyPayload) {
    using namespace Interstellar::IO;
    const uint32_t magic = 0xABCD1234, ver = 1;
    auto buf = pack_container(magic, ver, {}); // empty payload
    EXPECT_EQ(buf.size(), kHeaderSize);

    auto up = unpack_container(buf, magic, ver);
    ASSERT_TRUE(up.has_value());
    EXPECT_EQ(up->size(), 0u);
}
