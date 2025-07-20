#pragma once

#include "AppConfig.hpp"

namespace Interstellar::Core::Logging
{
    // Call once at startup. If logFilePath is empty, logs go to console only.
    void Init(AppConfig const& cfg);
}
