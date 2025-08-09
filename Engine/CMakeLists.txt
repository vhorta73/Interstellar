cmake_minimum_required(VERSION 3.21)

# ---- Sources ----
file(GLOB_RECURSE ENGINE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
add_library(Engine STATIC ${ENGINE_SOURCES})

# Compiled spdlog
target_compile_definitions(Engine PUBLIC SPDLOG_COMPILED_LIB)

# Public/Private includes
target_include_directories(Engine
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# ---- Warnings (your code only) ----
if(MSVC)
  target_compile_options(Engine PRIVATE /permissive- /W4)
else()
  target_compile_options(Engine PRIVATE -Wall -Wextra -Wpedantic)
endif()
if(WARNINGS_AS_ERRORS)
  if(MSVC)
    target_compile_options(Engine PRIVATE /WX)
  else()
    target_compile_options(Engine PRIVATE -Werror)
  endif()
endif()

# ---- Dependencies via FetchContent ----
include(FetchContent)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

# GLFW knobs
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)

# fmt v9.1.0 (public in headers) + spdlog that uses external fmt
FetchContent_Declare(fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG        9.1.0
  GIT_SHALLOW    TRUE
  GIT_PROGRESS   TRUE
)
FetchContent_GetProperties(fmt)
if(NOT fmt_POPULATED)
  FetchContent_Populate(fmt)
  add_subdirectory(${fmt_SOURCE_DIR} ${fmt_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()
set(fmt_DIR "${fmt_BINARY_DIR}" CACHE PATH "" FORCE)

set(SPDLOG_BUILD_SHARED OFF CACHE BOOL "" FORCE)
set(SPDLOG_FMT_EXTERNAL ON  CACHE BOOL "" FORCE)
FetchContent_Declare(spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG        v1.11.0
  GIT_SHALLOW    TRUE
  GIT_PROGRESS   TRUE
)

FetchContent_Declare(nlohmann_json GIT_REPOSITORY https://github.com/nlohmann/json.git  GIT_TAG v3.11.2 GIT_SHALLOW TRUE GIT_PROGRESS TRUE)
FetchContent_Declare(glad          GIT_REPOSITORY https://github.com/Dav1dde/glad.git   GIT_TAG v0.1.36  GIT_SHALLOW TRUE GIT_PROGRESS TRUE)
FetchContent_Declare(glm           GIT_REPOSITORY https://github.com/g-truc/glm.git     GIT_TAG 0.9.9.8  GIT_SHALLOW TRUE GIT_PROGRESS TRUE)
FetchContent_Declare(glfw          GIT_REPOSITORY https://github.com/glfw/glfw.git      GIT_TAG 3.3.8    GIT_SHALLOW TRUE GIT_PROGRESS TRUE)
FetchContent_Declare(CLI11         GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git GIT_TAG v2.3.1   GIT_SHALLOW TRUE GIT_PROGRESS TRUE)

FetchContent_MakeAvailable(spdlog nlohmann_json glad glm glfw CLI11)

# Vulkan
find_package(Vulkan REQUIRED)

# Linkage
target_link_libraries(Engine
  PUBLIC
    fmt::fmt
    spdlog::spdlog
    nlohmann_json::nlohmann_json
    glm::glm
    Vulkan::Vulkan
    CLI11::CLI11
  PRIVATE
    glfw
    glad
)

# (Remove if not present)
target_include_directories(Engine PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Interstellar/Graphics/OpenGL/stb
)

# ---- Install ----
include(GNUInstallDirs)
install(TARGETS Engine
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/"
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
