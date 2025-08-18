#pragma once
#include <string>
#include <string_view>
#include <system_error> // std::error_code, std::errc

/// \file Error.hpp
/// \brief Lightweight error code and message used across Interstellar IO.
/// \details
///  - Use strongly-typed \ref Interstellar::IO::ErrorCode to classify failures.
///  - Attach a short, human-readable message for diagnostics/logging.
///  - Prefer converting native errors (errno/Win32) to \ref Error via
///    \ref Error::from_system_error to preserve context.
///  - This type is intended to be used as the `E` in `expected<T, Error>`.

namespace Interstellar::IO {

    /**
     * @brief Categorical error codes for IO and serialization failures.
     * @details
     * - `None`: success; no error condition.
     * - `NotFound`: path/resource does not exist.
     * - `PermissionDenied`: lacking required privileges or locked by another process.
     * - `InvalidArgument`: API misuse, null/empty path, out-of-range parameter.
     * - `InvalidData`: well-formed file access but contents are semantically invalid (e.g., bad field).
     * - `VersionMismatch`: version/feature level incompatible with current reader/writer.
     * - `IOError`: I/O subsystem failure (disk error, short read/write, device error).
     * - `Corrupted`: structurally damaged file/stream (truncated, bad checksum, magic mismatch).
     */
    enum class ErrorCode {
        None = 0,
        NotFound,
        PermissionDenied,
        InvalidArgument,
        InvalidData,
        VersionMismatch,
        IOError,
        Corrupted,
    };

    /**
     * @brief Error value with code and optional human-readable message.
     * @note Kept small and trivially movable; suitable for `expected<T, Error>`.
     */
    struct Error {
        /// Primary classification.
        ErrorCode code{ ErrorCode::None };
        /// Human-readable detail (may be empty). Keep short; logs can add context.
        std::string message{};

        // ---------------- Convenience: state checks ----------------

        /// @brief True if no error.
        [[nodiscard]] constexpr bool ok() const noexcept { return code == ErrorCode::None; }
        /// @brief True if there is an error.
        [[nodiscard]] constexpr bool failed() const noexcept { return !ok(); }

        // ---------------- Factory helpers (ergonomic creation) ----------------

        /// @brief Create a success value.
        [[nodiscard]] static constexpr Error none() noexcept { return {}; }

        [[nodiscard]] static Error not_found(std::string_view msg = {}) {
            return { ErrorCode::NotFound, std::string(msg) };
        }
        [[nodiscard]] static Error permission_denied(std::string_view msg = {}) {
            return { ErrorCode::PermissionDenied, std::string(msg) };
        }
        [[nodiscard]] static Error invalid_argument(std::string_view msg = {}) {
            return { ErrorCode::InvalidArgument, std::string(msg) };
        }
        [[nodiscard]] static Error invalid_data(std::string_view msg = {}) {
            return { ErrorCode::InvalidData, std::string(msg) };
        }
        [[nodiscard]] static Error version_mismatch(std::string_view msg = {}) {
            return { ErrorCode::VersionMismatch, std::string(msg) };
        }
        [[nodiscard]] static Error io_error(std::string_view msg = {}) {
            return { ErrorCode::IOError, std::string(msg) };
        }
        [[nodiscard]] static Error corrupted(std::string_view msg = {}) {
            return { ErrorCode::Corrupted, std::string(msg) };
        }

        // ---------------- Interop with std::error_code ----------------

        /**
         * @brief Create from a system error, mapping common `std::errc` to our categories.
         * @param ec System error code (e.g., from filesystem ops).
         * @param context Optional extra context to prepend.
         * @return Mapped Error with combined message.
         * @note Preserves the original code value as text inside \ref message.
         */
        [[nodiscard]] static Error from_system_error(std::error_code ec, std::string_view context = {}) {
            const ErrorCode mapped = map_errc(ec);
            std::string msg;
            msg.reserve(context.size() + 2 + ec.message().size() + 16);
            if (!context.empty()) {
                msg.append(context);
                msg.append(": ");
            }
            msg.append(ec.message());
            msg.append(" [");
            msg.append(ec.category().name());
            msg.push_back(':');
            msg.append(std::to_string(ec.value()));
            msg.push_back(']');

            return { mapped == ErrorCode::None ? ErrorCode::IOError : mapped, std::move(msg) };
        }

