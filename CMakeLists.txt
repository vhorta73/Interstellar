cmake_minimum_required(VERSION 3.21)
project(Interstellar VERSION 1.0 LANGUAGES CXX)

# ============================================================
# Turbo build options (root-only, on by default)
# ============================================================
option(ENABLE_SCCACHE          "Use sccache compiler cache"                     ON)
option(ENABLE_UNITY            "Unity/Jumbo builds (CMAKE_UNITY_BUILD)"         ON)
option(ENABLE_PCH              "Enable precompiled headers where available"     ON)
option(ENABLE_FASTLINK_DEBUG   "MSVC /DEBUG:FASTLINK in Debug"                  ON)
option(MSVC_EXTERNAL_SILENCE   "Silence MSVC warnings from external headers"    ON)
option(DISABLE_GLAD_REGEN      "Prevent GLAD from regenerating each configure"  ON)

# -- sccache (compiler cache) --
if(ENABLE_SCCACHE)
  find_program(SCCACHE_PROGRAM sccache)
  if(SCCACHE_PROGRAM)
    message(STATUS "Using sccache: ${SCCACHE_PROGRAM}")
    set(CMAKE_CXX_COMPILER_LAUNCHER "${SCCACHE_PROGRAM}")
    set(CMAKE_C_COMPILER_LAUNCHER  "${SCCACHE_PROGRAM}")
  else()
    message(STATUS "sccache not found; disable ENABLE_SCCACHE or install sccache")
  endif()
endif()

# -- Unity/Jumbo builds (global default) --
if(ENABLE_UNITY)
  set(CMAKE_UNITY_BUILD ON)
  set(CMAKE_UNITY_BUILD_BATCH_SIZE 8) # tune if needed
endif()

# -- Fast MSVC debug linking --
if(MSVC AND ENABLE_FASTLINK_DEBUG)
  add_link_options("$<$<CONFIG:Debug>:/DEBUG:FASTLINK;/INCREMENTAL>")
endif()

# -- Silence external headers on MSVC --
if(MSVC AND MSVC_EXTERNAL_SILENCE)
  add_compile_options(
    /experimental:external
    /external:W0
    /external:anglebrackets
  )
endif()

# -- Stop GLAD from regenerating (applies before Engine/FetchContent) --
if(DISABLE_GLAD_REGEN)
  set(GLAD_INSTALL    OFF CACHE BOOL   "" FORCE)
  set(GLAD_REGENERATE OFF CACHE BOOL   "" FORCE)
  set(GLAD_GENERATOR  c   CACHE STRING "" FORCE)
  set(GLAD_API        "gl=4.6" CACHE STRING "" FORCE)
  set(GLAD_PROFILE    "core"  CACHE STRING "" FORCE)
endif()

# ============================================================
# Root build options (your existing ones)
# ============================================================
option(BUILD_TESTING "Enable building of tests" ON)
option(BUILD_DOCS "Build API documentation with Doxygen" OFF)
option(INSTALL_TESTS "Install test executables and a CTest file into the install tree" ON)
option(SILENCE_THIRD_PARTY_WARNINGS "Silence deprecation/developer warnings from third-party projects" ON)
option(WARNINGS_AS_ERRORS "Treat warnings as errors (recommended in CI)" OFF)

# ============================================================
# Tools
# ============================================================
add_executable(utf8_check EXCLUDE_FROM_ALL tools/utf8_check/main.cpp)
target_compile_features(utf8_check PRIVATE cxx_std_17)

add_custom_target(check-encoding
  COMMAND $<TARGET_FILE:utf8_check> "${CMAKE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}"
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
  COMMENT "Checking file encoding (UTF-8 without BOM)"
  DEPENDS utf8_check
)

# ============================================================
# Toolchain niceties
# ============================================================
if(SILENCE_THIRD_PARTY_WARNINGS)
  set(CMAKE_WARN_DEPRECATED OFF CACHE BOOL "" FORCE)
  set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS 1 CACHE INTERNAL "")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(UNIX)
  set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO ON)

