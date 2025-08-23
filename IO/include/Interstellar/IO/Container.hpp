#pragma once
#include <cstdint>
#include <cstddef>   // std::byte, size_t
#include <cstring>
#include <vector>
#include <span>
#include <array>
#include <limits>
#include <bit>       // optional: std::endian hint
#include "Interstellar/IO/Expected.hpp"
#include "Interstellar/IO/Result.hpp"  // defines Error/ErrorCode

/// \file
/// \ingroup IO
/// \brief Compact little-endian container to guard payloads with magic, version, size, and CRC32.
/// \details
/// Wire layout (little-endian, exactly 20 bytes):
///   [magic:4][version:4][payload_size:8][crc32:4][payload...]
/// Notes:
/// - The header is not a raw dump of a C++ struct; fields are encoded explicitly in LE.
/// - unpack_container() validates size and CRC before returning a span over the payload.
/// - CRC polynomial: 0xEDB88320 (CRC-32/ISO-HDLC), init 0xFFFFFFFF, final XOR 0xFFFFFFFF.
/// Thread-safety:
/// - crc32() uses a function-local static lookup table (thread-safe since C++11).
/// Exceptions:
/// - Validation failures return Error via expected.
/// - pack_container() may throw std::bad_alloc on vector growth.

namespace Interstellar::IO {

    /**
     * @ingroup IO
     * @brief Parsed container header (convenience view, not the wire layout).
     * @since 1.0
     */
    struct ContainerHeader {
        uint32_t magic{ 0 };
        uint32_t version{ 0 };
        uint64_t payload_size{ 0 };
        uint32_t crc32{ 0 };
    };

    // ------------------------------------------------------------------
    // Internal LE helpers (portable, no UB) -- hidden from public docs.
    // ------------------------------------------------------------------
    /// \cond INTERNAL

    constexpr std::size_t kHeaderSize = 4u + 4u + 8u + 4u; // 20 bytes
    static_assert(kHeaderSize == 20, "Header size must be 20 bytes");

    inline void store_le32(std::byte* dst, uint32_t v) noexcept {
        dst[0] = std::byte(v & 0xFFu);
        dst[1] = std::byte((v >> 8) & 0xFFu);
        dst[2] = std::byte((v >> 16) & 0xFFu);
        dst[3] = std::byte((v >> 24) & 0xFFu);
    }
    inline void store_le64(std::byte* dst, uint64_t v) noexcept {
        for (int i = 0; i < 8; ++i) dst[i] = std::byte((v >> (8 * i)) & 0xFFu);
    }
    inline uint32_t load_le32(const std::byte* src) noexcept {
        return  (uint32_t(std::to_integer<uint8_t>(src[0]))) |
            (uint32_t(std::to_integer<uint8_t>(src[1])) << 8) |
            (uint32_t(std::to_integer<uint8_t>(src[2])) << 16) |
            (uint32_t(std::to_integer<uint8_t>(src[3])) << 24);
    }
    inline uint64_t load_le64(const std::byte* src) noexcept {
        uint64_t v = 0;
        for (int i = 0; i < 8; ++i)
            v |= uint64_t(std::to_integer<uint8_t>(src[i])) << (8 * i);
        return v;
    }

    /// \endcond

    /**
     * @ingroup IO
     * @brief Compute CRC32 (CRC-32/ISO-HDLC) over a byte span.
     * @param data Input buffer.
     * @return 32-bit CRC.
     * @complexity O(N) in data size.
     * @thread_safety Thread-safe; uses a function-local static table.
     * @since 1.0
     */
    [[nodiscard]] inline uint32_t crc32(std::span<const std::byte> data) noexcept {
        static const std::array<uint32_t, 256> table = [] {
            std::array<uint32_t, 256> t{};
            for (uint32_t i = 0; i < 256; ++i) {
                uint32_t c = i;
                for (int k = 0; k < 8; ++k)
                    c = (c & 1u) ? (0xEDB88320u ^ (c >> 1)) : (c >> 1);
                t[i] = c;
            }
            return t;
            }();

        uint32_t c = 0xFFFFFFFFu;
        for (std::byte b : data) {
            const uint8_t x = std::to_integer<uint8_t>(b);
            c = table[(c ^ x) & 0xFFu] ^ (c >> 8);
        }
        return c ^ 0xFFFFFFFFu;
    }

