// Logging/tests/LoggingTests.cpp
#include <gtest/gtest.h>

#include <sstream>
#include <filesystem>
#include <fstream>
#include <optional>
#include <cstdlib> // setenv/_putenv_s
#include <spdlog/spdlog.h>
#include <spdlog/sinks/ostream_sink.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "Interstellar/Logging/Logging.hpp"
#include "Interstellar/Logging/LogLevel.hpp"

namespace Interstellar::Logging {

    // --- helpers -------------------------------------------------------------

    inline std::string UniqueName(const ::testing::TestInfo& ti, std::string suffix = {}) {
        std::string n = std::string(ti.test_suite_name()) + "." + ti.name();
        if (!suffix.empty()) n += "." + suffix;
        for (char& c : n) if (c == '/' || c == ' ') c = '_';
        return n;
    }

    // RAII env var setter so tests don't leak process state
    struct EnvVarGuard {
        std::string key;
        std::optional<std::string> old;
        EnvVarGuard(const std::string& k, const std::string& v) : key(k) {
#ifdef _WIN32
            size_t len = 0; char* buf = nullptr;
            if (_dupenv_s(&buf, &len, key.c_str()) == 0 && buf) { old = std::string(buf); free(buf); }
            _putenv_s(key.c_str(), v.c_str());
#else
            if (const char* o = std::getenv(key.c_str())) old = std::string(o);
            ::setenv(key.c_str(), v.c_str(), 1);
#endif
        }
        ~EnvVarGuard() {
#ifdef _WIN32
            if (old) _putenv_s(key.c_str(), old->c_str());
            else _putenv_s(key.c_str(), "");
#else
            if (old) ::setenv(key.c_str(), old->c_str(), 1);
            else ::unsetenv(key.c_str());
#endif
        }
    };

    static void RemoveAllQuiet(const std::filesystem::path& p) {
        std::error_code ec;
        std::filesystem::remove_all(p, ec);
    }

    struct LoggerFixture : ::testing::Test {
        void TearDown() override {
            spdlog::drop_all(); // close files & clear registry between tests
        }
    };

    // --- your existing tests -------------------------------------------------

    TEST_F(LoggerFixture, RegistersNamedLogger) {
        const auto& info = *::testing::UnitTest::GetInstance()->current_test_info();
        const auto loggerName = UniqueName(info, "named");

        spdlog::drop(loggerName);
        ASSERT_EQ(nullptr, spdlog::get(loggerName));

        Interstellar::Logging::Logger logger(loggerName);
        auto found = spdlog::get(loggerName);
        EXPECT_NE(nullptr, found);
        EXPECT_EQ(loggerName, found->name());
    }

    TEST_F(LoggerFixture, UsesExistingSpdlogInstanceIfPresent) {
        const auto& info = *::testing::UnitTest::GetInstance()->current_test_info();
        const auto loggerName = UniqueName(info, "existing");

        std::ostringstream oss;
        auto sink = std::make_shared<spdlog::sinks::ostream_sink_mt>(oss);
        auto pre = std::make_shared<spdlog::logger>(loggerName, sink);
        pre->set_level(spdlog::level::info);
        spdlog::register_logger(pre);

        Interstellar::Logging::Logger logger(loggerName);
        logger.LogInfo("hello {}", 123);

        pre->flush();
        EXPECT_NE(std::string::npos, oss.str().find("hello 123"));
    }

    TEST_F(LoggerFixture, RespectsLogLevelFiltering) {
        const auto& info = *::testing::UnitTest::GetInstance()->current_test_info();
        const auto loggerName = UniqueName(info, "levels");

        std::ostringstream oss;
        auto sink = std::make_shared<spdlog::sinks::ostream_sink_mt>(oss);
        sink->set_level(spdlog::level::warn);

        auto spd = std::make_shared<spdlog::logger>(loggerName, sink);
        spd->set_level(spdlog::level::warn);
        spdlog::register_logger(spd);

        Interstellar::Logging::Logger logger(loggerName);

        logger.LogDebug("debug drop");
        logger.LogInfo("info drop");
        logger.LogWarn("warn keep");
        logger.LogError("error keep");

        spd->flush();
        const std::string out = oss.str();
        EXPECT_NE(out.find("warn keep"), std::string::npos);
        EXPECT_NE(out.find("error keep"), std::string::npos);
        EXPECT_EQ(out.find("debug drop"), std::string::npos);
        EXPECT_EQ(out.find("info drop"), std::string::npos);
    }

    TEST_F(LoggerFixture, FormattedLoggingWrites) {
        const auto& info = *::testing::UnitTest::GetInstance()->current_test_info();
        const auto loggerName = UniqueName(info, "fmt");

        std::ostringstream oss;
        auto sink = std::make_shared<spdlog::sinks::ostream_sink_mt>(oss);
        auto spd = std::make_shared<spdlog::logger>(loggerName, sink);
        spd->set_level(spdlog::level::trace);
        spdlog::register_logger(spd);

        Interstellar::Logging::Logger logger(loggerName);
        logger.LogInfo("sum {} + {} = {}", 2, 3, 5);

        spd->flush();
        EXPECT_NE(std::string::npos, oss.str().find("sum 2 + 3 = 5"));
    }

