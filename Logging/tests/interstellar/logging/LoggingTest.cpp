// Logging/tests/LoggingTests.cpp
#include <gtest/gtest.h>

#include <sstream>
#include <filesystem>
#include <fstream>
#include <optional>
#include <cstdlib>
#include <thread>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/ostream_sink.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "Interstellar/Logging/Logging.hpp"
#include "Interstellar/Logging/LogLevel.hpp"

// Put helpers in an anonymous namespace to avoid polluting symbols,
// but keep TEST_F in the global namespace so VS filters work reliably.
namespace {
    // Unique, spdlog-safe logger names per test
    std::string UniqueName(const ::testing::TestInfo& ti, std::string suffix = {}) {
        std::string n = std::string(ti.test_suite_name()) + "." + ti.name();
        if (!suffix.empty()) n += "." + suffix;
        for (char& c : n) if (c == '/' || c == ' ') c = '_';
        return n;
    }

    // RAII env var guard (cross-platform)
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
            if (old) _putenv_s(key.c_str(), old->c_str()); else _putenv_s(key.c_str(), "");
#else
            if (old) ::setenv(key.c_str(), old->c_str(), 1); else ::unsetenv(key.c_str());
#endif
        }
    };

    void RemoveAllQuiet(const std::filesystem::path& p) {
        std::error_code ec;
        std::filesystem::remove_all(p, ec);
    }
}

// Use a suite name that carries the module grouping but stays namespace-free.
class LoggingLoggerFixture : public ::testing::Test {
protected:
    void TearDown() override {
        // Keep registry clean between tests
        spdlog::drop_all();
    }
};

using namespace Interstellar::Logging;

TEST_F(LoggingLoggerFixture, RegistersNamedLogger) {
    const auto& info = *::testing::UnitTest::GetInstance()->current_test_info();
    const auto loggerName = UniqueName(info, "named");

    spdlog::drop(loggerName);
    ASSERT_EQ(nullptr, spdlog::get(loggerName));

    Logger logger(loggerName);
    auto found = spdlog::get(loggerName);
    EXPECT_NE(nullptr, found);
    EXPECT_EQ(loggerName, found->name());
}

TEST_F(LoggingLoggerFixture, UsesExistingSpdlogInstanceIfPresent) {
    const auto& info = *::testing::UnitTest::GetInstance()->current_test_info();
    const auto loggerName = UniqueName(info, "existing");

    std::ostringstream oss;
    auto sink = std::make_shared<spdlog::sinks::ostream_sink_mt>(oss);
    auto pre = std::make_shared<spdlog::logger>(loggerName, sink);
    pre->set_level(spdlog::level::info);
    spdlog::register_logger(pre);

    Logger logger(loggerName);
    logger.LogInfo("hello {}", 123);

    pre->flush();
    EXPECT_NE(std::string::npos, oss.str().find("hello 123"));
}

