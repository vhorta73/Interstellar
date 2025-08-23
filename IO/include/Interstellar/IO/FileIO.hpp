#pragma once
#include <vector>
#include <span>
#include <filesystem>
#include <cstddef> // std::byte
#include "Interstellar/IO/IFilesystem.hpp"
#include "Interstellar/IO/Container.hpp"

/// \file
/// \ingroup IO
/// \brief Agnostic file read/write orchestration with pluggable serializers and optional container wrapping.
/// \details
/// Overview:
/// - FileIO bridges a concrete filesystem (IFilesystem) with a data-agnostic serializer (ISerializer) to persist/load collections of T.
/// - Optionally wraps payloads in a compact container header (magic + schema).
///
/// Error and exception policy:
/// - Recoverable failures are reported via expected<..., Error>.
/// - Implementations should not throw for I/O or validation failures.
/// - Dynamic allocations may throw std::bad_alloc.
///
/// Thread-safety:
/// - FileIO has no internal shared state and is reentrant.
/// - Thread-safety depends on the provided IFilesystem and ISerializer.
///
/// Container format assumptions:
/// - When SaveOptions::wrap_with_container or LoadOptions::expect_container are enabled,
///   bytes are wrapped by / validated against magic_u32() and schema_version().
/// - Prefer that unpack_container() returns a non-owning span to avoid extra allocations.
///
/// Lifetime:
/// - The referenced IFilesystem must outlive the FileIO instance.

namespace Interstellar::IO {

    /**
     * @ingroup IO
     * @brief Data-agnostic serializer interface for T.
     * @tparam T Value type being serialized.
     * @note Implementations should avoid throwing; return Error on failures.
     * @since 1.0
     */
    template <class T>
    struct ISerializer {
        virtual ~ISerializer() = default;

        /**
         * @ingroup IO
         * @brief Serialize a sequence of items into a contiguous byte buffer.
         * @param items [in] Input items (borrowed).
         * @return expected<std::vector<std::byte>, Error> - Byte buffer on success; Error otherwise.
         * @throws std::bad_alloc May allocate up to encoded size.
         * @complexity O(N) in the number/size of items.
         * @thread_safety Depends on concrete implementation.
         * @since 1.0
         */
        [[nodiscard]]
        virtual expected<std::vector<std::byte>, Error>
            serialize(std::span<const T> items) const = 0;

        /**
         * @ingroup IO
         * @brief Parse items from a byte buffer.
         * @param bytes [in] Raw buffer (borrowed).
         * @return expected<std::vector<T>, Error> - Decoded items on success; Error otherwise.
         * @throws std::bad_alloc If decoder allocates for results.
         * @complexity O(N) in buffer size.
         * @thread_safety Depends on concrete implementation.
         * @since 1.0
         */
        [[nodiscard]]
        virtual expected<std::vector<T>, Error>
            deserialize(std::span<const std::byte> bytes) const = 0;

        /**
         * @ingroup IO
         * @brief Schema version used when container wrapping is enabled.
         * @return uint32_t schema identifier (default 1).
         * @since 1.0
         */
        virtual uint32_t schema_version() const { return 1u; }

        /**
         * @ingroup IO
         * @brief Magic constant to identify the payload format in the container.
         * @details Default is 'IOS1' (0x49 0x4F 0x53 0x31).
         * @return uint32_t magic.
         * @since 1.0
         */
        virtual uint32_t magic_u32() const { return 0x494F5331u; /* 'IOS1' */ }
    };

    /**
     * @ingroup IO
     * @brief Save-time configuration.
     * @since 1.0
     */
    struct SaveOptions {
        /// If true, wrap serialized bytes with a small container header (magic + schema).
        bool wrap_with_container = true;
        /// If true, attempt to create parent directories as needed.
        bool ensure_directories = true;
    };

    /**
     * @ingroup IO
     * @brief Load-time configuration.
     * @since 1.0
     */
    struct LoadOptions {
        /// If true, expect container header and validate magic + schema.
        bool expect_container = true;
    };