    /**
     * @ingroup IO
     * @brief Pack a payload with a 20-byte LE header (magic, version, size, CRC).
     * @param magic   4-byte magic constant.
     * @param version Schema/format version.
     * @param payload Payload bytes (borrowed).
     * @return Concatenated [header|payload] buffer.
     * @post result.size() == 20 + payload.size().
     * @throws std::bad_alloc On vector allocation/growth.
     * @complexity O(N) in payload size.
     * @since 1.0
     */
    [[nodiscard]] inline std::vector<std::byte>
        pack_container(uint32_t magic, uint32_t version, std::span<const std::byte> payload) {
        const uint64_t psz = static_cast<uint64_t>(payload.size());
        const uint32_t crc = crc32(payload);

        std::vector<std::byte> out;
        out.resize(kHeaderSize + payload.size());

        std::byte* p = out.data();
        store_le32(p, magic);    p += 4;
        store_le32(p, version);  p += 4;
        store_le64(p, psz);      p += 8;
        store_le32(p, crc);      p += 4;

        std::memcpy(p, payload.data(), payload.size());
        return out;
    }

    /**
     * @ingroup IO
     * @brief Unpack and validate a container, returning a view of the payload on success.
     * @param bytes          Entire [header|payload] buffer (borrowed).
     * @param expect_magic   Expected magic constant (mismatch -> VersionMismatch).
     * @param expect_version Expected schema version (mismatch -> VersionMismatch).
     * @param out_hdr        Optional: if non-null, filled with the parsed header.
     * @return expected<span<const byte>, Error> - payload view on success; Error otherwise.
     * @errors
     *  - InvalidData: header too small.
     *  - VersionMismatch: magic or version mismatch.
     *  - Corrupted: truncated payload or CRC mismatch.
     * @complexity O(N) in payload size (for CRC).
     * @thread_safety Reentrant; operates on caller-provided buffers only.
     * @since 1.0
     */
    [[nodiscard]] inline expected<std::span<const std::byte>, Error>
        unpack_container(std::span<const std::byte> bytes,
            uint32_t expect_magic,
            uint32_t expect_version,
            ContainerHeader* out_hdr = nullptr)
    {
        if (bytes.size() < kHeaderSize) {
            return make_unexpected(Error{ ErrorCode::InvalidData, "Too small for header" });
        }

        const std::byte* p = bytes.data();
        const uint32_t magic = load_le32(p); p += 4;
        const uint32_t version = load_le32(p); p += 4;
        const uint64_t psz = load_le64(p); p += 8;
        const uint32_t crc = load_le32(p); p += 4;

        if (magic != expect_magic) {
            return make_unexpected(Error{ ErrorCode::VersionMismatch, "Magic mismatch" });
        }
        if (version != expect_version) {
            return make_unexpected(Error{ ErrorCode::VersionMismatch, "Version mismatch" });
        }

        // Bounds/overflow checks (avoid size_t wrap).
        const std::size_t header = kHeaderSize;
        const std::size_t total = bytes.size();
        if (psz > std::numeric_limits<std::size_t>::max()) {
            return make_unexpected(Error{ ErrorCode::Corrupted, "Payload size exceeds platform limit" });
        }
        const std::size_t psz_sz = static_cast<std::size_t>(psz);
        if (total < header || (total - header) < psz_sz) {
            return make_unexpected(Error{ ErrorCode::Corrupted, "Truncated payload" });
        }

        std::span<const std::byte> payload{ bytes.data() + header, psz_sz };
        if (crc32(payload) != crc) {
            return make_unexpected(Error{ ErrorCode::Corrupted, "CRC mismatch" });
        }

        if (out_hdr) {
            *out_hdr = ContainerHeader{ magic, version, psz, crc };
        }
        return payload;
    }

} // namespace Interstellar::IO