TEST_F(LoggingLoggerFixture, RespectsLogLevelFiltering) {
    const auto& info = *::testing::UnitTest::GetInstance()->current_test_info();
    const auto loggerName = UniqueName(info, "levels");

    std::ostringstream oss;
    auto sink = std::make_shared<spdlog::sinks::ostream_sink_mt>(oss);
    sink->set_level(spdlog::level::warn);

    auto spd = std::make_shared<spdlog::logger>(loggerName, sink);
    spd->set_level(spdlog::level::warn);
    spdlog::register_logger(spd);

    Logger logger(loggerName);

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

TEST_F(LoggingLoggerFixture, FormattedLoggingWrites) {
    const auto& info = *::testing::UnitTest::GetInstance()->current_test_info();
    const auto loggerName = UniqueName(info, "fmt");

    std::ostringstream oss;
    auto sink = std::make_shared<spdlog::sinks::ostream_sink_mt>(oss);
    auto spd = std::make_shared<spdlog::logger>(loggerName, sink);
    spd->set_level(spdlog::level::trace);
    spdlog::register_logger(spd);

    Logger logger(loggerName);
    logger.LogInfo("sum {} + {} = {}", 2, 3, 5);

    spd->flush();
    EXPECT_NE(std::string::npos, oss.str().find("sum 2 + 3 = 5"));
}

TEST_F(LoggingLoggerFixture, CreatesDefaultSinksWhenNotPreRegistered) {
    const auto& info = *::testing::UnitTest::GetInstance()->current_test_info();
    const auto loggerName = UniqueName(info, "sinks");

    spdlog::drop(loggerName);
    ASSERT_EQ(nullptr, spdlog::get(loggerName));

    Logger logger(loggerName, LogLevel::Info);
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

TEST_F(LoggingLoggerFixture, MultipleLoggersConfiguredIndependently) {
    const auto& info = *::testing::UnitTest::GetInstance()->current_test_info();
    const auto A = UniqueName(info, "A");
    const auto B = UniqueName(info, "B");
    const auto C = UniqueName(info, "C");

    Logger loggerA(A, LogLevel::Error);
    Logger loggerB(B, LogLevel::Info);
    Logger loggerC(C, LogLevel::Trace);

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

TEST_F(LoggingLoggerFixture, Smoke_DefaultPatternDoesNotCrash) {
    const auto& info = *::testing::UnitTest::GetInstance()->current_test_info();
    const auto loggerName = UniqueName(info, "pattern");

    std::ostringstream oss;
    auto sink = std::make_shared<spdlog::sinks::ostream_sink_mt>(oss);
    auto spd = std::make_shared<spdlog::logger>(loggerName, sink);
    spd->set_level(spdlog::level::info);
    spdlog::register_logger(spd);

    Logger logger(loggerName);
    logger.LogInfo("pattern smoke");

    spd->flush();
    EXPECT_NE(std::string::npos, oss.str().find("pattern smoke"));
}

TEST_F(LoggingLoggerFixture, DestructorFlushesInfoMessages) {
    const auto& info = *::testing::UnitTest::GetInstance()->current_test_info();
    const auto name = UniqueName(info, "dtorflush");

    std::ostringstream oss;
    auto sink = std::make_shared<spdlog::sinks::ostream_sink_mt>(oss);
    auto spd = std::make_shared<spdlog::logger>(name, sink);
    spd->set_level(spdlog::level::info);
    spdlog::register_logger(spd);

    {
        Logger logger(name);
        logger.LogInfo("flushed by dtor");
    } // ~Logger flushes

    EXPECT_NE(std::string::npos, oss.str().find("flushed by dtor"));
}

TEST_F(LoggingLoggerFixture, AdoptsRequestedLevelForPreRegisteredLogger) {
    const auto& info = *::testing::UnitTest::GetInstance()->current_test_info();
    const auto name = UniqueName(info, "adoptlvl");

    auto sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto spd = std::make_shared<spdlog::logger>(name, sink);
    spd->set_level(spdlog::level::info); // pre-existing at INFO
    spdlog::register_logger(spd);

    Logger wrapper(name, LogLevel::Error);
    EXPECT_EQ(spdlog::level::err, spd->level()); // wrapper should update it to ERR
}

TEST_F(LoggingLoggerFixture, HonorsEnvironmentLogDir) {
    const auto& info = *::testing::UnitTest::GetInstance()->current_test_info();
    const auto name = UniqueName(info, "envdir");

    const auto dir = std::filesystem::path("test_logs_envdir") / name;
    RemoveAllQuiet(dir);
    std::filesystem::create_directories(dir);

    EnvVarGuard guard("INTERSTELLAR_LOG_DIR", dir.string());

    {
        Logger logger(name, LogLevel::Info);
        logger.LogInfo("to-file");
        if (auto spd = spdlog::get(name)) spd->flush();
    }

    spdlog::drop_all();

    const auto base = dir / (name + ".log");
    bool exists = std::filesystem::exists(base);
    if (!exists) {
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

TEST_F(LoggingLoggerFixture, SetsDefaultLoggerWhenGeneric) {
    const auto genericName = std::string(LOG_GENERIC);
    Logger lg(genericName, LogLevel::Info);

    auto def = spdlog::default_logger();
    ASSERT_NE(nullptr, def);
    EXPECT_EQ(genericName, def->name());
}

TEST_F(LoggingLoggerFixture, AppliesLevelToExistingSpdLogger) {
    const auto& ti = *::testing::UnitTest::GetInstance()->current_test_info();
    const auto name = UniqueName(ti, "level_override");

    auto sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto pre = std::make_shared<spdlog::logger>(name, sink);
    pre->set_level(spdlog::level::trace);
    spdlog::register_logger(pre);

    Logger logger(name, LogLevel::Error);

    auto spd = spdlog::get(name);
    ASSERT_NE(nullptr, spd);
    EXPECT_EQ(spdlog::level::err, spd->level());
}

TEST_F(LoggingLoggerFixture, SameNameSharesUnderlyingSpdLogger) {
    const auto& ti = *::testing::UnitTest::GetInstance()->current_test_info();
    const auto name = UniqueName(ti, "same");

    Logger a(name, LogLevel::Info);
    Logger b(name, LogLevel::Info);

    auto pa = a.GetSpdLogger();
    auto pb = b.GetSpdLogger();
    ASSERT_TRUE(pa && pb);
    EXPECT_EQ(pa.get(), pb.get()); // same instance
}

// Global singletons smoke test
TEST_F(LoggingLoggerFixture, GlobalSingletonsSmoke) {
    logGeneric.LogInfo("generic ok");
    logInit.LogWarn("init ok");
    logConfig.LogDebug("config ok");
    logGraphic.LogError("graphic ok");
    SUCCEED();
}

// Parse helpers / formatting behavior
TEST_F(LoggingLoggerFixture, ParseHelpers_WorkAsExpected) {
    using enum LogLevel;

    // TryParse: positives & aliases
    EXPECT_TRUE(TryParseLogLevel("trace").has_value());
    EXPECT_EQ(Warn, *TryParseLogLevel("warning"));
    EXPECT_EQ(Critical, *TryParseLogLevel("fatal"));

    // TryParse: negative
    EXPECT_FALSE(TryParseLogLevel("nope"));

    // Or-with-default
    EXPECT_EQ(Info, ParseLogLevelOr("INFO", Warn));
    EXPECT_EQ(Warn, ParseLogLevelOr("???", Warn));

    // Throwing parser
    EXPECT_NO_THROW({ auto lvl = ParseLogLevel("debug"); (void)lvl; });
    EXPECT_THROW(ParseLogLevel(""), std::invalid_argument);

    // ToString / operator<<
    EXPECT_STREQ("ERROR", std::string(ToString(Error)).c_str());
    std::ostringstream os;
    os << Critical;
    EXPECT_EQ("CRITICAL", os.str());
}
