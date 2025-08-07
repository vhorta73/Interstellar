#pragma once

#include <string>

/**
* @ingroup InterstellarConfig Interstellar Config
 * @brief Abstract interface for file-based config storage systems.
 *
 * Provides platform-agnostic file operations used by the JSON configuration layer.
 * Enables testability and pluggable I/O mechanisms such as in-memory mocks or virtual file systems.
 * 
 * @since 1.0
 */
struct IConfigStorage {
    virtual ~IConfigStorage() = default;

    /**
     * @brief Checks if a file or directory exists at the given path.
     * @param path Path to check (relative or absolute).
     * @return true if the file exists, false otherwise.
     */
    [[nodiscard]] virtual bool exists(const std::string& path) const = 0;

    /**
     * @brief Recursively creates directories for the given path.
     * @param path Directory path to create.
     */
    virtual void createDirectories(const std::string& path) = 0;

    /**
     * @brief Writes the specified content to a file, overwriting any existing content.
     * @param path File path to write to.
     * @param content Text content to write.
     */
    virtual void writeText(const std::string& path, const std::string& content) = 0;

    /**
     * @brief Reads all text content from the specified file.
     * @param path Path to the file to read.
     * @return Entire file content as a string.
     */
    [[nodiscard]] virtual std::string readText(const std::string& path) = 0;
};
