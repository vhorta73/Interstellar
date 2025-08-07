#include <gtest/gtest.h>
#include <spdlog/spdlog.h>
#include <spdlog/spdlog-inl.h>
#include <spdlog/sinks/ostream_sink.h>

#include "Interstellar/Core/Logging.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Interstellar::Core;

namespace Microsoft::VisualStudio::CppUnitTestFramework {
    // Specialize ToString for spdlog::level::level_enum
    template<>
    inline std::wstring ToString(const spdlog::level::level_enum& level) {
        switch (level) {
        case spdlog::level::trace:    return L"trace";
        case spdlog::level::debug:    return L"debug";
        case spdlog::level::info:     return L"info";
        case spdlog::level::warn:     return L"warn";
        case spdlog::level::err:      return L"error";
        case spdlog::level::critical: return L"critical";
        case spdlog::level::off:      return L"off";
        default:                      return L"unknown";
        }
    }
}

namespace LoggingTest
{
    TEST_CLASS(LoggerTest)
    {
    public:

        TEST_METHOD(RegistersNamedLogger)
        {
            spdlog::drop(LOG_GENERIC);
            Interstellar::Core::Logger logger(LOG_GENERIC);
            Assert::IsNotNull(spdlog::get(LOG_GENERIC).get(), L"Logger should be registered");
            spdlog::drop_all();
        }

        TEST_METHOD(CreatesDefaultLoggerOnUnregistered)
        {
            const std::string unknownLogger = "nonexistent";
            spdlog::drop(unknownLogger); // Ensure it is not registered.

            Interstellar::Core::Logger logger(unknownLogger);
            auto actual = spdlog::get(unknownLogger);

            Assert::IsNotNull(actual.get(), L"Logger should have been created");
            Assert::AreEqual(unknownLogger, actual->name(), L"Logger name should match the request one");
            spdlog::drop_all();
        }

        TEST_METHOD(CreatesLoggerIfNotAlreadyRegistered)
        {
            std::ostringstream oss;
            auto sink = std::make_shared<spdlog::sinks::ostream_sink_mt>(oss);

            auto logger = std::make_shared<spdlog::logger>("test_logger", sink);
            spdlog::register_logger(logger);

            Interstellar::Core::Logger log("test_logger");
            log.LogInfo("Hello, MS Test!");

            logger->flush();

            std::string output = oss.str();
            Assert::IsTrue(output.find("Hello, MS Test!") != std::string::npos, L"Log message not found in output stream");
            spdlog::drop_all();
        }

        TEST_METHOD(RespectsLogLevelFiltering)
        {
            std::ostringstream oss;
            auto sink = std::make_shared<spdlog::sinks::ostream_sink_mt>(oss);
            sink->set_level(spdlog::level::warn); // Only WARN and above

            const std::string loggerName = "level_test";
            auto spdLogger = std::make_shared<spdlog::logger>(loggerName, sink);
            spdLogger->set_level(spdlog::level::warn);
            spdlog::register_logger(spdLogger);

            Interstellar::Core::Logger logger(loggerName); // Uses already-registered logger

            logger.LogDebug("This should NOT appear");
            logger.LogInfo("Nor should this");
            logger.LogWarn("This should appear");
            logger.LogError("This also should appear");

            spdLogger->flush();

            const std::string output = oss.str();
            Assert::IsTrue(output.find("This should appear") != std::string::npos, L"WARN message missing");
            Assert::IsTrue(output.find("This also should appear") != std::string::npos, L"ERROR message missing");
            Assert::IsTrue(output.find("This should NOT appear") == std::string::npos, L"DEBUG message should be filtered");
            Assert::IsTrue(output.find("Nor should this") == std::string::npos, L"INFO message should be filtered");

            spdlog::drop_all();
        }

        TEST_METHOD(MultipleLoggersConfiguredIndependently)
        {
            // Cleanup first in case of residual registrations
            spdlog::drop_all();

            const std::string loggerAName = "LoggerA";
            const std::string loggerBName = "LoggerB";
            const std::string loggerCName = "LoggerC";

            Interstellar::Core::Logger loggerA(loggerAName, LogLevel::Error);
            Interstellar::Core::Logger loggerB(loggerBName, LogLevel::Info);
            Interstellar::Core::Logger loggerC(loggerCName, LogLevel::Trace);

            // Confirm all are registered
            auto a = spdlog::get(loggerAName);
            auto b = spdlog::get(loggerBName);
            auto c = spdlog::get(loggerCName);

            Assert::IsNotNull(a.get(), L"LoggerA should be registered");
            Assert::IsNotNull(b.get(), L"LoggerB should be registered");
            Assert::IsNotNull(c.get(), L"LoggerC should be registered");

            // Confirm levels are set correctly
            Assert::AreEqual(spdlog::level::err, a->level(), L"LoggerA level should be Error");
            Assert::AreEqual(spdlog::level::info, b->level(), L"LoggerB level should be Info");
            Assert::AreEqual(spdlog::level::trace, c->level(), L"LoggerC level should be Trace");

            // Cleanup
            spdlog::drop_all();
        }

    };
}
