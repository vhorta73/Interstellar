#include <unordered_map>
#include <string>
#include <span>
#include <vector>
#include <filesystem>
#include "Interstellar/IO/IFilesystem.hpp"

namespace Interstellar::IO {

    std::string MemoryFilesystem::make_key(const std::filesystem::path& p) {
        auto u8 = p.lexically_normal().generic_u8string(); // std::u8string
        return std::string(u8.begin(), u8.end());          // UTF-8 narrow copy
    }

    expected<std::vector<std::byte>, Error>
        MemoryFilesystem::read_all_bytes(const std::filesystem::path& p) const {
        if (p.empty()) {
            return unexpected<Error>({ ErrorCode::InvalidArgument, "Path is empty" });
        }
        const auto k = make_key(p);
        auto it = files_.find(k);
        if (it == files_.end()) {
            return unexpected<Error>({ ErrorCode::NotFound, "Missing: " + k });
        }
        return it->second;
    }

    expected<void, Error>
        MemoryFilesystem::write_all_bytes_atomic(const std::filesystem::path& p,
            std::span<const std::byte> bytes,
            bool /*create_dirs*/) const {
        if (p.empty()) {
            return unexpected<Error>({ ErrorCode::InvalidArgument, "Path is empty" });
        }
        const auto k = make_key(p);
        files_.insert_or_assign(k, std::vector<std::byte>(bytes.begin(), bytes.end()));
        return {};
    }

    bool MemoryFilesystem::exists(const std::filesystem::path& p) const noexcept {
        if (p.empty()) return false;
        const auto k = make_key(p);
        return files_.find(k) != files_.end();
    }

    expected<void, Error>
        MemoryFilesystem::remove_file(const std::filesystem::path& p) const {
        if (p.empty()) return {};
        const auto k = make_key(p);
        files_.erase(k);
        return {};
    }

} // namespace Interstellar::IO
