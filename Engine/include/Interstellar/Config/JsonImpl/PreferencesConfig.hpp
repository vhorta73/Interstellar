// PreferencesConfig.hpp
#pragma once

#include <string>
#include <nlohmann/json.hpp>

struct PreferencesConfig {
    // your two fields
    std::string Language;
    bool        DarkMode = false;

    // fluent setters
     PreferencesConfig& setLanguage(const std::string& lang) {
        Language = lang;
        return *this;
    }
    PreferencesConfig& setDarkMode(bool dark) {
        DarkMode = dark;
        return *this;
    }

    static constexpr auto Namespace = "preferences";
    static constexpr auto Filename = "prefs.json";
};

// automatic JSON mapping (to_json/from_json)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    PreferencesConfig,
    Language,
    DarkMode
);
