#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

#include "Interstellar/Core/Logging.hpp"

namespace Interstellar::Core {

    void InitLogger() {
        auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        auto logger = std::make_shared<spdlog::logger>("interstellar", consoleSink);
        logger->set_level(spdlog::level::info);
        spdlog::set_default_logger(logger);
    }
}
