#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <filesystem>
#include <cstddef>
#include "Result.hpp"
#include "Expected.hpp"

namespace Interstellar::IO {

    class IFilesystem {
    public:
        virtual ~IFilesystem() = default;

        // Read the whole file into memory.
        virtual expected<std::vector<std::byte>, Error>
            read_all_bytes(const std::filesystem::path& p) const = 0;

        // Atomic write: write temp in same dir, flush, then rename over target.
        virtual expected<void, Error>
            write_all_bytes_atomic(const std::filesystem::path& p,
                const std::vector<std::byte>& bytes,
                bool create_dirs = true) const = 0;

        virtual bool exists(const std::filesystem::path& p) const = 0;

        virtual expected<void, Error>
            remove_file(const std::filesystem::path& p) const = 0;
    };

    // Production implementation
    class LocalFilesystem final : public IFilesystem {
    public:
        expected<std::vector<std::byte>, Error>
            read_all_bytes(const std::filesystem::path& p) const override;

        expected<void, Error>
            write_all_bytes_atomic(const std::filesystem::path& p,
                const std::vector<std::byte>& bytes,
                bool create_dirs) const override;

        bool exists(const std::filesystem::path& p) const override;

        expected<void, Error>
            remove_file(const std::filesystem::path& p) const override;
    };

    // In-memory filesystem (great for tests/tools)
    class MemoryFilesystem final : public IFilesystem {
    public:
        expected<std::vector<std::byte>, Error>
            read_all_bytes(const std::filesystem::path& p) const override;

        expected<void, Error>
            write_all_bytes_atomic(const std::filesystem::path& p,
                const std::vector<std::byte>& bytes,
                bool /*create_dirs*/) const override;

        bool exists(const std::filesystem::path& p) const override;

        expected<void, Error>
            remove_file(const std::filesystem::path& p) const override;

    private:
        // Note: intentionally not synchronized; tests are single-threaded.
        mutable std::unordered_map<std::string, std::vector<std::byte>> files_;
    };

} // namespace Interstellar::IO
