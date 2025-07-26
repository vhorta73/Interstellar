#pragma once

#include <string>

/// Abstraction for all file operations used by the JSON config system.
struct IConfigStorage {

    virtual ~IConfigStorage() = default;

    /// Returns true if the given path exists.
    virtual bool exists(const std::string& path) const = 0;

    /// Recursively create this directory (and any parents).
    virtual void createDirectories(const std::string& path) = 0;

    /// Write text content to the given file path (overwriting if it exists).
    virtual void writeText(const std::string& path, const std::string& content) = 0;

    /// Read the entire text content of the given file path.
    virtual std::string readText(const std::string& path) = 0;
};
