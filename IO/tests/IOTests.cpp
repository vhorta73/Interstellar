#include <gtest/gtest.h>

#include <filesystem>
#include <string>
#include <vector>
#include <span>
#include <cstring>
#include <simdjson.h>

#include "Interstellar/IO/FileIO.hpp"
#include "Interstellar/IO/IFilesystem.hpp"

// pull in the factory (avoids MSVC's ::unexpected clash)
using Interstellar::IO::make_unexpected;

using namespace Interstellar::IO;

// -------------------------
// Test domain types
// -------------------------
struct Element {
    int           AtomicNumber{};
    std::string   Symbol;
    std::string   Name;
    double        AtomicMass{};
};

// -------------------------
// Minimal binary serializer (test helper)
// -------------------------
class ElementBinSer : public ISerializer<Element> {
public:
    uint32_t schema_version() const override { return 1u; }
    uint32_t magic_u32()      const override { return 0x454C4D31u; /* 'ELM1' */ }

    expected<std::vector<std::byte>, Error>
        serialize(std::span<const Element> items) const override {
        std::vector<std::byte> out;
        auto put_u32 = [&](uint32_t v) { size_t i = out.size(); out.resize(i + 4); std::memcpy(out.data() + i, &v, 4); };
        auto put_f64 = [&](double   v) { size_t i = out.size(); out.resize(i + 8); std::memcpy(out.data() + i, &v, 8); };
        auto put_str = [&](const std::string& s) {
            put_u32(static_cast<uint32_t>(s.size()));
            size_t i = out.size(); out.resize(i + s.size());
            std::memcpy(out.data() + i, s.data(), s.size());
            };

        put_u32(static_cast<uint32_t>(items.size()));
        for (const auto& e : items) {
            put_u32(static_cast<uint32_t>(e.AtomicNumber));
            put_str(e.Symbol);
            put_str(e.Name);
            put_f64(e.AtomicMass);
        }
        return out;
    }

    expected<std::vector<Element>, Error>
        deserialize(std::span<const std::byte> bytes) const override {
        const uint8_t* p = reinterpret_cast<const uint8_t*>(bytes.data());
        const uint8_t* end = p + bytes.size();

        auto get_u32 = [&]() -> expected<uint32_t, Error> {
            if (end - p < 4) return make_unexpected(Error{ ErrorCode::InvalidData, "eof u32" });
            uint32_t v; std::memcpy(&v, p, 4); p += 4; return v;
            };
        auto get_f64 = [&]() -> expected<double, Error> {
            if (end - p < 8) return make_unexpected(Error{ ErrorCode::InvalidData, "eof f64" });
            double v; std::memcpy(&v, p, 8); p += 8; return v;
            };
        auto get_str = [&]() -> expected<std::string, Error> {
            auto lenR = get_u32(); if (!lenR) return make_unexpected(lenR.error());
            uint32_t len = lenR.value();
            if (static_cast<size_t>(end - p) < len) return make_unexpected(Error{ ErrorCode::InvalidData, "eof str" });
            std::string s(reinterpret_cast<const char*>(p), reinterpret_cast<const char*>(p) + len);
            p += len; return s;
            };

        std::vector<Element> out;
        auto nR = get_u32(); if (!nR) return make_unexpected(nR.error());
        out.reserve(nR.value());
        for (uint32_t i = 0; i < nR.value(); ++i) {
            auto zR = get_u32(); if (!zR)    return make_unexpected(zR.error());
            auto symR = get_str(); if (!symR)  return make_unexpected(symR.error());
            auto nameR = get_str(); if (!nameR) return make_unexpected(nameR.error());
            auto massR = get_f64(); if (!massR) return make_unexpected(massR.error());
            out.push_back(Element{ static_cast<int>(zR.value()), symR.value(), nameR.value(), massR.value() });
        }
        if (p != end) return make_unexpected(Error{ ErrorCode::InvalidData, "trailing bytes" });
        return out;
    }
};

// -------------------------
// JSON serializer (fast-load via simdjson, pretty-save via minimal JSON)
// -------------------------
class ElementJsonSer : public ISerializer<Element> {
public:
    uint32_t schema_version() const override { return 1u; }
    uint32_t magic_u32()      const override { return 0x4A534F4Eu; /* 'JSON' */ }

