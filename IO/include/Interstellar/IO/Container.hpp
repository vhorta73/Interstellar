#pragma once
#include <cstdint>
#include <cstring>
#include <vector>
#include <span>
#include "Result.hpp"
#include "Expected.hpp"

namespace Interstellar::IO {

    // Optional container header to catch mismatches/corruption early.
    // Layout (LE): [magic:4][version:4][payload_size:8][crc32:4][payload...]
    struct ContainerHeader {
        uint32_t magic{ 0 };
        uint32_t version{ 0 };
        uint64_t payload_size{ 0 };
        uint32_t crc32{ 0 };
    };

    inline uint32_t crc32(std::span<const std::byte> data) {
        static uint32_t table[256];
        static bool init = false;
        if (!init) {
            for (uint32_t i = 0; i < 256; ++i) {
                uint32_t c = i;
                for (int k = 0; k < 8; ++k) c = (c & 1u) ? (0xEDB88320u ^ (c >> 1)) : (c >> 1);
                table[i] = c;
            }
            init = true;
        }
        uint32_t c = 0xFFFFFFFFu;
        for (auto b : data) c = table[(c ^ (uint8_t)b) & 0xFFu] ^ (c >> 8);
        return c ^ 0xFFFFFFFFu;
    }

    inline std::vector<std::byte>
        pack_container(uint32_t magic, uint32_t version, std::span<const std::byte> payload) {
        ContainerHeader h{ magic, version, static_cast<uint64_t>(payload.size()), crc32(payload) };
        std::vector<std::byte> out(sizeof(ContainerHeader) + payload.size());
        std::memcpy(out.data(), &h, sizeof(ContainerHeader));
        std::memcpy(out.data() + sizeof(ContainerHeader), payload.data(), payload.size());
        return out;
    }

    inline expected<std::span<const std::byte>, Error>
        unpack_container(std::span<const std::byte> bytes,
            uint32_t expect_magic,
            uint32_t expect_version,
            const ContainerHeader** out_hdr = nullptr) {
        if (bytes.size() < sizeof(ContainerHeader))
            return unexpected<Error>({ ErrorCode::InvalidData, "Too small for header" });
        auto* hdr = reinterpret_cast<const ContainerHeader*>(bytes.data());
        if (hdr->magic != expect_magic)   return unexpected<Error>({ ErrorCode::VersionMismatch, "Magic mismatch" });
        if (hdr->version != expect_version) return unexpected<Error>({ ErrorCode::VersionMismatch, "Version mismatch" });
        const size_t total = sizeof(ContainerHeader) + static_cast<size_t>(hdr->payload_size);
        if (bytes.size() < total) return unexpected<Error>({ ErrorCode::Corrupted, "Truncated payload" });
        std::span<const std::byte> payload{ bytes.data() + sizeof(ContainerHeader),
                                           static_cast<size_t>(hdr->payload_size) };
        if (crc32(payload) != hdr->crc32) return unexpected<Error>({ ErrorCode::Corrupted, "CRC mismatch" });
        if (out_hdr) *out_hdr = hdr;
        return payload;
    }

} // namespace Interstellar::IO
