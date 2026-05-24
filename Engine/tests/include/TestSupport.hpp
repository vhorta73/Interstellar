#pragma once

#include <gtest/gtest.h>
#include <optional>
#include <string>
#include <fstream>
#include <sstream>

namespace TestSupport {

    inline constexpr float kEPS = 1e-4f;

    inline std::string read_text_file(const std::string& path) {
        std::ifstream file(path, std::ios::binary);
        if (!file) throw std::runtime_error("Failed to open: " + path);
        std::ostringstream oss; oss << file.rdbuf();
        return oss.str();
    }

    inline std::string bracket_core(std::string s) {
        if (s.size() >= 2 && s.front() == '[' && s.back() == ']')
            return s.substr(1, s.size() - 2);
        return s;
    }

    // optional<T> == optional<T>
    template <typename T>
    inline void expect_opt_eq(const std::optional<T>& got,
        const std::optional<T>& exp,
        const char* what) {
        if (exp.has_value()) {
            ASSERT_TRUE(got.has_value()) << what << " expected a value";
            EXPECT_EQ(*got, *exp) << what;
        }
        else {
            EXPECT_FALSE(got.has_value()) << what << " expected nullopt";
        }
    }

    // optional<float> ~= optional<float>
    inline void expect_opt_float_near_optopt(const std::optional<float>& got,
        const std::optional<float>& exp,
        float eps,
        const char* what) {
        if (exp.has_value()) {
            ASSERT_TRUE(got.has_value()) << what << " expected a value";
            EXPECT_NEAR(*got, *exp, eps) << what;
        }
        else {
            EXPECT_FALSE(got.has_value()) << what << " expected nullopt";
        }
    }

    // float ~= optional<float>
    inline void expect_opt_float_near_valopt(float got,
        const std::optional<float>& exp,
        float eps,
        const char* what) {
        if (exp.has_value()) {
            EXPECT_NEAR(got, *exp, eps) << what;
        }
        else {
            SUCCEED() << what << " expected nullopt; runtime has non-optional";
        }
    }

}