        /**
         * @brief Convert this Error to a generic `std::error_code` for interop/logging.
         * @details This uses `std::errc` approximations; detailed OS codes are only
         *          available if you captured them earlier and included them in `message`.
         */
        [[nodiscard]] static std::error_code to_std_error(ErrorCode code) noexcept {
            using std::errc;
            switch (code) {
            case ErrorCode::None:             return {};
            case ErrorCode::NotFound:         return make_error_code(errc::no_such_file_or_directory);
            case ErrorCode::PermissionDenied: return make_error_code(errc::permission_denied);
            case ErrorCode::InvalidArgument:  return make_error_code(errc::invalid_argument);
            case ErrorCode::InvalidData:      return make_error_code(errc::illegal_byte_sequence);
            case ErrorCode::VersionMismatch:  return make_error_code(errc::protocol_error);
            case ErrorCode::IOError:          return make_error_code(errc::io_error);
            case ErrorCode::Corrupted:        return make_error_code(errc::bad_message);
            }
            return make_error_code(errc::io_error);
        }

        // ---------------- Utilities ----------------

        /// @brief Convert code to a stable string token (for logs/telemetry).
        [[nodiscard]] static constexpr const char* to_string(ErrorCode c) noexcept {
            switch (c) {
            case ErrorCode::None:             return "None";
            case ErrorCode::NotFound:         return "NotFound";
            case ErrorCode::PermissionDenied: return "PermissionDenied";
            case ErrorCode::InvalidArgument:  return "InvalidArgument";
            case ErrorCode::InvalidData:      return "InvalidData";
            case ErrorCode::VersionMismatch:  return "VersionMismatch";
            case ErrorCode::IOError:          return "IOError";
            case ErrorCode::Corrupted:        return "Corrupted";
            }
            return "Unknown";
        }

        /// @brief Human-readable summary (code + message).
        [[nodiscard]] std::string summary() const {
            if (message.empty()) return std::string(to_string(code));
            std::string out{ to_string(code) };
            out.append(": ");
            out.append(message);
            return out;
        }

        // ---------------- Comparisons (useful in tests) ----------------

        friend constexpr bool operator==(const Error& a, const Error& b) noexcept {
            // Intentionally compare both fields; messages matter in tests.
            return a.code == b.code && a.message == b.message;
        }
        friend constexpr bool operator!=(const Error& a, const Error& b) noexcept {
            return !(a == b);
        }

    private:
        [[nodiscard]] static ErrorCode map_errc(std::error_code ec) noexcept {
            using std::errc;
            switch (static_cast<errc>(ec.value())) {
            case errc::no_such_file_or_directory: return ErrorCode::NotFound;
            case errc::permission_denied:          return ErrorCode::PermissionDenied;
            case errc::invalid_argument:           return ErrorCode::InvalidArgument;
            case errc::illegal_byte_sequence:      return ErrorCode::InvalidData;
            case errc::protocol_error:             return ErrorCode::VersionMismatch;
            case errc::io_error:                   return ErrorCode::IOError;
            case errc::bad_message:                return ErrorCode::Corrupted;
            default:                                return ErrorCode::IOError;
            }
        }
    };

} // namespace Interstellar::IO

// ---------------- Optional {fmt} integration ----------------
#if __has_include(<fmt/format.h>)
#include <fmt/format.h>
template <>
struct fmt::formatter<Interstellar::IO::Error> : fmt::formatter<std::string_view> {
    template <typename FormatContext>
    auto format(const Interstellar::IO::Error& e, FormatContext& ctx) {
        return fmt::formatter<std::string_view>::format(e.summary(), ctx);
    }
};
#endif
