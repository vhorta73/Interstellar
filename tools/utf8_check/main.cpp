#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <iomanip>

namespace fs = std::filesystem;

// Mapping: single 8-bit byte -> replacement string (written to file).
// Keep replacements ASCII if you want to avoid introducing Unicode.
static const std::unordered_map<unsigned char, std::string> kFixupMap = {
    {0x91, "'"},   // left single quote
    {0x92, "'"},   // right single quote
    {0x93, "\""},  // left double quote
    {0x94, "\""},  // right double quote
    {0x95, "*"},   // bullet
    {0x96, "-"},   // en dash
    {0x97, "-"},   // em dash
    {0x85, "..."}, // ellipsis
    {0xA0, " "},   // non-breaking space -> regular space
    {0xB1, "+/-"}, // plus-minus
    {0xB5, "u"},   // micro sign -> 'u'
    {0xB7, "*"},   // middle dot
    {0xF7, "/"},   // division sign
    {0xE9, "e"},   // e acute -> e
    {0xE8, "e"},   // e grave -> e
    {0xEA, "e"},   // e circumflex -> e
    {0xEB, "e"},   // e diaeresis -> e
    {0xE4, "a"},   // a diaeresis -> a
    {0xE6, "ae"},  // ae ligature -> ae
    {0xF6, "o"},   // o diaeresis -> o
    {0xFC, "u"},   // u diaeresis -> u
    {0xC4, "A"},   // A diaeresis -> A
    {0xD6, "O"},   // O diaeresis -> O
    {0xDC, "U"},   // U diaeresis -> U
    {0xDF, "ss"},  // sharp s -> ss
    // Add more as needed. Example: {0x80, "EUR"} for euro sign.
};

static std::string hex_byte(unsigned char b) {
    std::ostringstream oss;
    oss << std::uppercase << std::hex << std::setfill('0') << std::setw(2)
        << static_cast<unsigned int>(b);
    return oss.str();
}

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
    if (is_under(p, build_root)) return true;
    for (auto& seg : p) {
        auto s = seg.string();
        if (s == "_deps" || s == "third_party" || s == "external" ||
            s == "deps" || s == "vendor" || s == "out")
            return true;
    }
    return false;
}

// Attempt to fix buffer using kFixupMap.
// Changes: now auto-strips a leading UTF-8 BOM instead of failing.
static bool try_fix_with_map(const std::vector<unsigned char>& in,
    std::vector<unsigned char>& out,
    size_t& first_bad_off)
{
    out.clear();

    // Start index: skip BOM if present (EF BB BF)
    size_t i = 0;
    if (in.size() >= 3 && in[0] == 0xEF && in[1] == 0xBB && in[2] == 0xBF) {
        i = 3; // drop BOM
    }

    for (; i < in.size();) {
        unsigned char c = in[i];
        if (c <= 0x7F) {
            out.push_back(c);
            ++i;
            continue;
        }

        size_t need = 0;
        if ((c & 0xE0) == 0xC0) { need = 1; if ((c & 0x1E) == 0) { need = (size_t)-1; } }
        else if ((c & 0xF0) == 0xE0) { need = 2; }
        else if ((c & 0xF8) == 0xF0) { need = 3; if ((c & 0x07) > 0x04) { need = (size_t)-1; } }
        else { need = (size_t)-1; }

        if (need != (size_t)-1 && i + need < in.size()) {
            bool ok = true;
            for (size_t k = 1; k <= need; ++k) if ((in[i + k] & 0xC0) != 0x80) { ok = false; break; }
            if (ok) {
                for (size_t k = 0; k <= need; ++k) out.push_back(in[i + k]);
                i += 1 + need;
                continue;
            }
        }

        auto it = kFixupMap.find(c);
        if (it != kFixupMap.end()) {
            const std::string& repl = it->second;
            out.insert(out.end(), repl.begin(), repl.end());
            ++i;
            continue;
        }

        first_bad_off = i;
        out.clear();
        return false;
    }

    return true;
}

int main(int argc, char** argv) {
    const fs::path source_root = (argc > 1) ? fs::path(argv[1]) : fs::current_path();
    const fs::path build_root = (argc > 2) ? fs::path(argv[2]) : fs::path(); // optional

    std::vector<fs::path> bad;
    size_t fixed_count = 0;

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
            std::vector<unsigned char> fixed;
            size_t first_bad = 0;
            if (try_fix_with_map(buf, fixed, first_bad)) {
                if (fixed != buf) {
                    std::ofstream out(p, std::ios::binary | std::ios::trunc);
                    if (!out) {
                        std::cerr << "warn: cannot write " << p << " after fix attempt\n";
                        bad.push_back(p);
                        std::cout << "== ENCODING ERRORS: [X] " << p.generic_string()
                            << " : bad encoding at byte " << off << "\n";
                        continue;
                    }
                    out.write(reinterpret_cast<const char*>(fixed.data()), static_cast<std::streamsize>(fixed.size()));
                    ++fixed_count;
                    std::cout << "== FIXED VIA MAP: [OK] " << p.generic_string() << "\n";
                }
            }
            else {
                size_t where = first_bad < buf.size() ? first_bad : off;
                unsigned char b = (where < buf.size() ? buf[where] : 0);
                std::cout << "== ENCODING ERRORS: [X] " << p.generic_string()
                    << " : bad encoding at byte " << where
                    << " (byte 0x" << hex_byte(b) << " has no map or is invalid)\n";
                bad.push_back(p);
            }
        }
    }

    if (!bad.empty()) {
        std::cout << "\nFound " << bad.size() << " file(s) that are not UTF-8 without BOM (and not fully fixable via map).\n";
        std::cout << "Tip: extend kFixupMap to auto-replace known single-byte offenders, or convert the file to UTF-8.\n";
        return 1;
    }

    if (fixed_count) {
        std::cout << "\nAuto-fixed " << fixed_count << " file(s) using map replacements.\n";
    }

    return 0;
}
