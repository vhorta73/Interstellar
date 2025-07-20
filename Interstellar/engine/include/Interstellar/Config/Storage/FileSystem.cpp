#pragma once

#include <filesystem>
#include <fstream>

#include "Interstellar/Config/IConfigStorage.hpp"

namespace Interstellar::Config::Storage {

    struct FileSystem : IConfigStorage {
        bool exists(const std::string& path) const override {
            return std::filesystem::exists(path);
        }
        void createDirectories(const std::string& path) override {
            std::filesystem::create_directories(path);
        }
        void writeText(const std::string& path, const std::string& content) override {
            std::ofstream ofs(path, std::ios::trunc);
            ofs << content;
        }
        std::string readText(const std::string& path) override {
            std::ifstream ifs(path);
            return std::string((std::istreambuf_iterator<char>(ifs)), {});
        }
    };
}  // namespace Interstellar::Config
