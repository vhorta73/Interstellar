#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

static bool is_utf8_no_bom(const std::vector<unsigned char>& b, size_t& bad_off) {
    if (b.size() >= 3 && b[0] == 0xEF && b[1] == 0xBB && b[2] == 0xBF) { bad_off = 0; return false; }

    for (size_t i = 0; i < b.size();) {
        unsigned char c = b[i];
        if (c <= 0x7F) { ++i; continue; }

        size_t need = 0;
        if ((c & 0xE0) == 0xC0) { need = 1; if ((c & 0x1E) == 0) { bad_off = i; return false; } }
        else if ((c & 0xF0) == 0xE0) { need = 2; }
        else if ((c & 0xF8) == 0xF0) { need = 3; if ((c & 0x07) > 0x04) { bad_off = i; return false; } }
        else { bad_off = i; return false; }

        if (i + need >= b.size()) { bad_off = i; return false; }
        for (size_t k = 1; k <= need; ++k) if ((b[i + k] & 0xC0) != 0x80) { bad_off = i + k; return false; }

        // (Optional) quick surrogate/overlong guard can be added, but above is enough for our use.
        i += 1 + need;
    }
    return true;
}

static bool has_source_ext(const fs::path& p) {
    static const char* exts[] = { ".h", ".hpp", ".hh", ".inl", ".c", ".cc", ".cpp" };
    auto e = p.extension().string();
    for (auto* x : exts) if (e == x) return true;
    return false;
}

static bool is_under(const fs::path& p, const fs::path& root) {
    if (root.empty()) return false;
    std::error_code ec;
    auto rel = fs::weakly_canonical(p, ec);
    auto r = fs::weakly_canonical(root, ec);
    if (ec) return false;
    auto itp = rel.begin(), itr = r.begin();
    for (; itr != r.end(); ++itr, ++itp) {
        if (itp == rel.end() || *itp != *itr) return false;
    }
    return true;
}

static bool should_skip(const fs::path& p, const fs::path& build_root) {
    // Skip anything inside the build dir
    if (is_under(p, build_root)) return true;

    // Skip common third-party dirs by segment name
    for (auto& seg : p) {
        auto s = seg.string();
        if (s == "_deps" || s == "third_party" || s == "external" ||
            s == "deps" || s == "vendor" || s == "out")
            return true;
    }
    return false;
}

int main(int argc, char** argv) {
    const fs::path source_root = (argc > 1) ? fs::path(argv[1]) : fs::current_path();
    const fs::path build_root = (argc > 2) ? fs::path(argv[2]) : fs::path(); // optional

    std::vector<fs::path> bad;

    for (fs::recursive_directory_iterator it(source_root), end; it != end; ++it) {
        const auto& p = it->path();
        if (!it->is_regular_file()) continue;
        if (should_skip(p, build_root)) continue;
        if (!has_source_ext(p)) continue;

        std::ifstream f(p, std::ios::binary);
        if (!f) { std::cerr << "warn: cannot open " << p << "\n"; continue; }
        std::vector<unsigned char> buf((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());

        size_t off = 0;
        if (!is_utf8_no_bom(buf, off)) {
            bad.push_back(p);
            std::cout << "== ENCODING ERRORS: [X] " << p.generic_string() << " : bad encoding at byte " << off << "\n";
        }
    }

    if (!bad.empty()) {
        std::cout << "\nFound " << bad.size() << " file(s) that are not UTF-8 without BOM.\n";
        std::cout << "Tip: convert to UTF-8 (no BOM) or replace Windows-1252 punctuation with ASCII.\n";
        return 1;
    }

    return 0;
}
