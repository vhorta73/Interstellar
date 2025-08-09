// Interstellar/Utils/Json/JsonHelpers.hpp
#pragma once
#include <optional>
#include <string>
#include <stdexcept>
#include <type_traits>
#include <nlohmann/json.hpp>

namespace Interstellar::Utils::Json {

    inline std::string extractBracketContent(const std::string& input) {
        auto start = input.find('[');
        auto end = input.find(']');

        if (start != std::string::npos && end != std::string::npos && end > start) {
            return input.substr(start + 1, end - start - 1);
        }
        return "";
    }

    inline int get_json_to_int(const nlohmann::json& j, const char* key) {
        const auto it = j.find(key);
        if (it == j.end()) throw std::runtime_error(std::string("Missing key: ") + key);
        if (it->is_number_integer()) return it->get<int>();
        if (it->is_number_float())   return static_cast<int>(it->get<double>());
        throw std::runtime_error(std::string("Key '") + key + "' is not numeric");
    }

    inline int get_json_to_int(const nlohmann::json& value) {
        if (value.is_number_integer()) return value.get<int>();
        if (value.is_number_float())   return static_cast<int>(value.get<double>());
        throw std::runtime_error("JSON value is not numeric (int-compatible)");
    }

    inline std::optional<int> get_json_to_opt_int(const nlohmann::json& j, const char* key) {
        const auto it = j.find(key);
        if (it == j.end() || it->is_null())   return std::nullopt;
        if (it->is_number_integer())          return it->get<int>();
        if (it->is_number_float())            return static_cast<int>(it->get<double>());
        return std::nullopt;
    }
    
    inline std::optional<int> get_json_to_opt_int(const nlohmann::json& value) {
        if (value.is_null())                return std::nullopt;
        if (value.is_number_integer())      return value.get<int>();
        if (value.is_number_float())        return static_cast<int>(value.get<double>());
        return std::nullopt;
    }

    inline float get_json_to_float(const nlohmann::json& j, const char* key) {
        const auto it = j.find(key);
        if (it == j.end()) throw std::runtime_error(std::string("Missing key: ") + key);
        if (it->is_number_float())   return static_cast<float>(it->get<double>());
        if (it->is_number_integer()) return static_cast<float>(it->get<long long>());
        throw std::runtime_error(std::string("Key '") + key + "' is not numeric");
    }

    inline float get_json_to_float(const nlohmann::json& value) {
        if (value.is_number_float())   return static_cast<float>(value.get<double>());
        if (value.is_number_integer()) return static_cast<float>(value.get<long long>());
        throw std::runtime_error("JSON value is not numeric (float-compatible)");
    }

    inline std::optional<float> get_json_to_opt_float(const nlohmann::json& j, const char* key) {
        const auto it = j.find(key);
        if (it == j.end() || it->is_null()) return std::nullopt;
        if (it->is_number_float())   return static_cast<float>(it->get<double>());
        if (it->is_number_integer()) return static_cast<float>(it->get<long long>());
        return std::nullopt;
    }

    inline std::string get_json_to_string(const nlohmann::json& j, const char* key) {
        const auto it = j.find(key);
        if (it == j.end()) throw std::runtime_error(std::string("Missing key: ") + key);
        if (!it->is_string()) throw std::runtime_error(std::string("Key '") + key + "' is not string");
        return it->get<std::string>();
    }

    inline std::optional<std::string> get_json_to_opt_string(const nlohmann::json& j, const char* key) {
        const auto it = j.find(key);
        if (it == j.end() || it->is_null()) return std::nullopt;
        if (!it->is_string()) return std::nullopt;
        return it->get<std::string>();
    }

} // namespace Interstellar::Utils::Json
