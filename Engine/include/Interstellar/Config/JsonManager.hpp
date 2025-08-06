#pragma once

#include "IConfigStorage.hpp"
#include "IJsonConfig.hpp"
#include <string>
#include <nlohmann/json.hpp>


struct JsonManager {
    JsonManager(IConfigStorage* storage, const std::string& root)
        : storage(storage), rootDir(root) {
    }

    void save(const IJsonConfig& cfg, const std::string& subNs = "") {
        std::string ns = cfg.getNamespace();
        if (!subNs.empty()) ns += "/" + subNs;
        std::string dir = rootDir + "/" + ns;
        storage->createDirectories(dir);
        std::string path = dir + "/" + cfg.getFilename();
        storage->writeText(path, cfg.toJson().dump(4));
    }

    void load(IJsonConfig& cfg, const std::string& subNs = "") {
        std::string ns = cfg.getNamespace();
        if (!subNs.empty()) ns += "/" + subNs;
        std::string path = rootDir + "/" + ns + "/" + cfg.getFilename();
        if (!storage->exists(path)) return;
        auto txt = storage->readText(path);
        auto j = nlohmann::json::parse(txt);
        cfg.fromJson(j);
    }

private:
    IConfigStorage* storage;
    std::string rootDir;
};