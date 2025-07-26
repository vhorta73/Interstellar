#pragma once
#include "IJsonConfig.hpp"
#include <map>
#include <vector>
#include <functional>

struct JsonConfigBase : IJsonConfig {
protected:
    std::vector<std::string> fieldOrder;
    std::map<std::string, std::function<nlohmann::json()>> getters;
    std::map<std::string, std::function<void(const nlohmann::json&)>> setters;

    void registerField(
        std::string key,
        std::function<nlohmann::json()> get,
        std::function<void(const nlohmann::json&)> set
    ) {
        fieldOrder.push_back(key);
        getters[key] = std::move(get);
        setters[key] = std::move(set);
    }

    void applyJson(const nlohmann::json& j) {
        for (auto& key : fieldOrder)
            if (j.contains(key))
                setters.at(key)(j.at(key));
    }

public:
    JsonConfigBase() = default;
    JsonConfigBase(const nlohmann::json& j) { applyJson(j); }
    JsonConfigBase(const std::map<std::string, nlohmann::json>& m)
        : JsonConfigBase(nlohmann::json(m)) {
    }

    nlohmann::json toJson() const override {
        nlohmann::json j;
        for (auto& key : fieldOrder)
            j[key] = getters.at(key)();
        return j;
    }

    void fromJson(const nlohmann::json& j) override {
        applyJson(j);
    }
};