if(MSVC)
  add_compile_options(/utf-8)
  add_compile_definitions(_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING)
  if(NOT DEFINED CMAKE_MSVC_RUNTIME_LIBRARY)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
  endif()
  add_compile_options("$<$<CONFIG:Release>:/O2>" "$<$<CONFIG:Release>:/Oi>" "$<$<CONFIG:Release>:/GL>")
  add_link_options("$<$<CONFIG:Release>:/LTCG>")
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<$<CONFIG:Release>:ProgramDatabase>")
endif()

# Default install prefix
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/package" CACHE PATH "Install prefix" FORCE)
endif()

# ============================================================
# Third-party (via FetchContent)
# ============================================================
include(FetchContent)

# gsl-lite headers via minimal interface
FetchContent_Declare(
  gsl_lite
  GIT_REPOSITORY https://github.com/gsl-lite/gsl-lite.git
  GIT_TAG        v1.0.1
  GIT_SHALLOW    TRUE
  GIT_PROGRESS   TRUE
)
FetchContent_GetProperties(gsl_lite)
if(NOT gsl_lite_POPULATED)
  FetchContent_Populate(gsl_lite)
  add_library(gsl_lite_headers INTERFACE)
  target_include_directories(gsl_lite_headers INTERFACE "${gsl_lite_SOURCE_DIR}/include")
  target_compile_definitions(gsl_lite_headers INTERFACE gsl_FEATURE_GSL_COMPATIBILITY_MODE=1)
  add_library(gsl::gsl-lite ALIAS gsl_lite_headers)
endif()

# mp-units (trim extras; valid contracts: NONE;GSL-LITE;MS-GSL)
FetchContent_Declare(
  mp_units
  GIT_REPOSITORY https://github.com/mpusz/units.git
  GIT_TAG        v2.4.0
  GIT_SHALLOW    TRUE
  GIT_PROGRESS   TRUE
  SOURCE_SUBDIR  src
)
set(MP_UNITS_BUILD_MODULES     OFF  CACHE BOOL   "" FORCE)
set(MP_UNITS_BUILD_TESTING     OFF  CACHE BOOL   "" FORCE)
set(MP_UNITS_BUILD_BENCHMARKS  OFF  CACHE BOOL   "" FORCE)
set(MP_UNITS_BUILD_EXAMPLES    OFF  CACHE BOOL   "" FORCE)
set(MP_UNITS_API_CONTRACTS     NONE CACHE STRING "" FORCE)

# fmt + spdlog moved to ROOT so Logging can link spdlog::spdlog
set(SPDLOG_BUILD_SHARED OFF CACHE BOOL "" FORCE)
set(SPDLOG_FMT_EXTERNAL ON  CACHE BOOL "" FORCE)
FetchContent_Declare(fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG        9.1.0
  GIT_SHALLOW    TRUE
  GIT_PROGRESS   TRUE
)
FetchContent_Declare(spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG        v1.11.0
  GIT_SHALLOW    TRUE
  GIT_PROGRESS   TRUE
)

# Make the third-parties available BEFORE subprojects
FetchContent_MakeAvailable(fmt spdlog mp_units)

# ============================================================
# Portable install dirs / rpaths
# ============================================================
include(GNUInstallDirs)
set(CMAKE_BUILD_WITH_INSTALL_RPATH OFF)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH OFF)
if(APPLE)
  set(CMAKE_INSTALL_RPATH "@loader_path;@loader_path/../${CMAKE_INSTALL_LIBDIR}")
elseif(UNIX)
  set(CMAKE_INSTALL_RPATH "$ORIGIN;$ORIGIN/../${CMAKE_INSTALL_LIBDIR}")
endif()

# ============================================================
# Testing (global)
# ============================================================
include(CTest)
if(BUILD_TESTING)
  FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.14.0
    GIT_SHALLOW    TRUE
    GIT_PROGRESS   TRUE
  )
  if(MSVC)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  endif()
  set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(googletest)
endif()

# ============================================================
# Make helper modules discoverable
# ============================================================
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(EnsureSources)        # shared empty-TU helper

