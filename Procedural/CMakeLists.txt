cmake_minimum_required(VERSION 3.21)

# Inherit root version; fallback for standalone builds
if(NOT DEFINED Interstellar_VERSION)
  set(Interstellar_VERSION 0.0.0)
endif()

project(ProceduralProj VERSION ${Interstellar_VERSION} LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(EnsureSources)  # shared empty-TU helper

# ---------------- Sources ----------------
file(GLOB_RECURSE PROCEDURAL_SOURCES CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)
# If empty, inject an empty TU so the target shows up in IDEs
interstellar_ensure_sources(PROCEDURAL_SOURCES procedural)

# ---------------- Library ----------------
add_library(Procedural STATIC ${PROCEDURAL_SOURCES})
add_library(Interstellar::Procedural ALIAS Procedural)
target_compile_features(Procedural PUBLIC cxx_std_20)

# Allow linking this static lib into shared libs on UNIX
if(UNIX)
  set_target_properties(Procedural PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

# Compiled spdlog (consumer must see this)
target_compile_definitions(Procedural PUBLIC SPDLOG_COMPILED_LIB)

# Public/Private includes
# Put headers under: Procedural/include/Interstellar/Procedural/...
set(INTERSTELLAR_PROCEDURAL_INSTALL_INCLUDE_DIR "${CMAKE_INSTALL_INCLUDEDIR}/Interstellar")
target_include_directories(Procedural
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${INTERSTELLAR_PROCEDURAL_INSTALL_INCLUDE_DIR}>
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# ---------------- Warnings ----------------
option(PROCEDURAL_WARNINGS_AS_ERRORS "Treat warnings as errors for Procedural" OFF)
if(MSVC)
  target_compile_options(Procedural PRIVATE /permissive- /W4 $<$<BOOL:${PROCEDURAL_WARNINGS_AS_ERRORS}>:/WX>)
else()
  target_compile_options(Procedural PRIVATE -Wall -Wextra -Wpedantic $<$<BOOL:${PROCEDURAL_WARNINGS_AS_ERRORS}>:-Werror>)
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

target_link_libraries(Procedural
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
# If Procedural’s public headers include ECS or Simulation, also:
# target_link_libraries(Procedural PUBLIC Interstellar::ECS Interstellar::Simulation)

# ---------------- Install ----------------
option(PROCEDURAL_ENABLE_PACKAGE "Install CMake package for Interstellar::Procedural (requires system packages for deps)" OFF)

install(TARGETS Procedural
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# Install public headers under include/Interstellar
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/"
        DESTINATION "${INTERSTELLAR_PROCEDURAL_INSTALL_INCLUDE_DIR}"
        FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp" PATTERN "*.hh" PATTERN "*.hxx")

if(PROCEDURAL_ENABLE_PACKAGE)
  install(EXPORT ProceduralTargets
    NAMESPACE Interstellar::
    FILE ProceduralTargets.cmake
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/InterstellarProcedural"
  )

  write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/InterstellarProceduralConfigVersion.cmake"
    VERSION "${PROJECT_VERSION}"
    COMPATIBILITY SameMajorVersion
  )

  file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/InterstellarProceduralConfig.cmake" [=[
include(CMakeFindDependencyMacro)
find_dependency(fmt)
find_dependency(spdlog)
find_dependency(nlohmann_json)
find_dependency(glm)
find_dependency(CLI11)
find_package(mp-units CONFIG REQUIRED)
include("${CMAKE_CURRENT_LIST_DIR}/ProceduralTargets.cmake")
]=])

  install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/InterstellarProceduralConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/InterstellarProceduralConfigVersion.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/InterstellarProcedural"
  )
endif()

# ---------------- Tests ----------------
option(PROCEDURAL_BUILD_TESTS "Build Procedural tests" ${BUILD_TESTING})
if(PROCEDURAL_BUILD_TESTS AND BUILD_TESTING)
  include(SubprojectTesting)
  add_subproject_tests(TARGET Procedural NAME Procedural)
endif()
