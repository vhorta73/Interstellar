#pragma once
#include <string>

namespace Interstellar::IO {

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

    struct Error {
        ErrorCode code{ ErrorCode::None };
        std::string message{};
    };

} // namespace Interstellar::IO
