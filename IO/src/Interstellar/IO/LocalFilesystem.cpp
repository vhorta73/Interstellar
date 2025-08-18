// Interstellar/IO/LocalFilesystem.cpp
#include <fstream>
#include <system_error>
#include <filesystem>
#include <vector>
#include <string>
#include <chrono>
#include <atomic>
#include <span>

#include "Interstellar/IO/IFilesystem.hpp"
#include "Interstellar/IO/Result.hpp"   // Error / ErrorCode (alias header for errors)

#if defined(_WIN32)
#define NOMINMAX
#include <windows.h>
#else
#include <fcntl.h>
#include <unistd.h>
#endif

namespace Interstellar::IO {

    namespace {

        // Ensure parent directory exists (no-op if present).
        [[nodiscard]] expected<void, Error> ensure_dir(const std::filesystem::path& dir) {
            if (dir.empty()) return {};
            std::error_code ec;
            if (std::filesystem::exists(dir, ec)) return {};
            if (std::filesystem::create_directories(dir, ec)) return {};
            return make_unexpected(Error{
                ErrorCode::IOError,
                "Failed to create directories: " + dir.string() + (ec ? " (" + ec.message() + ")" : "")
                });
        }

        // Unique temp path in the same directory (PID + time + counter).
        std::filesystem::path unique_temp_path_in(const std::filesystem::path& dir,
            const std::string& base) {
            static std::atomic<uint64_t> counter{ 0 };
            const auto now = static_cast<unsigned long long>(
                std::chrono::high_resolution_clock::now().time_since_epoch().count());

#if defined(_WIN32)
            const unsigned long pid = ::GetCurrentProcessId();
#else
            const unsigned long pid = static_cast<unsigned long>(::getpid());
#endif

            for (int i = 0; i < 64; ++i) {
                const uint64_t c = counter.fetch_add(1, std::memory_order_relaxed);
                auto tmp = dir / (base + ".tmp." + std::to_string(pid) + "." +
                    std::to_string(now) + "." + std::to_string(c));
                std::error_code ec;
                if (!std::filesystem::exists(tmp, ec)) return tmp;
            }
            return dir / (base + ".tmp.fallback");
        }

#if defined(_WIN32)
        [[nodiscard]] bool flush_file_win32(const std::filesystem::path& p) {
            HANDLE h = ::CreateFileW(
                p.c_str(), GENERIC_READ | GENERIC_WRITE,
                FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
            if (h == INVALID_HANDLE_VALUE) return false;
            const BOOL ok = ::FlushFileBuffers(h);
            ::CloseHandle(h);
            return ok == TRUE;
        }
#else
        [[nodiscard]] bool fsync_path(const std::filesystem::path& p) {
            int fd = ::open(p.c_str(), O_RDONLY | O_CLOEXEC);
            if (fd < 0) return false;
            const bool ok = (::fsync(fd) == 0);
            ::close(fd);
            return ok;
        }

        [[nodiscard]] bool fsync_parent_dir(const std::filesystem::path& p) {
            std::error_code ec;
            std::filesystem::path dir = p.parent_path();
            if (dir.empty()) dir = std::filesystem::current_path(ec);
            if (ec) return false;
            int dfd = ::open(dir.c_str(), O_RDONLY | O_CLOEXEC);
            if (dfd < 0) return false;
            const bool ok = (::fsync(dfd) == 0);
            ::close(dfd);
            return ok;
        }
#endif

    } // namespace

    // -------------------- LocalFilesystem --------------------

    expected<std::vector<std::byte>, Error>
        LocalFilesystem::read_all_bytes(const std::filesystem::path& p) const {
        // Open at end to size quickly
        std::ifstream f(p, std::ios::binary | std::ios::ate);
        if (!f) {
            // Differentiate NotFound vs PermissionDenied best-effort
            std::error_code ec;
            if (!std::filesystem::exists(p, ec)) {
                return make_unexpected(Error{ ErrorCode::NotFound, "Cannot open file: " + p.string() });
            }
            return make_unexpected(Error{ ErrorCode::PermissionDenied, "Cannot open file: " + p.string() });
        }

        const std::streampos endpos = f.tellg();
        if (endpos < std::streampos(0)) {
            return make_unexpected(Error{ ErrorCode::IOError, "tellg failed for: " + p.string() });
        }

        const auto n = static_cast<std::size_t>(endpos);
        std::vector<std::byte> data(n);

        f.seekg(0, std::ios::beg);
        if (n > 0) {
            if (!f.read(reinterpret_cast<char*>(data.data()), static_cast<std::streamsize>(n))) {
                return make_unexpected(Error{ ErrorCode::IOError, "Read failed: " + p.string() });
            }
        }
        return data;
    }

