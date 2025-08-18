#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <filesystem>
#include <cstddef>
#include <span>            // C++20
#include "Result.hpp"
#include "Expected.hpp"

/// \file Filesystem.hpp
/// \brief Filesystem abstraction for reading/writing raw bytes with atomic replace.
/// \details
/// ### Design goals
/// - Deterministic, platform-agnostic API for file I/O used by serializers and asset pipelines.
/// - Clear failure reporting via `expected<T, Error>`.
/// - Support for test doubles (in-memory filesystem).
/// ### Error policy
/// - Implementations SHOULD avoid throwing for I/O failures and return `Error`.
/// - Memory allocation failures may throw `std::bad_alloc`.
/// ### Atomicity
/// - `write_all_bytes_atomic()` writes to a temporary file in the **same directory**,
///   flushes data and metadata, then atomically replaces the target when supported
///   (POSIX `rename`; Windows `ReplaceFileW`). See class docs for platform nuances.
/// ### Thread-safety
/// - `IFilesystem` has no inherent synchronization. Implementations may or may not be thread-safe.
///   `MemoryFilesystem` is **not** thread-safe.

namespace Interstellar::IO {

    /**
     * @brief Filesystem interface for whole-file byte I/O.
     * @note Paths are treated as files. Behavior on directories is implementation-defined and documented below.
     */
    class IFilesystem {
    public:
        virtual ~IFilesystem() = default;

        /**
         * @brief Read the entire file into memory.
         * @param p Target file path (must refer to a regular file).
         * @return On success, a buffer of file bytes. On failure, an Error.
         * @pre `p` is non-empty.
         * @post Returned buffer size equals file size at read time.
         * @warning May allocate up to the size of the file; `std::bad_alloc` may be thrown.
         * @thread_safety Not thread-safe by itself; see implementation notes.
         */
        [[nodiscard]]
        virtual expected<std::vector<std::byte>, Error>
            read_all_bytes(const std::filesystem::path& p) const = 0;

        /**
         * @brief Atomically write the full file contents by replace-in-place.
         * @param p Target file path.
         * @param bytes Buffer to write (full contents).
         * @param create_dirs If true, create parent directories as needed.
         * @return Success or Error.
         * @details
         * Steps (intended):
         *  1. Create a unique temp file **in the same directory** as `p`.
         *  2. Write `bytes` completely; flush file buffers to disk.
         *  3. Flush the parent directory entry (best effort).
         *  4. Atomically replace `p` with the temp file (POSIX: `rename`; Windows: `ReplaceFileW`).
         * @pre `p` is non-empty.
         * @note If atomic replace is not supported by the platform/filesystem, the implementation SHOULD fail with a descriptive `Error`.
         */
        [[nodiscard]]
        virtual expected<void, Error>
            write_all_bytes_atomic(const std::filesystem::path& p,
                std::span<const std::byte> bytes,
                bool create_dirs = true) const = 0;

        /**
         * @brief Check if a **regular file** exists at `p`.
         * @param p Path to test.
         * @return true if a regular file exists; false otherwise.
         * @note Implementations should not throw; use error-code overloads.
         */
        virtual bool exists(const std::filesystem::path& p) const noexcept = 0;

        /**
         * @brief Remove a file if it exists.
         * @param p Path to remove.
         * @return Success or Error. Removing a non-existent path SHOULD return success.
         */
        [[nodiscard]]
        virtual expected<void, Error>
            remove_file(const std::filesystem::path& p) const = 0;

        // --- Convenience overload preserved for source compatibility ---
        [[nodiscard]]
        expected<void, Error>
            write_all_bytes_atomic(const std::filesystem::path& p,
                const std::vector<std::byte>& bytes,
                bool create_dirs = true) const
        {
            return write_all_bytes_atomic(p, std::span<const std::byte>(bytes.data(), bytes.size()), create_dirs);
        }
    };

    /**
     * @brief Production implementation backed by the host OS.
     * @details
     * - Uses `std::filesystem` with error-code overloads to avoid exceptions for I/O failures.
     * - **Atomic replace**:
     *   - POSIX: `::rename` overwrites atomically; implementation should `fsync` file and parent dir.
     *   - Windows: use `ReplaceFileW` for atomic replacement; `std::filesystem::rename` may fail if target exists.
     * - `exists(p)` returns true only for regular files.
     * @thread_safety Thread-safe for independent paths; no internal shared state.
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

        bool exists(const std::filesystem::path& p) const noexcept override;

        [[nodiscard]]
        expected<void, Error>
            remove_file(const std::filesystem::path& p) const override;
    };

    /**
     * @brief In-memory filesystem (ideal for tests and tooling).
     * @details
     * - Stores byte buffers keyed by a **normalized** path string.
     * - No persistence; process-local only.
     * - Not thread-safe by design.
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
