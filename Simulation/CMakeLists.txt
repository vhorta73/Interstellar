cmake_minimum_required(VERSION 3.21)

# Inherit root version; fallback for standalone builds
if(NOT DEFINED Interstellar_VERSION)
  set(Interstellar_VERSION 0.0.0)
endif()

project(SimulationProj VERSION ${Interstellar_VERSION} LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(EnsureSources)  # <- DRY helper

# ---------------- Sources ----------------
file(GLOB_RECURSE SIMULATION_SOURCES CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)
# If empty, inject an empty TU
interstellar_ensure_sources(SIMULATION_SOURCES simulation)

# ---------------- Library ----------------
add_library(Simulation STATIC ${SIMULATION_SOURCES})
add_library(Interstellar::Simulation ALIAS Simulation)
target_compile_features(Simulation PUBLIC cxx_std_20)

# Allow linking this static lib into shared libs on UNIX
if(UNIX)
  set_target_properties(Simulation PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

# Compiled spdlog (consumer must see this)
target_compile_definitions(Simulation PUBLIC SPDLOG_COMPILED_LIB)

# Public/Private includes
# Headers should live at: Simulation/include/Interstellar/Simulation/...
set(INTERSTELLAR_SIMULATION_INSTALL_INCLUDE_DIR "${CMAKE_INSTALL_INCLUDEDIR}/Interstellar")
target_include_directories(Simulation
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${INTERSTELLAR_SIMULATION_INSTALL_INCLUDE_DIR}>
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# ---------------- Warnings ----------------
option(SIMULATION_WARNINGS_AS_ERRORS "Treat warnings as errors for Simulation" OFF)
if(MSVC)
  target_compile_options(Simulation PRIVATE /permissive- /W4 $<$<BOOL:${SIMULATION_WARNINGS_AS_ERRORS}>:/WX>)
else()
  target_compile_options(Simulation PRIVATE -Wall -Wextra -Wpedantic $<$<BOOL:${SIMULATION_WARNINGS_AS_ERRORS}>:-Werror>)
endif()

# ---------------- Dependencies ----------------
include(FetchContent)

# fmt + spdlog (spdlog uses external fmt)
set(SPDLOG_BUILD_SHARED OFF CACHE BOOL "" FORCE)
set(SPDLOG_FMT_EXTERNAL ON  CACHE BOOL "" FORCE)
if(NOT TARGET fmt::fmt)
  FetchContent_Declare(fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG        9.1.0
    GIT_SHALLOW    TRUE
    GIT_PROGRESS   TRUE
  )
endif()
if(NOT TARGET spdlog::spdlog)
  FetchContent_Declare(spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG        v1.11.0
    GIT_SHALLOW    TRUE
    GIT_PROGRESS   TRUE
  )
endif()

# mp-units
set(MP_UNITS_BUILD_MODULES     OFF CACHE BOOL "" FORCE)
set(MP_UNITS_BUILD_TESTING     OFF CACHE BOOL "" FORCE)
set(MP_UNITS_BUILD_BENCHMARKS  OFF CACHE BOOL "" FORCE)
set(MP_UNITS_BUILD_EXAMPLES    OFF CACHE BOOL "" FORCE)
if(NOT TARGET mp-units::mp-units AND NOT TARGET mp-units::units)
  FetchContent_Declare(mp_units
    GIT_REPOSITORY https://github.com/mpusz/units.git
    GIT_TAG        v2.4.0
    GIT_SHALLOW    TRUE
    GIT_PROGRESS   TRUE
    SOURCE_SUBDIR  src
  )
endif()

# nlohmann/json (header-only)
if(NOT TARGET nlohmann_json::nlohmann_json)
  FetchContent_Declare(nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG        v3.11.2
    GIT_SHALLOW    TRUE
    GIT_PROGRESS   TRUE
  )
endif()

# glad (C OpenGL loader lib)
if(NOT TARGET glad)
  FetchContent_Declare(glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG        v0.1.36
    GIT_SHALLOW    TRUE
    GIT_PROGRESS   TRUE
  )
endif()

# glm (header-only)
if(NOT TARGET glm::glm)
  FetchContent_Declare(glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        0.9.9.8
    GIT_SHALLOW    TRUE
    GIT_PROGRESS   TRUE
  )
endif()

# glfw
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)
if(NOT TARGET glfw)
  FetchContent_Declare(glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.3.8
    GIT_SHALLOW    TRUE
    GIT_PROGRESS   TRUE
  )
endif()

# CLI11
if(NOT TARGET CLI11::CLI11)
  FetchContent_Declare(CLI11
    GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
    GIT_TAG        v2.3.1
    GIT_SHALLOW    TRUE
    GIT_PROGRESS   TRUE
  )
endif()

# Make available any not already present
FetchContent_MakeAvailable(fmt spdlog mp_units nlohmann_json glad glm glfw CLI11)

# Linkage (prefer canonical mp-units name)
set(_mpu_target "")
foreach(_cand mp-units::mp-units mp-units::units)
  if(TARGET ${_cand})
    set(_mpu_target ${_cand})
    break()
  endif()
endforeach()
if(NOT _mpu_target)
  message(FATAL_ERROR "mp-units target not found (expected mp-units::mp-units or mp-units::units)")
endif()

target_link_libraries(Simulation
  PUBLIC
    Interstellar::Logging
    ${_mpu_target}
    fmt::fmt
    spdlog::spdlog
    nlohmann_json::nlohmann_json
    glm::glm
    CLI11::CLI11
  PRIVATE
    glfw
    glad
)

# ---------------- Install ----------------
option(SIMULATION_ENABLE_PACKAGE "Install CMake package for Interstellar::Simulation (requires system packages for deps)" OFF)

install(TARGETS Simulation
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# Install public headers under include/Interstellar
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/"
        DESTINATION "${INTERSTELLAR_SIMULATION_INSTALL_INCLUDE_DIR}"
        FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp" PATTERN "*.hh" PATTERN "*.hxx")

if(SIMULATION_ENABLE_PACKAGE)
  install(EXPORT SimulationTargets
    NAMESPACE Interstellar::
    FILE SimulationTargets.cmake
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/InterstellarSimulation"
  )

  write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/InterstellarSimulationConfigVersion.cmake"
    VERSION "${PROJECT_VERSION}"
    COMPATIBILITY SameMajorVersion
  )

  file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/InterstellarSimulationConfig.cmake" [=[
include(CMakeFindDependencyMacro)
include("${CMAKE_CURRENT_LIST_DIR}/SimulationTargets.cmake")
]=])

  install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/InterstellarSimulationConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/InterstellarSimulationConfigVersion.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/InterstellarSimulation"
  )
endif()

# ---------------- Tests ----------------
option(SIMULATION_BUILD_TESTS "Build Simulation tests" ${BUILD_TESTING})
if(SIMULATION_BUILD_TESTS AND BUILD_TESTING)
  include(SubprojectTesting)
  add_subproject_tests(TARGET Simulation NAME Simulation)
endif()
