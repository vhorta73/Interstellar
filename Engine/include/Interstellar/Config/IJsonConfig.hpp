#pragma once

#include <string>
#include <nlohmann/json.hpp>

/**
 * @brief Interface for JSON-serializable configuration objects.
 *
 * Classes implementing this interface can be loaded/saved through
 * JsonManager or similar systems that abstract file I/O.
 * 
 * @since 1.0
 */
struct IJsonConfig {
    virtual ~IJsonConfig() = default;

    /**
     * @brief Returns the logical namespace of this config (e.g., "gameplay", "data").
     * Used as a subdirectory path by config loaders.
     */
    virtual std::string getNamespace() const = 0;

    /**
     * @brief Returns the filename used for this config (e.g., "settings.json").
     */
    virtual std::string getFilename() const = 0;

    /**
     * @brief Serializes the current object into a JSON object.
     * @return nlohmann::json representing this config.
     */
    [[nodiscard]] virtual nlohmann::json toJson() const = 0;

    /**
     * @brief Loads object state from the provided JSON object.
     * @param j JSON object containing config fields.
     */
    virtual void fromJson(const nlohmann::json& j) = 0;
};