    /**
     * @ingroup IO
     * @brief Thin, exception-averse orchestrator for reading/writing serialized data.
     * @details Owns no resources; holds a reference to an external filesystem.
     * @since 1.0
     */
    class FileIO {
    public:
        /**
         * @ingroup IO
         * @brief Construct a FileIO bound to a filesystem.
         * @param fs [in] Filesystem to use (must outlive this instance).
         * @since 1.0
         */
        explicit FileIO(const IFilesystem& fs) : fs_(fs) {}

        /**
         * @ingroup IO
         * @brief Serialize and save a collection to disk, optionally wrapped in a container.
         * @tparam T Item type.
         * @param path [in] Destination file path.
         * @param items [in] Items to persist.
         * @param serializer [in] Serializer implementation for T.
         * @param opt [in] Save behavior toggles (container, directory creation).
         * @return expected<void, Error> - Success or Error.
         * @pre path is non-empty.
         * @post On success, path holds new bytes (atomic replace semantics per filesystem).
         * @throws std::bad_alloc On buffer growth or serializer allocations.
         * @complexity O(N) including serialization and optional wrapping.
         * @thread_safety Reentrant; depends on IFilesystem and ISerializer thread-safety.
         * @see pack_container
         * @since 1.0
         */
        template <class T>
        [[nodiscard]]
        expected<void, Error>
            save(const std::filesystem::path& path,
                std::span<const T> items,
                const ISerializer<T>& serializer,
                const SaveOptions& opt = {}) const
        {
            auto ser = serializer.serialize(items);
            if (!ser) return unexpected<Error>(ser.error());

            // If wrapping, compose a container buffer; otherwise reuse the serialized buffer.
            std::vector<std::byte> bytes;
            if (opt.wrap_with_container) {
                // Prefer a pack_container overload that accepts an rvalue to avoid extra copies.
                bytes = pack_container(serializer.magic_u32(),
                    serializer.schema_version(),
                    ser.value() /* payload copy if pack needs owning */);
            }
            else {
                bytes = std::move(ser.value());
            }

            // Prefer an FS overload that accepts span to avoid copies.
            return fs_.write_all_bytes_atomic(path,
                std::span<const std::byte>(bytes.data(), bytes.size()),
                opt.ensure_directories);
        }

        /**
         * @ingroup IO
         * @brief Load a collection from disk, optionally validating a container header.
         * @tparam T Item type.
         * @param path [in] Source file path.
         * @param serializer [in] Serializer implementation for T.
         * @param opt [in] Load behavior toggles (expect container).
         * @return expected<std::vector<T>, Error> - Decoded items on success; Error otherwise.
         * @pre path exists and refers to a regular file.
         * @throws std::bad_alloc If serializer allocates for results.
         * @warning Entire file is read into memory; consider streaming for very large assets.
         * @complexity O(N) in file size plus decode time.
         * @thread_safety Reentrant; depends on IFilesystem and ISerializer thread-safety.
         * @see unpack_container
         * @since 1.0
         */
        template <class T>
        [[nodiscard]]
        expected<std::vector<T>, Error>
            load(const std::filesystem::path& path,
                const ISerializer<T>& serializer,
                const LoadOptions& opt = {}) const
        {
            auto rd = fs_.read_all_bytes(path);
            if (!rd) return unexpected<Error>(rd.error());

            std::span<const std::byte> bytes{ rd.value().data(), rd.value().size() };

            if (opt.expect_container) {
                auto up = unpack_container(bytes, serializer.magic_u32(), serializer.schema_version());
                if (!up) return unexpected<Error>(up.error());
                // Prefer that unpack_container returns a span to avoid allocation here.
                return serializer.deserialize(up.value());
            }

            return serializer.deserialize(bytes);
        }

    private:
        const IFilesystem& fs_;
    };

} // namespace Interstellar::IO
