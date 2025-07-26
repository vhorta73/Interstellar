#pragma once

#include "JsonManager.hpp"
#include "IJsonConfig.hpp"
#include <memory>
#include <type_traits>

#include "Interstellar/Config/Storage/FileSystem.cpp"

struct AppConfig {
    /// @param storage  platform-specific or test stub implementing IConfigStorage. Defaults to FileSystemStorage which uses the filesystem for reading/writing.
    /// @param root     base config directory (e.g. "assets/config")
    static AppConfig* New(IConfigStorage* storage = new Interstellar::Config::Storage::FileSystem,
        const std::string& root = "assets/config")
    {
        return new AppConfig(storage, root);
    }

    /// Serialize & save, then destroy the impl
    void save(IJsonConfig* impl) {
        manager.save(*impl);
    }

    /// Instantiate T, load into it if data exists, and return it
    template<typename T>
    std::unique_ptr<T> load(const std::string& subNs = "") {
        static_assert(std::is_base_of<IJsonConfig, T>::value,
            "T must implement IJsonConfig");
        auto ptr = std::make_unique<T>();
        manager.load(*ptr, subNs);
        return ptr;
    }

private:
    AppConfig(IConfigStorage* storage, const std::string& root)
        : manager(storage, root)
    {
    }

    JsonManager manager;
};

