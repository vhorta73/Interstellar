#include "Interstellar/Config/JsonImpl/ElementDatabase.hpp"
#include "Interstellar/Config/JsonImpl/ElementConfig.hpp"
#include <fstream>
#include <nlohmann/json.hpp>
#include <iostream>

namespace Interstellar::Config::JsonImpl {

    bool ElementDatabase::loadFromFile(const std::string& path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            std::cerr << "[ElementDatabase] Failed to open file: " << path << "\n";
            return false;
        }

        nlohmann::json jsonArray;
        try {
            file >> jsonArray;
        }
        catch (const std::exception& e) {
            std::cerr << "[ElementDatabase] JSON parsing error: " << e.what() << "\n";
            return false;
        }

        if (!jsonArray.is_array()) {
            std::cerr << "[ElementDatabase] Expected array of elements in: " << path << "\n";
            return false;
        }

        data_.clear();

        for (const auto& jsonElem : jsonArray) {
            ElementConfig config;
            config.fromJson(jsonElem);
            ElementData data = config.toData();

            if (data.isValid()) {
                data_[data.symbol] = std::move(data);
            }
            else {
                std::cerr << "[ElementDatabase] Skipped invalid element entry.\n";
            }
        }

        return !data_.empty();
    }

    const ElementData* ElementDatabase::get(const std::string& symbol) const {
        auto it = data_.find(symbol);
        return it != data_.end() ? &it->second : nullptr;
    }

    const std::unordered_map<std::string, ElementData>& ElementDatabase::all() const noexcept {
        return data_;
    }

}
