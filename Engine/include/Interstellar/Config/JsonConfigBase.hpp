#pragma once

#include "Interstellar/Config/IJsonConfig.hpp"
#include <map>
#include <vector>
#include <functional>
#include <nlohmann/json.hpp>

/**
 * @file JsonConfigBase.hpp
 * @brief Base class for defining JSON-serializable config types.
 * @ingroup InterstellarConfig
 */

 /**
  * @ingroup InterstellarConfig
  * @addtogroup InterstellarConfig
  * @{
  *
  * @brief Provides field registration, ordering, and automatic (de)serialization using nlohmann::json.
  *
  * Extend this class to build structured JSON configurations that can be read/written with `JsonManager`.
  * Fields must be registered via `registerField()` in the constructor.
  *
  * @since 1.0
  */
struct JsonConfigBase : IJsonConfig {
protected:
    /// Field serialization order and existence control
    std::vector<std::string> fieldOrder;

    /// Map of field keys to getter functions that return their JSON representation.
    std::map<std::string, std::function<nlohmann::json()>> getters;

    /// Map of field keys to setter functions that extract their value from JSON.
    std::map<std::string, std::function<void(const nlohmann::json&)>> setters;

    /**
     * @brief Registers a field for automatic (de)serialization.
     *
     * Each registered field is serialized in order and conditionally populated during `fromJson`.
     * Typically called in the derived class constructor.
     *
     * @param key Field name in the JSON object.
     * @param get Lambda returning the current field value as JSON.
     * @param set Lambda accepting a JSON value to set the field.
     *
     * @warning Lambdas must capture field references correctly. Capturing copies may break sync.
     *
     * @since 1.0
     */
    void registerField(
        std::string key,
        std::function<nlohmann::json()> get,
        std::function<void(const nlohmann::json&)> set
    ) {
        fieldOrder.push_back(std::move(key));
        getters[fieldOrder.back()] = std::move(get);
        setters[fieldOrder.back()] = std::move(set);
    }

    /**
     * @brief Populates registered fields from a JSON object.
     *
     * Skips missing fields. Calls `onParsed()` after parsing completes.
     *
     * @param j The JSON input object.
     * @since 1.0
     */
    void applyJson(const nlohmann::json& j) {
        for (const auto& key : fieldOrder) {
            if (j.contains(key)) {
                setters.at(key)(j.at(key));
            }
        }
        onParsed();
    }

public:
    /**
     * @brief Default constructor.
     * @since 1.0
     */
    JsonConfigBase() = default;

    /**
     * @brief Construct from a JSON object.
     * @param j The JSON input to deserialize.
     * @since 1.0
     */
    explicit JsonConfigBase(const nlohmann::json& j) {
        applyJson(j);
    }

    /**
     * @brief Construct from a key-value map of JSON fields.
     * @param m A map of string-to-json values.
     * @since 1.0
     */
    explicit JsonConfigBase(const std::map<std::string, nlohmann::json>& m)
        : JsonConfigBase(nlohmann::json(m)) {
    }

    /**
     * @brief Virtual destructor for polymorphic deletion.
     * @since 1.0
     */
    virtual ~JsonConfigBase() = default;

    /**
     * @brief Optional hook after all fields have been parsed from JSON.
     *
     * Override in derived classes to implement validation or transformation logic.
     * @return void
     * @since 1.0
     */
    virtual void onParsed() {}

    /**
     * @brief Serialize the config to JSON using registered fields.
     * @return A JSON object representing this config.
     * @since 1.0
     */
    [[nodiscard]] nlohmann::json toJson() const override {
        nlohmann::json j;
        for (const auto& key : fieldOrder) {
            j[key] = getters.at(key)();
        }
        return j;
    }

    /**
     * @brief Deserialize from a JSON object using registered fields.
     * @param j The JSON input.
     * @return void
     * @since 1.0
     */
    void fromJson(const nlohmann::json& j) override {
        applyJson(j);
    }
};
/** @} */
