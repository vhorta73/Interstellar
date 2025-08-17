#pragma once
#include <vector>
#include <span>
#include <filesystem>
#include "IFilesystem.hpp"
#include "Container.hpp"

namespace Interstellar::IO {

    // Data-agnostic serializer interface.
    // Your Data layer implements this for each T (JSON, binary, etc.).
    template <class T>
    struct ISerializer {
        virtual ~ISerializer() = default;

        virtual expected<std::vector<std::byte>, Error>
            serialize(std::span<const T> items) const = 0;

        virtual expected<std::vector<T>, Error>
            deserialize(std::span<const std::byte> bytes) const = 0;

        virtual uint32_t schema_version() const { return 1u; }
        virtual uint32_t magic_u32()      const { return 0x494F5331u; /* 'IOS1' */ }
    };

    struct SaveOptions { bool wrap_with_container = true; bool ensure_directories = true; };
    struct LoadOptions { bool expect_container = true; };

    // FileIO is a thin, safe wrapper around IFilesystem.
    class FileIO {
    public:
        explicit FileIO(const IFilesystem& fs) : fs_(fs) {}

        template <class T>
        expected<void, Error>
            save(const std::filesystem::path& path,
                std::span<const T> items,
                const ISerializer<T>& serializer,
                const SaveOptions& opt = {}) const
        {
            auto ser = serializer.serialize(items);
            if (!ser) return unexpected<Error>(ser.error());

            std::vector<std::byte> bytes;
            if (opt.wrap_with_container) {
                bytes = pack_container(serializer.magic_u32(), serializer.schema_version(), ser.value());
            }
            else {
                bytes = std::move(ser.value());
            }
            return fs_.write_all_bytes_atomic(path, bytes, opt.ensure_directories);
        }

        template <class T>
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
                return serializer.deserialize(up.value());
            }
            return serializer.deserialize(bytes);
        }

    private:
        const IFilesystem& fs_;
    };

} // namespace Interstellar::IO
