#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

#include <map>
#include <string>
#include <vector>
#include <memory>

#include "Interstellar/Config/IConfigStorage.hpp"
#include "Interstellar/Config/AppConfig.hpp"
#include "Interstellar/Config/JsonImpl/UserInfoConfig.hpp"

namespace Interstellar_Config_AppConfig {
    struct FileWrite { std::string path, content; };

    // Fake storage to capture all calls for assertion
    struct FakeConfigStorage : IConfigStorage {
        std::vector<std::string>      dirsCreated;
        std::vector<FileWrite>        writes;
        std::map<std::string, std::string> files;

        void reset() {
            dirsCreated.clear();
            writes.clear();
            files.clear();
        }

        bool exists(const std::string& path) const override {
            return files.find(path) != files.end();
        }
        void createDirectories(const std::string& path) override {
            dirsCreated.push_back(path);
        }
        void writeText(const std::string& path, const std::string& content) override {
            writes.push_back({ path, content });
            files[path] = content;
        }
        std::string readText(const std::string& path) override {
            auto it = files.find(path);
            return (it != files.end()) ? it->second : std::string{};
        }
    };

    class AppConfigTests : public ::testing::Test {
    protected:
        FakeConfigStorage storage;
        std::unique_ptr<AppConfig> cfg;   // safer ownership than raw pointer
        std::string rootDir = "root";

        void SetUp() override {
            storage.reset();
            // If AppConfig::New returns raw pointer, wrap it
            cfg.reset(AppConfig::New(&storage, rootDir));
            ASSERT_NE(cfg, nullptr) << "AppConfig::New returned null";
        }

        void TearDown() override {
            cfg.reset();
            storage.reset();
        }
    };

    TEST_F(AppConfigTests, SaveUserInfo_WritesExpectedJsonAndPaths)
    {
        auto userConfig = std::make_unique<UserInfoConfig>("TestUser", 99);

        ASSERT_NO_THROW(cfg->save(userConfig.get()));

        // Directory creation recorded
        ASSERT_FALSE(storage.dirsCreated.empty());
        EXPECT_EQ("user", userConfig->getNamespace());
        EXPECT_EQ("info.json", userConfig->getFilename());
        EXPECT_EQ(rootDir + "/user", storage.dirsCreated.front());

        // A write happened
        ASSERT_FALSE(storage.writes.empty());
        const auto& firstWrite = storage.writes.front();

        // Path formed correctly
        EXPECT_EQ(rootDir + "/user/info.json", firstWrite.path);

        // JSON content correct
        ASSERT_NO_THROW({
          auto j = nlohmann::json::parse(firstWrite.content);
          EXPECT_EQ("TestUser", j.at("Name").get<std::string>());
          EXPECT_EQ(99,         j.at("Age").get<int>());
            });
    }

    TEST_F(AppConfigTests, LoadUserInfo_ReadsFromStorageAndParses)
    {
        // Arrange: seed storage with a valid JSON file
        const std::string path = rootDir + "/user/info.json";
        storage.files[path] = R"({"Name":"TestUser","Age":99})";

        // Act
        std::unique_ptr<UserInfoConfig> loaded;
        ASSERT_NO_THROW({
          loaded = cfg->load<UserInfoConfig>();
            });

        ASSERT_NE(loaded, nullptr);
        EXPECT_EQ("TestUser", loaded->Name);
        EXPECT_EQ(99, loaded->Age);
    }
};