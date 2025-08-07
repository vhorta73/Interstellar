# Engine/CMakeLists.txt (no cmake_minimum_required or project here)

# Ensure C++20 features (e.g. defaulted comparisons, using enum) are enabled
# This inherits the global CMAKE_CXX_STANDARD set in the root CMakeLists

# 0. Generate stb_image.h from stb_image.hpp for legacy includes
execute_process(COMMAND ${CMAKE_COMMAND} -E copy
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Interstellar/Graphics/OpenGL/stb/stb_image.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Interstellar/Graphics/OpenGL/stb/stb_image.h"
    RESULT_VARIABLE _stb_copy
)
if(NOT _stb_copy EQUAL 0)
  message(FATAL_ERROR "Failed to generate stb_image.h for stb support.")
endif()

# 1. Gather source files
file(GLOB_RECURSE ENGINE_SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)
add_library(Engine STATIC ${ENGINE_SOURCES})

# 2. Include directories
target_include_directories(Engine
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# 3. Fetch third-party dependencies via FetchContent
include(FetchContent)
# Logging
FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG        v1.11.0
)
# Formatting
FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG        10.1.1
)
# JSON
FetchContent_Declare(
  nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG        v3.11.2
)
# OpenGL loader
FetchContent_Declare(
  glad
  GIT_REPOSITORY https://github.com/Dav1dde/glad.git
  GIT_TAG        v0.1.36
)
# Math
FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG        0.9.9.8
)
# Window/input
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG        3.3.8
)
# CLI parser
FetchContent_Declare(
  CLI11
  GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
  GIT_TAG        v2.2.0
)
FetchContent_MakeAvailable(
  spdlog
  fmt
  nlohmann_json
  glad
  glm
  glfw
  CLI11
)

# 4. Find system libraries (Vulkan)
find_package(Vulkan REQUIRED)

# 5. Link dependencies into Engine
target_link_libraries(Engine
  PUBLIC
    spdlog::spdlog
    fmt::fmt
    nlohmann_json::nlohmann_json
    glm::glm
    Vulkan::Vulkan
    glfw
    CLI11::CLI11
    glad
  PRIVATE
)

# 6. stb_image include path, now header exists
# (no pkg-config dependency needed)
target_include_directories(Engine PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Interstellar/Graphics/OpenGL/stb
)
