#include "CppUnitTest.h"

#include <string>
#include <vector>
#include <map>
#include <nlohmann/json.hpp>

#include "Interstellar/Config/IConfigStorage.hpp"
#include "Interstellar/Config/AppConfig.hpp"
#include "Interstellar/Config/JsonImpl/UserInfoConfig.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

struct FileWrite { std::string path, content; };

/// Fake storage to capture all calls for assertion
struct FakeConfigStorage : IConfigStorage {
    std::vector<std::string> dirsCreated;
    std::vector<FileWrite> writes;
    std::map<std::string, std::string> files;

    bool exists(const std::string& path) const override {
        return files.find(path) != files.end();
    }
    void createDirectories(const std::string& path) override {
        dirsCreated.push_back(path);
    }
    void writeText(const std::string& path,
        const std::string& content) override {
        writes.push_back({ path,content });
        files[path] = content;
    }
    std::string readText(const std::string& path) override {
        auto it = files.find(path);
        return it != files.end() ? it->second : std::string();
    }
};

namespace AppConfigTest {

    TEST_CLASS(JsonIntentTests)
    {
    public:
        FakeConfigStorage storage;
        AppConfig* cfg = nullptr;
        std::string rootDir = "root";

        TEST_METHOD_INITIALIZE(Init)
        {
            cfg = AppConfig::New(&storage, rootDir);
        }

        TEST_METHOD_CLEANUP(Cleanup)
        {
            delete cfg;
        }

        TEST_METHOD(TestSaveIntent)
        {
            auto userConfig = new UserInfoConfig{ "TestUser", 99 };
            // Act: save a user config
            cfg->save(userConfig);

            // Assert directory creation
            Assert::IsTrue(!storage.dirsCreated.empty());

            auto dir = userConfig->getNamespace();
            Assert::AreEqual(std::string("user"), dir);
            auto fileName = userConfig->getFilename();
            Assert::AreEqual(std::string("info.json"), fileName);
            Assert::AreEqual(std::string(rootDir + "/" + dir), storage.dirsCreated[0]);

            // Assert file path
            Assert::AreEqual(
                std::string(rootDir+"/"+dir+"/"+fileName),
                storage.writes[0].path
            );

            // Assert JSON content
            auto j = nlohmann::json::parse(storage.writes[0].content);
            Assert::AreEqual(std::string("TestUser"), j["Name"].get<std::string>());
            Assert::AreEqual(99, j["Age"].get<int>());
        }

        TEST_METHOD(TestLoadIntent)
        {
            auto userConfig = new UserInfoConfig{ "TestUser", 99 };
            //Arrange: seed the storage with a JSON string
            std::string path = rootDir + "/user/info.json";
            storage.files[path] = R"({"Name":"TestUser","Age":99})";

            //Act: load
            auto loaded = cfg->load<UserInfoConfig>();

            //Assert fields populated
            Assert::AreEqual(std::string("TestUser"), loaded->Name);
            Assert::AreEqual(99, loaded->Age);
        }
    };

}  // namespace ConfigStorageTests
