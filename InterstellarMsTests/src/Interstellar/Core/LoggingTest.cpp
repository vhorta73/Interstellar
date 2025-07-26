#include "pch.h"
#include "CppUnitTest.h"
#include <spdlog/spdlog.h>
#include <spdlog/spdlog-inl.h>
#include <spdlog/sinks/ostream_sink.h>

#include "Interstellar/Core/Logging.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Interstellar::Core;

namespace LoggingTest
{
    TEST_CLASS(LoggerTest)
    {
    public:

        TEST_METHOD(CreatesNamedLoggerSuccessfully)
        {
            Interstellar::Core::Logger logger(GENERIC_LOG);
            Assert::IsNotNull(spdlog::get(GENERIC_LOG).get(), L"Logger should be registered");
        }

        TEST_METHOD(LoggerThrowsOnMissingName)
        {
            const std::string unknwonLogger = "nonexistent";
            spdlog::drop(unknwonLogger); // Ensure it is not registered.

            Interstellar::Core::Logger logger(unknwonLogger);
            auto actual = spdlog::get(unknwonLogger);

            Assert::IsNotNull(actual.get(), L"Logger should h ave been created");
            Assert::AreEqual(unknwonLogger, actual->name(), L"Loggerl name should match the request one");
        }

        TEST_METHOD(LoggerWritesToCustomStream)
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
        }
    };
}
