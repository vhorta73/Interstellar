// interstellar/renderers/TestFileHelpers.hpp
#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include <stdexcept>

inline std::string write_temp_file(const std::string& basename,
    const std::string& contents) {
    namespace fs = std::filesystem;
    const fs::path dir = fs::temp_directory_path() / "interstellar_gl_tests";
    std::error_code ec;
    fs::create_directories(dir, ec); // don't throw on existing / permission issues
    if (ec) {
        throw std::runtime_error("Failed to create temp directory: " + dir.string() +
            " (" + ec.message() + ")");
    }

    const fs::path path = dir / basename;

    std::ofstream f(path, std::ios::binary);
    if (!f) {
        throw std::runtime_error("Failed to open temp file for writing: " + path.string());
    }

    if (!contents.empty()) {
        f.write(contents.data(), static_cast<std::streamsize>(contents.size()));
        if (!f) {
            throw std::runtime_error("Failed to write contents to file: " + path.string());
        }
    }

    f.flush();
    if (!f) {
        throw std::runtime_error("Failed to flush file: " + path.string());
    }
    f.close();

    return fs::absolute(path).string(); // narrow string (no UTF-8 conversion)
}

inline std::string write_1x1_png(const std::string& basename = "tex_1x1.png") {
    static const unsigned char kPng1x1[] = {
        0x89,0x50,0x4E,0x47,0x0D,0x0A,0x1A,0x0A,0x00,0x00,0x00,0x0D,0x49,0x48,0x44,0x52,
        0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x01,0x08,0x06,0x00,0x00,0x00,0x1F,0x15,0xC4,
        0x89,0x00,0x00,0x00,0x0A,0x49,0x44,0x41,0x54,0x78,0x9C,0x63,0xF8,0xCF,0xC0,0x00,
        0x00,0x03,0x01,0x01,0x00,0x18,0xDD,0x8D,0x7E,0x00,0x00,0x00,0x00,0x49,0x45,0x4E,
        0x44,0xAE,0x42,0x60,0x82
    };

    namespace fs = std::filesystem;
    const fs::path dir = fs::temp_directory_path() / "interstellar_gl_tests";
    std::error_code ec;
    fs::create_directories(dir, ec);
    if (ec) {
        throw std::runtime_error("Failed to create temp directory: " + dir.string() +
            " (" + ec.message() + ")");
    }

    const fs::path path = dir / basename;

    std::ofstream f(path, std::ios::binary);
    if (!f) {
        throw std::runtime_error("Failed to open temp file for writing: " + path.string());
    }

    f.write(reinterpret_cast<const char*>(kPng1x1),
        static_cast<std::streamsize>(sizeof(kPng1x1)));
    if (!f) {
        throw std::runtime_error("Failed to write PNG bytes to file: " + path.string());
    }

    f.flush();
    if (!f) {
        throw std::runtime_error("Failed to flush file: " + path.string());
    }
    f.close();

    return fs::absolute(path).string(); // narrow string (no UTF-8 conversion)
}
