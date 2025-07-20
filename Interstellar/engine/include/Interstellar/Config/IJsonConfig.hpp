#pragma once
#include <string>
#include <nlohmann/json.hpp>

struct IJsonConfig {
    virtual ~IJsonConfig() = default;
    virtual std::string getNamespace() const = 0;
    virtual std::string getFilename()  const = 0;
    virtual nlohmann::json toJson()    const = 0;
    virtual void fromJson(const nlohmann::json&) = 0;
};