# ============================================================
# Subprojects
# ============================================================
set(UNITS_BUILD_TESTS        ${BUILD_TESTING} CACHE BOOL "" FORCE)
set(UTILS_BUILD_TESTS        ${BUILD_TESTING} CACHE BOOL "" FORCE)
set(ENGINE_BUILD_TESTS       ${BUILD_TESTING} CACHE BOOL "" FORCE)
set(ECS_BUILD_TESTS          ${BUILD_TESTING} CACHE BOOL "" FORCE)
set(IO_BUILD_TESTS           ${BUILD_TESTING} CACHE BOOL "" FORCE)
set(DATA_BUILD_TESTS         ${BUILD_TESTING} CACHE BOOL "" FORCE)
set(INTERSTELLAR_BUILD_TESTS ${BUILD_TESTING} CACHE BOOL "" FORCE)
set(LOGGING_BUILD_TESTS      ${BUILD_TESTING} CACHE BOOL "" FORCE)
set(INPUT_BUILD_TESTS        ${BUILD_TESTING} CACHE BOOL "" FORCE)
set(UNIVERSE_BUILD_TESTS     ${BUILD_TESTING} CACHE BOOL "" FORCE)

add_subdirectory(Logging)
add_subdirectory(Units)
add_subdirectory(Utils)
add_subdirectory(Universe)
add_subdirectory(Procedural)
add_subdirectory(Engine)
add_subdirectory(ECS)
add_subdirectory(Simulation)
add_subdirectory(Interstellar)
add_subdirectory(IO)
add_subdirectory(Input)
add_subdirectory(Data)
add_subdirectory(Graphics)
add_subdirectory(Renderers)

# Run encoding check before key targets
add_dependencies(Engine       check-encoding)
add_dependencies(Interstellar check-encoding)

# ============================================================
# Post-subdirs: apply PCH (if src/pch.hpp exists) & per-target unity tweaks
# ============================================================
function(_interstellar_try_pch target subdir)
  if(NOT ENABLE_PCH)
    return()
  endif()
  if(NOT TARGET ${target})
    return()
  endif()
  set(_pch "${CMAKE_SOURCE_DIR}/${subdir}/src/pch.hpp")
  if(EXISTS "${_pch}")
    message(STATUS "[PCH] ${target} -> ${_pch}")
    target_precompile_headers(${target} PRIVATE
      "$<$<COMPILE_LANGUAGE:CXX>:${_pch}>"
    )
  endif()
endfunction()

function(_interstellar_try_unity_batch target)
  if(ENABLE_UNITY AND TARGET ${target})
    # set_property(TARGET ${target} PROPERTY UNITY_BUILD_BATCH_SIZE 8) # optional per-target
  endif()
endfunction()

foreach(_pair
  "Engine;Engine"
  "ECS;ECS"
  "Procedural;Procedural"
  "Simulation;Simulation"
  "Interstellar;Interstellar"
  "InterstellarUtils;Utils"
  "InterstellarIO;IO"
  "InterstellarInput;Input"
  "InterstellarData;Data"
  "InterstellarLogging;Logging"
  "InterstellarGraphics;Graphics"
  "InterstellarRenderers;Renderers"
)
  list(GET _pair 0 _target)
  list(GET _pair 1 _subdir)
  _interstellar_try_pch(${_target} ${_subdir})
  _interstellar_try_unity_batch(${_target})
endforeach()

# ============================================================
# Packaging
# ============================================================
include(InstallRequiredSystemLibraries)
set(CPACK_PACKAGE_NAME "Interstellar")
set(CPACK_PACKAGE_VENDOR "Imagine")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_GENERATOR "ZIP")
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
include(CPack)

# ============================================================
# Installed-tests index (kept)
# ============================================================
if(INSTALL_TESTS)
  install(CODE [[
    file(MAKE_DIRECTORY "${CMAKE_INSTALL_PREFIX}/tests")
    file(WRITE "${CMAKE_INSTALL_PREFIX}/tests/CTestTestfile.cmake" "")
    function(_add_installed_test name)
      set(_exe "${CMAKE_INSTALL_PREFIX}/tests/bin/${name}$<$<PLATFORM_ID:Windows>:.exe>")
      set(_wd  "${CMAKE_INSTALL_PREFIX}/bin")
      file(APPEND "${CMAKE_INSTALL_PREFIX}/tests/CTestTestfile.cmake"
        "add_test(NAME Installed.${name} COMMAND \"${_exe}\" --gtest_color=yes)\n"
        "set_tests_properties(Installed.${name} PROPERTIES WORKING_DIRECTORY \"${_wd}\")\n"
      )
    endfunction()
    _add_installed_test("EngineTests")
  ]])
endif()