    expected<void, Error>
        LocalFilesystem::write_all_bytes_atomic(const std::filesystem::path& p,
            std::span<const std::byte> bytes,
            bool create_dirs) const {
        const auto dir = p.parent_path();
        if (create_dirs) {
            auto ed = ensure_dir(dir);
            if (!ed) return ed;
        }

        const auto base = p.filename().string();
        const auto temp = unique_temp_path_in(dir, base);

        // 1) Write temp
        {
            std::ofstream f(temp, std::ios::binary | std::ios::trunc);
            if (!f) {
                return make_unexpected(Error{ ErrorCode::IOError, "Cannot open temp file: " + temp.string() });
            }
            if (!bytes.empty()) {
                if (!f.write(reinterpret_cast<const char*>(bytes.data()),
                    static_cast<std::streamsize>(bytes.size()))) {
                    std::error_code ignore;
                    std::filesystem::remove(temp, ignore);
                    return make_unexpected(Error{ ErrorCode::IOError, "Write failed: " + temp.string() });
                }
            }
            f.flush();
#if defined(_WIN32)
            (void)flush_file_win32(temp);
#else
            (void)fsync_path(temp);
#endif
        }

        // 2) Atomic replace/create
#if defined(_WIN32)
        std::error_code ec_exists;
        const bool target_exists = std::filesystem::exists(p, ec_exists);

        if (target_exists) {
            // Replace existing target atomically.
            const BOOL ok = ::ReplaceFileW(
                p.c_str(),           // destination (to be replaced)
                temp.c_str(),        // replacement (temp file)
                /*lpBackupFileName*/ nullptr,
                /*dwReplaceFlags*/ 0,
                /*lpExclude*/ nullptr,
                /*lpReserved*/ nullptr
            );
            if (!ok) {
                const DWORD le = ::GetLastError();
                std::error_code ignore;
                std::filesystem::remove(temp, ignore);
                return make_unexpected(Error{
                    ErrorCode::IOError,
                    "ReplaceFileW failed: " + temp.string() + " -> " + p.string() +
                    " (WinErr " + std::to_string(le) + ": " +
                    std::system_category().message(le) + ")"
                    });
            }
        }
        else {
            // First create: MoveFileExW (allows create), request write-through.
            const BOOL ok = ::MoveFileExW(
                temp.c_str(), p.c_str(),
                MOVEFILE_COPY_ALLOWED | MOVEFILE_WRITE_THROUGH | MOVEFILE_REPLACE_EXISTING
            );
            if (!ok) {
                const DWORD le = ::GetLastError();
                std::error_code ignore;
                std::filesystem::remove(temp, ignore);
                return make_unexpected(Error{
                    ErrorCode::IOError,
                    "MoveFileExW failed: " + temp.string() + " -> " + p.string() +
                    " (WinErr " + std::to_string(le) + ": " +
                    std::system_category().message(le) + ")"
                    });
            }
        }
        // No public API to fsync parent dir on Windows; we flushed the file.

#else
        std::error_code ec;
        std::filesystem::rename(temp, p, ec);
        if (ec) {
            std::error_code ignore;
            std::filesystem::remove(temp, ignore);
            return make_unexpected(Error{
                ErrorCode::IOError,
                "Rename failed: " + temp.string() + " -> " + p.string() + " (" + ec.message() + ")"
                });
        }
        (void)fsync_parent_dir(p);
#endif

        return {};
    }

    bool LocalFilesystem::exists(const std::filesystem::path& p) const noexcept {
        std::error_code ec;
        return std::filesystem::is_regular_file(p, ec);
    }

    expected<void, Error>
        LocalFilesystem::remove_file(const std::filesystem::path& p) const {
        std::error_code ec;
        std::filesystem::remove(p, ec);
        if (ec) {
            return make_unexpected(Error{
                ErrorCode::IOError,
                "Remove failed: " + p.string() + " (" + ec.message() + ")"
                });
        }
        return {};
    }

} // namespace Interstellar::IO
