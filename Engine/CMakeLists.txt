# Engine/CMakeLists.txt

# Sources
file(GLOB_RECURSE ENGINE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
add_library(Engine STATIC ${ENGINE_SOURCES})

# If spdlog compiled lib is desired
target_compile_definitions(Engine PUBLIC SPDLOG_COMPILED_LIB)

# Includes
target_include_directories(Engine
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Deps
include(FetchContent)
FetchContent_Declare(spdlog          GIT_REPOSITORY https://github.com/gabime/spdlog.git      GIT_TAG v1.11.0)
FetchContent_Declare(fmt             GIT_REPOSITORY https://github.com/fmtlib/fmt.git         GIT_TAG 10.1.1)
FetchContent_Declare(nlohmann_json   GIT_REPOSITORY https://github.com/nlohmann/json.git      GIT_TAG v3.11.2)
FetchContent_Declare(glad            GIT_REPOSITORY https://github.com/Dav1dde/glad.git       GIT_TAG v0.1.36)
FetchContent_Declare(glm             GIT_REPOSITORY https://github.com/g-truc/glm.git         GIT_TAG 0.9.9.8)
FetchContent_Declare(glfw            GIT_REPOSITORY https://github.com/glfw/glfw.git          GIT_TAG 3.3.8)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)
FetchContent_Declare(CLI11           GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git     GIT_TAG v2.3.1)

FetchContent_MakeAvailable(spdlog fmt nlohmann_json glad glm glfw CLI11)

find_package(Vulkan REQUIRED)

# Keep everything PRIVATE unless your public headers require them
target_link_libraries(Engine
  PUBLIC
    spdlog::spdlog
    Vulkan::Vulkan
    nlohmann_json::nlohmann_json
    glm::glm
    CLI11::CLI11
  PRIVATE
    fmt::fmt
    glfw
    glad
)

# (Remove if the path doesn't exist)
target_include_directories(Engine PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Interstellar/Graphics/OpenGL/stb
)

# ---------- Install just the library + headers (no export) ----------
install(TARGETS Engine
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/" DESTINATION include)

# (Intentionally no install(EXPORT ...) or package config here)
