#pragma once
#include "../JsonConfigBase.hpp"

struct UserInfoConfig : JsonConfigBase {
    std::string Name;
    int         Age = 0;

    UserInfoConfig() {
        registerField("Name",
            [this]() { return Name; },
            [this](auto& v) { Name = v.get<std::string>(); });
        registerField("Age",
            [this]() { return Age; },
            [this](auto& v) { Age = v.get<int>(); });
    }

    // 2) convenience ctor: delegates to default, then sets members
    UserInfoConfig(const std::string& name, int age)
        : UserInfoConfig()    // run registration
    {
        Name = name;
        Age = age;
    }

    // JSON based actor (optional)
    UserInfoConfig(const nlohmann::json& j)
        : UserInfoConfig()    // register fields
    {
        applyJson(j);
    }

    std::string getNamespace() const override { return "user"; }
    std::string getFilename()  const override { return "info.json"; }
};
