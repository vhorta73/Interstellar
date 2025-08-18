#pragma once
#include <vector>
#include <span>
#include <filesystem>
#include "IFilesystem.hpp"
#include "Container.hpp"

/// \file FileIO.hpp
/// \brief Agnostic file read/write orchestration with pluggable serializers and optional container wrapping.
/// \details
/// ### Overview
/// `FileIO` bridges a concrete filesystem (\ref Interstellar::IO::IFilesystem)
/// with a data-agnostic \ref ISerializer to persist/load collections of `T`.
/// Optionally wraps payloads in a compact container header (magic + schema).
///
/// ### Error & exception policy
/// - All recoverable failures are reported via `expected<..., Error>`.
/// - Implementations should **not throw** for I/O or validation failures.
/// - Dynamic allocations may throw `std::bad_alloc`.
///
/// ### Thread-safety
/// - `FileIO` has no internal shared state and is reentrant.
/// - Thread-safety depends on the provided \ref IFilesystem and \ref ISerializer.
///
/// ### Container format assumptions
/// - When `SaveOptions::wrap_with_container` or `LoadOptions::expect_container` are enabled,
///   the bytes are wrapped by/validated against `magic_u32()` and `schema_version()`.
/// - Prefer that `unpack_container()` returns a non-owning view/span into the original buffer
///   to avoid extra allocations on load. If it returns an owning buffer, loading will copy.
///
/// ### Lifetime
/// - The referenced \ref IFilesystem must outlive the `FileIO` instance.

namespace Interstellar::IO {

    /**
     * @brief Data-agnostic serializer interface for `T`.
     * @tparam T Value type (POD or otherwise) being serialized.
     * @note Implementations should avoid throwing; return `Error` on failures.
     */
    template <class T>
    struct ISerializer {
        virtual ~ISerializer() = default;

        /**
         * @brief Serialize a sequence of items into a contiguous byte buffer.
         * @param items Input items (borrowed).
         * @return Byte buffer on success; `Error` otherwise.
         * @warning May allocate up to the encoded size of `items`.
         */
        [[nodiscard]]
        virtual expected<std::vector<std::byte>, Error>
            serialize(std::span<const T> items) const = 0;

        /**
         * @brief Parse items from a byte buffer.
         * @param bytes Raw buffer (borrowed).
         * @return Decoded items on success; `Error` otherwise.
         */
        [[nodiscard]]
        virtual expected<std::vector<T>, Error>
            deserialize(std::span<const std::byte> bytes) const = 0;

        /**
         * @brief Schema version used when container wrapping is enabled.
         * @details Override in concrete serializers to evolve formats.
         */
        virtual uint32_t schema_version() const { return 1u; }

        /**
         * @brief Magic constant used to identify the payload format in the container.
         * @details Default is 'IOS1' (0x49 0x4F 0x53 0x31).
         */
        virtual uint32_t magic_u32() const { return 0x494F5331u; /* 'IOS1' */ }
    };

    /**
     * @brief Save-time configuration.
     */
    struct SaveOptions {
        /// If true, wrap serialized bytes with a small container header (magic + schema).
        bool wrap_with_container = true;
        /// If true, attempt to create parent directories as needed.
        bool ensure_directories = true;
    };

    /**
     * @brief Load-time configuration.
     */
    struct LoadOptions {
        /// If true, expect container header and validate magic + schema.
        bool expect_container = true;
    };

    /**
     * @brief Thin, exception-averse orchestrator for reading/writing serialized data.
     * @details Owns no resources; holds a reference to an external filesystem.
     */
    class FileIO {
    public:
        /// @brief Construct a FileIO bound to a filesystem.
        /// @param fs Filesystem to use (must outlive this instance).
        explicit FileIO(const IFilesystem& fs) : fs_(fs) {}

        /**
         * @brief Serialize and save a collection to disk, optionally wrapped in a container.
         * @tparam T Item type.
         * @param path Destination file path.
         * @param items Items to persist.
         * @param serializer Serializer implementation for `T`.
         * @param opt Save behavior toggles (container, directory creation).
         * @return Success or Error.
         * @pre `path` is non-empty.
         * @post On success, `path` holds the new bytes (atomic replace semantics per filesystem).
         * @warning May allocate; see serializer's allocation behavior.
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
            return fs_.write_all_bytes_atomic(path, std::span<const std::byte>(bytes.data(), bytes.size()),
                opt.ensure_directories);
        }

        /**
         * @brief Load a collection from disk, optionally validating a container header.
         * @tparam T Item type.
         * @param path Source file path.
         * @param serializer Serializer implementation for `T`.
         * @param opt Load behavior toggles (expect container).
         * @return Decoded items on success; `Error` otherwise.
         * @pre `path` exists and refers to a regular file.
         * @warning The entire file is read into memory; consider streaming for very large assets.
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
                // Prefer that unpack_container returns a span/view to avoid an allocation here.
                return serializer.deserialize(up.value());
            }

            return serializer.deserialize(bytes);
        }

    private:
        const IFilesystem& fs_;
    };

} // namespace Interstellar::IO