    TEST_F(LoggerFixture, CreatesDefaultSinksWhenNotPreRegistered) {
        const auto& info = *::testing::UnitTest::GetInstance()->current_test_info();
        const auto loggerName = UniqueName(info, "sinks");

        spdlog::drop(loggerName);
        ASSERT_EQ(nullptr, spdlog::get(loggerName));

        Interstellar::Logging::Logger logger(loggerName, Interstellar::Logging::LogLevel::Info);
        auto spd = spdlog::get(loggerName);
        ASSERT_NE(nullptr, spd);

        const auto& sinks = spd->sinks();
        EXPECT_GE(sinks.size(), 2u);

        bool has_console = false, has_daily = false;
        for (const auto& s : sinks) {
            if (std::dynamic_pointer_cast<spdlog::sinks::stdout_color_sink_mt>(s)) has_console = true;
            if (std::dynamic_pointer_cast<spdlog::sinks::daily_file_sink_mt>(s))   has_daily = true;
        }
        EXPECT_TRUE(has_console);
        EXPECT_TRUE(has_daily);
    }

    TEST_F(LoggerFixture, MultipleLoggersConfiguredIndependently) {
        const auto& info = *::testing::UnitTest::GetInstance()->current_test_info();
        const auto A = UniqueName(info, "A");
        const auto B = UniqueName(info, "B");
        const auto C = UniqueName(info, "C");

        Interstellar::Logging::Logger loggerA(A, Interstellar::Logging::LogLevel::Error);
        Interstellar::Logging::Logger loggerB(B, Interstellar::Logging::LogLevel::Info);
        Interstellar::Logging::Logger loggerC(C, Interstellar::Logging::LogLevel::Trace);

        auto a = spdlog::get(A);
        auto b = spdlog::get(B);
        auto c = spdlog::get(C);

        ASSERT_NE(nullptr, a);
        ASSERT_NE(nullptr, b);
        ASSERT_NE(nullptr, c);

        EXPECT_EQ(spdlog::level::err, a->level());
        EXPECT_EQ(spdlog::level::info, b->level());
        EXPECT_EQ(spdlog::level::trace, c->level());
    }

    TEST_F(LoggerFixture, Smoke_DefaultPatternDoesNotCrash) {
        const auto& info = *::testing::UnitTest::GetInstance()->current_test_info();
        const auto loggerName = UniqueName(info, "pattern");

        std::ostringstream oss;
        auto sink = std::make_shared<spdlog::sinks::ostream_sink_mt>(oss);
        auto spd = std::make_shared<spdlog::logger>(loggerName, sink);
        spd->set_level(spdlog::level::info);
        spdlog::register_logger(spd);

        Interstellar::Logging::Logger logger(loggerName);
        logger.LogInfo("pattern smoke");

        spd->flush();
        EXPECT_NE(std::string::npos, oss.str().find("pattern smoke"));
    }

    // --- additions -----------------------------------------------------------

    TEST_F(LoggerFixture, DestructorFlushesInfoMessages) {
        const auto& info = *::testing::UnitTest::GetInstance()->current_test_info();
        const auto name = UniqueName(info, "dtorflush");

        std::ostringstream oss;
        auto sink = std::make_shared<spdlog::sinks::ostream_sink_mt>(oss);
        auto spd = std::make_shared<spdlog::logger>(name, sink);
        spd->set_level(spdlog::level::info);
        spdlog::register_logger(spd);

        {
            Interstellar::Logging::Logger logger(name);
            logger.LogInfo("flushed by dtor");
        } // ~Logger flushes

        EXPECT_NE(std::string::npos, oss.str().find("flushed by dtor"));
    }

    TEST_F(LoggerFixture, AdoptsRequestedLevelForPreRegisteredLogger) {
        const auto& info = *::testing::UnitTest::GetInstance()->current_test_info();
        const auto name = UniqueName(info, "adoptlvl");

        auto sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        auto spd = std::make_shared<spdlog::logger>(name, sink);
        spd->set_level(spdlog::level::info); // pre-existing at INFO
        spdlog::register_logger(spd);

        Interstellar::Logging::Logger wrapper(name, Interstellar::Logging::LogLevel::Error);
        EXPECT_EQ(spdlog::level::err, spd->level()); // wrapper should update it to ERR
    }

    TEST_F(LoggerFixture, HonorsEnvironmentLogDir) {
        const auto& info = *::testing::UnitTest::GetInstance()->current_test_info();
        const auto name = UniqueName(info, "envdir");

        // Short relative dir to avoid Windows path quirks
        const auto dir = std::filesystem::path("test_logs_envdir") / name;
        RemoveAllQuiet(dir);
        std::filesystem::create_directories(dir);

        EnvVarGuard guard("INTERSTELLAR_LOG_DIR", dir.string());

        {
            Interstellar::Logging::Logger logger(name, Interstellar::Logging::LogLevel::Info);
            logger.LogInfo("to-file");
            if (auto spd = spdlog::get(name)) spd->flush();  // determinism
        }

        spdlog::drop_all();

        const auto base = dir / (name + ".log");
        bool exists = std::filesystem::exists(base);

        if (!exists) {
            // Fallback: look for any rotated/variant file (e.g., daily sink moved it)
            for (auto& entry : std::filesystem::directory_iterator(dir)) {
                const auto& p = entry.path().filename().string();
                if (p.rfind(name, 0) == 0 && p.size() >= 4 && p.substr(p.size() - 4) == ".log") {
                    exists = true;
                    break;
                }
            }
        }

        EXPECT_TRUE(exists) << "Expected log file at (or variant under): " << base.string();

        RemoveAllQuiet(dir.parent_path());
    }


} // namespace Interstellar::Logging
