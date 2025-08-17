#include <fstream>
#include <random>
#include <system_error>
#include <filesystem>
#include <vector>

#include "Interstellar/IO/IFilesystem.hpp"

namespace Interstellar::IO {

    static expected<void, Error> ensure_dir(const std::filesystem::path& dir) {
        std::error_code ec;
        if (dir.empty()) return {};
        if (std::filesystem::exists(dir, ec)) return {};
        if (std::filesystem::create_directories(dir, ec)) return {};
        return unexpected<Error>({ ErrorCode::IOError, "Failed to create directories: " + dir.string() });
    }

    static std::filesystem::path unique_temp_path_in(const std::filesystem::path& dir,
        const std::string& base) {
        std::random_device rd; std::mt19937_64 gen(rd());
        for (int i = 0; i < 32; ++i) {
            auto tmp = dir / (base + ".tmp." + std::to_string(gen()));
            std::error_code ec; if (!std::filesystem::exists(tmp, ec)) return tmp;
        }
        return dir / (base + ".tmp.fallback");
    }

    expected<std::vector<std::byte>, Error>
        LocalFilesystem::read_all_bytes(const std::filesystem::path& p) const {
        std::ifstream f(p, std::ios::binary);
        if (!f) return unexpected<Error>({ ErrorCode::NotFound, "Cannot open file: " + p.string() });
        f.seekg(0, std::ios::end);
        auto size = f.tellg();
        if (size < 0) return unexpected<Error>({ ErrorCode::IOError, "tellg failed" });
        f.seekg(0, std::ios::beg);
        std::vector<std::byte> data(static_cast<size_t>(size));
        if (!f.read(reinterpret_cast<char*>(data.data()), size))
            return unexpected<Error>({ ErrorCode::IOError, "Read failed: " + p.string() });
        return data;
    }

    expected<void, Error>
        LocalFilesystem::write_all_bytes_atomic(const std::filesystem::path& p,
            const std::vector<std::byte>& bytes,
            bool create_dirs) const {
        std::error_code ec;
        const auto dir = p.parent_path();
        if (create_dirs) {
            auto ed = ensure_dir(dir); if (!ed) return ed;
        }
        auto temp = unique_temp_path_in(dir, p.filename().string());

        // write temp
        {
            std::ofstream f(temp, std::ios::binary | std::ios::trunc);
            if (!f) return unexpected<Error>({ ErrorCode::IOError, "Cannot open temp file: " + temp.string() });
            if (!f.write(reinterpret_cast<const char*>(bytes.data()),
                static_cast<std::streamsize>(bytes.size()))) {
                std::filesystem::remove(temp, ec);
                return unexpected<Error>({ ErrorCode::IOError, "Write failed: " + temp.string() });
            }
            f.flush();
            // We don't attempt platform-specific fsync here; rename is atomic within same FS.
        }

        // rename over target
        std::filesystem::rename(temp, p, ec);
        if (ec) {
            std::filesystem::remove(temp, ec);
            return unexpected<Error>({ ErrorCode::IOError,
                                      "Rename failed: " + temp.string() + " -> " + p.string() });
        }
        return {};
    }

    bool LocalFilesystem::exists(const std::filesystem::path& p) const {
        std::error_code ec; return std::filesystem::exists(p, ec);
    }

    expected<void, Error>
        LocalFilesystem::remove_file(const std::filesystem::path& p) const {
        std::error_code ec; std::filesystem::remove(p, ec);
        if (ec) return unexpected<Error>({ ErrorCode::IOError, "Remove failed: " + p.string() });
        return {};
    }

} // namespace Interstellar::IO
