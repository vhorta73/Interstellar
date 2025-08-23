#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <filesystem>
#include <cstddef>  // std::byte
#include <span>     // C++20
#include "Interstellar/IO/Result.hpp"
#include "Interstellar/IO/Expected.hpp"

/// \file
/// \ingroup IO
/// \brief Filesystem abstraction for reading/writing raw bytes with atomic replace.
/// \details
/// Design goals:
/// - Deterministic, platform-agnostic API for file I/O used by serializers and asset pipelines.
/// - Clear failure reporting via expected<T, Error>.
/// - Support for test doubles (in-memory filesystem).
///
/// Error policy:
/// - Implementations should avoid throwing for I/O failures and return Error.
/// - Memory allocation failures may throw std::bad_alloc.
///
/// Atomicity:
/// - write_all_bytes_atomic() writes to a temporary file in the same directory,
///   flushes data and metadata, then atomically replaces the target when supported
///   (POSIX rename; Windows ReplaceFileW). See class docs for platform nuances.
///
/// Thread-safety:
/// - IFilesystem has no inherent synchronization. Implementations may or may not be thread-safe.
///   MemoryFilesystem is not thread-safe.

namespace Interstellar::IO {

    /**
     * @ingroup IO
     * @brief Filesystem interface for whole-file byte I/O.
     * @note Paths are treated as files. Behavior on directories is implementation-defined.
     * @since 1.0
     */
    class IFilesystem {
    public:
        virtual ~IFilesystem() = default;

        /**
         * @ingroup IO
         * @brief Read the entire file into memory.
         * @param p Target file path (must refer to a regular file).
         * @return expected<vector<byte>, Error> - On success, a buffer of file bytes; otherwise Error.
         * @pre p is non-empty.
         * @post Returned buffer size equals file size at read time.
         * @throws std::bad_alloc May allocate up to the file size.
         * @thread_safety Depends on implementation; interface itself is reentrant.
         * @complexity O(N) in file size.
         * @since 1.0
         */
        [[nodiscard]]
        virtual expected<std::vector<std::byte>, Error>
            read_all_bytes(const std::filesystem::path& p) const = 0;

        /**
         * @ingroup IO
         * @brief Atomically write the full file contents by replace-in-place.
         * @param p Target file path.
         * @param bytes Buffer to write (full contents).
         * @param create_dirs If true, create parent directories as needed.
         * @return expected<void, Error> - Success or Error.
         * @details Intended steps:
         *  1) Create a unique temp file in the same directory as p.
         *  2) Write bytes completely; flush file buffers to disk.
         *  3) Flush the parent directory entry (best effort).
         *  4) Atomically replace p with the temp file (POSIX: rename; Windows: ReplaceFileW).
         * @pre p is non-empty.
         * @note If atomic replace is not supported by the platform/filesystem, implementation should fail with a descriptive Error.
         * @throws std::bad_alloc May allocate for small internal buffers or path conversions.
         * @thread_safety Depends on implementation; interface itself is reentrant.
         * @complexity O(N) in bytes size.
         * @since 1.0
         */
        [[nodiscard]]
        virtual expected<void, Error>
            write_all_bytes_atomic(const std::filesystem::path& p,
                std::span<const std::byte> bytes,
                bool create_dirs = true) const = 0;

        /**
         * @ingroup IO
         * @brief Check if a regular file exists at p.
         * @param p Path to test.
         * @return true if a regular file exists; false otherwise.
         * @since 1.0
         */
        [[nodiscard]]
        virtual bool exists(const std::filesystem::path& p) const noexcept = 0;

        /**
         * @ingroup IO
         * @brief Remove a file if it exists.
         * @param p Path to remove.
         * @return expected<void, Error> - Success or Error. Removing a non-existent path should return success.
         * @throws std::bad_alloc Only on internal string/path conversions, if any.
         * @since 1.0
         */
        [[nodiscard]]
        virtual expected<void, Error>
            remove_file(const std::filesystem::path& p) const = 0;

        /**
         * @ingroup IO
         * @brief Convenience overload that accepts a std::vector buffer.
         * @param p Target file path.
         * @param bytes Buffer to write (full contents).
         * @param create_dirs If true, create parent directories as needed.
         * @return expected<void, Error>
         * @since 1.0
         */
        [[nodiscard]]
        expected<void, Error>
            write_all_bytes_atomic(const std::filesystem::path& p,
                const std::vector<std::byte>& bytes,
                bool create_dirs = true) const
        {
            return write_all_bytes_atomic(
                p,
                std::span<const std::byte>(bytes.data(), bytes.size()),
                create_dirs
            );
        }
    };

    /**
     * @ingroup IO
     * @brief Production implementation backed by the host OS.
     * @details
     * - Uses std::filesystem with error-code overloads to avoid exceptions for I/O failures.
     * - Atomic replace:
     *   - POSIX: ::rename overwrites atomically; implementation should fsync file and parent dir.
     *   - Windows: use ReplaceFileW for atomic replacement; std::filesystem::rename may fail if target exists.
     * - exists(p) returns true only for regular files.
     * @thread_safety Thread-safe for independent paths; no internal shared state.
     * @since 1.0
     */
    class LocalFilesystem final : public IFilesystem {
    public:
        [[nodiscard]]
        expected<std::vector<std::byte>, Error>
            read_all_bytes(const std::filesystem::path& p) const override;

        [[nodiscard]]
        expected<void, Error>
            write_all_bytes_atomic(const std::filesystem::path& p,
                std::span<const std::byte> bytes,
                bool create_dirs) const override;

        [[nodiscard]]
        bool exists(const std::filesystem::path& p) const noexcept override;

        [[nodiscard]]
        expected<void, Error>
            remove_file(const std::filesystem::path& p) const override;
    };

    /**
     * @ingroup IO
     * @brief In-memory filesystem (ideal for tests and tooling).
     * @details
     * - Stores byte buffers keyed by a normalized path string.
     * - No persistence; process-local only.
     * - Not thread-safe by design.
     * @since 1.0
     */
    class MemoryFilesystem final : public IFilesystem {
    public:
        [[nodiscard]]
        expected<std::vector<std::byte>, Error>
            read_all_bytes(const std::filesystem::path& p) const override;

        [[nodiscard]]
        expected<void, Error>
            write_all_bytes_atomic(const std::filesystem::path& p,
                std::span<const std::byte> bytes,
                bool /*create_dirs*/) const override;

        [[nodiscard]]
        bool exists(const std::filesystem::path& p) const noexcept override;

        [[nodiscard]]
        expected<void, Error>
            remove_file(const std::filesystem::path& p) const override;

    private:
        /// Normalize a path to a stable, portable key (generic, lexically normalized).
        static std::string make_key(const std::filesystem::path& p);

        // Intentionally not synchronized; tests are single-threaded.
        // All methods are const; map is mutable to allow modification through const interface.
        mutable std::unordered_map<std::string, std::vector<std::byte>> files_;
    };

} // namespace Interstellar::IO