    expected<std::vector<std::byte>, Error>
        serialize(std::span<const Element> items) const override {
        std::string s;
        s += "[";
        for (size_t i = 0; i < items.size(); ++i) {
            const auto& e = items[i];
            s += "{";
            s += "\"Symbol\":\"" + e.Symbol + "\",";
            s += "\"Name\":\"" + e.Name + "\",";
            s += "\"AtomicNumber\":" + std::to_string(e.AtomicNumber) + ",";
            s += "\"AtomicMass\":" + std::to_string(e.AtomicMass);
            s += "}";
            if (i + 1 < items.size()) s += ",";
        }
        s += "]";
        std::vector<std::byte> out(s.size());
        std::memcpy(out.data(), s.data(), s.size());
        return out;
    }

    expected<std::vector<Element>, Error>
        deserialize(std::span<const std::byte> bytes) const override {
        // --- 1) Skip UTF-8 BOM and leading whitespace (if any)
        const uint8_t* b = reinterpret_cast<const uint8_t*>(bytes.data());
        size_t n = bytes.size();

        // UTF-8 BOM: EF BB BF
        if (n >= 3 && b[0] == 0xEF && b[1] == 0xBB && b[2] == 0xBF) {
            b += 3; n -= 3;
        }

        // Skip ASCII whitespace (space, tab, CR, LF)
        while (n > 0 && (*b == ' ' || *b == '\t' || *b == '\r' || *b == '\n')) {
            ++b; --n;
        }

        // Quick sanity check
        if (n == 0) {
            return make_unexpected(Error{ ErrorCode::InvalidData, "Empty JSON after trimming" });
        }

        // --- 2) Make a padded copy for ondemand (portable across simdjson versions)
        simdjson::padded_string ps(n);            // allocates n + SIMDJSON_PADDING
        std::memcpy(ps.data(), b, n);             // copy JSON bytes

        simdjson::ondemand::parser parser;
        auto doc_result = parser.iterate(ps);
        if (doc_result.error()) {
            return make_unexpected(Error{ ErrorCode::InvalidData, "Invalid JSON (iterate)" });
        }
        // document is move-only - take it by move
        simdjson::ondemand::document doc = std::move(doc_result.value());

        std::vector<Element> out;

        // --- 3) Detect top-level type and parse
        auto t_res = doc.type();
        if (t_res.error()) {
            return make_unexpected(Error{ ErrorCode::InvalidData, "Invalid JSON (type())" });
        }
        auto t = t_res.value();

        if (t == simdjson::ondemand::json_type::array) {
            auto arr_res = doc.get_array();
            if (arr_res.error()) {
                return make_unexpected(Error{ ErrorCode::InvalidData, "Invalid JSON array" });
            }
            for (auto v : arr_res.value()) {
                auto obj_res = v.get_object();
                if (obj_res.error()) {
                    return make_unexpected(Error{ ErrorCode::InvalidData, "Array element not object" });
                }
                auto eR = parse_one(obj_res.value());
                if (!eR) return make_unexpected(eR.error());
                out.push_back(std::move(eR.value()));
            }
        }
        else if (t == simdjson::ondemand::json_type::object) {
            auto obj_res = doc.get_object();
            if (obj_res.error()) {
                return make_unexpected(Error{ ErrorCode::InvalidData, "Invalid JSON object" });
            }
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

// -------------------------
// Tests
// -------------------------

TEST(IO, Roundtrip_Local_Binary_With_Container) {
    LocalFilesystem lfs;
    FileIO io(lfs);
    ElementBinSer ser;

    std::vector<Element> elems{ {1,"H","Hydrogen",1.008},{2,"He","Helium",4.0026} };
    auto path = std::filesystem::temp_directory_path() / "io_test" / "elements.bin";

    SaveOptions sopt{};
    sopt.wrap_with_container = true;
    sopt.ensure_directories = true;

    LoadOptions lopt{};
    lopt.expect_container = true;

    ASSERT_TRUE(io.save<Element>(path, elems, ser, sopt));
    auto loaded = io.load(path, ser, lopt);
    ASSERT_TRUE(loaded);
    const auto& items = loaded.value();
    EXPECT_EQ(items.size(), elems.size());
    EXPECT_EQ(items.at(0).Symbol, "H");
}

TEST(IO, VersionMismatch) {
    LocalFilesystem lfs; FileIO io(lfs);
    struct V2Ser final : ElementBinSer { uint32_t schema_version() const override { return 2u; } } v2;
    ElementBinSer v1;

    auto path = std::filesystem::temp_directory_path() / "io_test" / "v2.bin";
    std::vector<Element> elems{ {1,"H","Hydrogen",1.008} };

    SaveOptions sopt{};
    sopt.wrap_with_container = true;
    sopt.ensure_directories = true;

    ASSERT_TRUE(io.save<Element>(path, elems, v2, sopt));

    LoadOptions lopt{};
    lopt.expect_container = true;

    auto ld = io.load(path, v1, lopt);
    ASSERT_FALSE(ld.has_value());
    EXPECT_EQ(ld.error().code, ErrorCode::VersionMismatch);
}

TEST(IO, CRC_Corruption_Detected) {
    LocalFilesystem lfs; FileIO io(lfs); ElementBinSer ser;
    auto path = std::filesystem::temp_directory_path() / "io_test" / "corrupt.bin";
    std::vector<Element> elems{ {1,"H","Hydrogen",1.008},{2,"He","Helium",4.0026} };

    SaveOptions sopt{};
    sopt.wrap_with_container = true;
    sopt.ensure_directories = true;

    ASSERT_TRUE(io.save<Element>(path, elems, ser, sopt));

    auto bytes = lfs.read_all_bytes(path);
    ASSERT_TRUE(bytes);

    // flip a byte in payload
    auto& buf = bytes.value();
    buf.at(sizeof(ContainerHeader) + 5) = std::byte{
        static_cast<unsigned char>(~(unsigned char)buf.at(sizeof(ContainerHeader) + 5))
    };
    ASSERT_TRUE(lfs.write_all_bytes_atomic(path, buf, true));

    LoadOptions lopt{};
    lopt.expect_container = true;

    auto ld = io.load(path, ser, lopt);
    EXPECT_FALSE(ld.has_value());
    EXPECT_EQ(ld.error().code, ErrorCode::Corrupted);
}

TEST(IO, RawPayload_Mode) {
    LocalFilesystem lfs; FileIO io(lfs); ElementBinSer ser;
    auto path = std::filesystem::temp_directory_path() / "io_test" / "raw.bin";
    std::vector<Element> elems{ {3,"Li","Lithium",6.94} };

    SaveOptions sopt{};
    sopt.wrap_with_container = false;
    sopt.ensure_directories = true;

    ASSERT_TRUE(io.save<Element>(path, elems, ser, sopt));

    LoadOptions lopt{};
    lopt.expect_container = false;

    auto ld = io.load(path, ser, lopt);
    ASSERT_TRUE(ld);
    const auto& data = ld.value();
    EXPECT_EQ(data.size(), 1u);
    EXPECT_EQ(data.at(0).Symbol, "Li");
}

TEST(IO, MemoryFilesystem) {
    MemoryFilesystem mfs; FileIO io(mfs); ElementBinSer ser;
    std::vector<Element> elems{ {10,"Ne","Neon",20.1797} };
    ASSERT_TRUE(io.save<Element>("/mem/elements.bin", elems, ser));

    auto ld = io.load("/mem/elements.bin", ser);
    ASSERT_TRUE(ld);
    const auto& data = ld.value();
    EXPECT_EQ(data.at(0).Symbol, "Ne");
}

TEST(IO, JsonFastLoad_With_Container) {
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
        auto path = std::filesystem::temp_directory_path() / "io_test" / "elements.jsonc";

        SaveOptions sopt{};
        sopt.wrap_with_container = true;
        sopt.ensure_directories = true;

        LoadOptions lopt{};
        lopt.expect_container = true;

        ASSERT_TRUE(io.save<Element>(path, elems, jser, sopt));

        auto out = io.load(path, jser, lopt);
        if (!out) {
            ADD_FAILURE() << "load() failed. code=" << static_cast<int>(out.error().code)
                << " msg=" << out.error().message;
        }
        ASSERT_TRUE(out);

        const auto& data = out.value();
        EXPECT_EQ(data.size(), 1u);
        EXPECT_EQ(data.at(0).Symbol, "H");
    }

    // Direct-load from raw (no container), feeding the object string
    {
        auto path = std::filesystem::temp_directory_path() / "io_test" / "one_obj.json";
        std::vector<std::byte> raw(strlen(hydrogen));
        std::memcpy(raw.data(), hydrogen, raw.size());
        ASSERT_TRUE(lfs.write_all_bytes_atomic(path, raw, true));

        LoadOptions lopt{};
        lopt.expect_container = false;

        auto out = io.load(path, jser, lopt);
        ASSERT_TRUE(out);
        const auto& data = out.value();
        EXPECT_EQ(data.size(), 1u);
        EXPECT_EQ(data.at(0).AtomicNumber, 1);
    }
}
