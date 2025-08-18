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
