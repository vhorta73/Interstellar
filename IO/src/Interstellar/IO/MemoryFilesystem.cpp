#include <unordered_map>
#include <string>
#include <vector>
#include <filesystem>
#include "Interstellar/IO/IFilesystem.hpp"

namespace Interstellar::IO {

    static std::string norm(const std::filesystem::path& p) {
        return std::filesystem::path(p).generic_string();
    }

    expected<std::vector<std::byte>, Error>
        MemoryFilesystem::read_all_bytes(const std::filesystem::path& p) const {
        const auto k = norm(p);
        auto it = files_.find(k);
        if (it == files_.end())
            return unexpected<Error>({ ErrorCode::NotFound, "Missing: " + k });
        return it->second;
    }

    expected<void, Error>
        MemoryFilesystem::write_all_bytes_atomic(const std::filesystem::path& p,
            const std::vector<std::byte>& bytes,
            bool) const {
        files_[norm(p)] = bytes;
        return {};
    }

    bool MemoryFilesystem::exists(const std::filesystem::path& p) const {
        return files_.count(norm(p)) > 0;
    }

    expected<void, Error>
        MemoryFilesystem::remove_file(const std::filesystem::path& p) const {
        files_.erase(norm(p));
        return {};
    }

} // namespace Interstellar::IO
