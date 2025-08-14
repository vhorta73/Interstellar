cmake_minimum_required(VERSION 3.21)
project(Interstellar VERSION 1.0 LANGUAGES CXX)

# ---------- Options ----------
option(BUILD_TESTING                 "Enable building of tests" ON)
option(BUILD_DOCS                    "Build API documentation with Doxygen" OFF)
option(INSTALL_TESTS                 "Install test executables and a CTest file into the install tree" ON)
option(SILENCE_THIRD_PARTY_WARNINGS  "Silence deprecation/developer warnings from third-party projects" ON)
option(WARNINGS_AS_ERRORS            "Treat warnings as errors (recommended in CI)" OFF)

# ---------- Helper modules path ----------
# Make cmake/ modules discoverable by subprojects (e.g., SubprojectTesting.cmake)
list(PREPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# Bootstrap a safe stub if SubprojectTesting.cmake is missing
if(NOT EXISTS "${CMAKE_SOURCE_DIR}/cmake/SubprojectTesting.cmake")
  file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/cmake")
  set(_stub "${CMAKE_BINARY_DIR}/cmake/SubprojectTesting.cmake")
  file(WRITE "${_stub}" [=[
# Auto-generated fallback stub. Replace with your real cmake/SubprojectTesting.cmake when available.
if(NOT COMMAND add_subproject_tests)
  function(add_subproject_tests target_name)
    if(BUILD_TESTING AND TARGET ${target_name})
      # No-op placeholder. Real implementation can add tests or wire CTest here.
      # Example idea:
      # add_test(NAME ${target_name}.selftest COMMAND $<TARGET_FILE:${target_name}> --gtest_color=yes)
    endif()
  endfunction()
endif()
]=])
  list(PREPEND CMAKE_MODULE_PATH "${CMAKE_BINARY_DIR}/cmake")
endif()

# ---------- Tools ----------
add_executable(utf8_check EXCLUDE_FROM_ALL tools/utf8_check/main.cpp)
target_compile_features(utf8_check PRIVATE cxx_std_17)

add_custom_target(check-encoding
  COMMAND $<TARGET_FILE:utf8_check> "${CMAKE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}"
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
  COMMENT "Checking file encoding (UTF-8 without BOM)"
  DEPENDS utf8_check
)

# ---------- Global toolchain niceties ----------
if(SILENCE_THIRD_PARTY_WARNINGS)
  set(CMAKE_WARN_DEPRECATED OFF CACHE BOOL "" FORCE)
  set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS 1 CACHE INTERNAL "")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(UNIX)
  set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

# Enable IPO/LTO for Release and RelWithDebInfo (keep off in Debug)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE        ON)
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

# Default install prefix (presets can override)
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/package" CACHE PATH "Install prefix" FORCE)
endif()

# ---------- Third-party via FetchContent ----------
include(FetchContent)

# ---------- gsl-lite as IMPORTED target ----------
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
  # Create an IMPORTED INTERFACE target so that downstream exports (mp-units)
  # don't try to re-export it.
  add_library(gsl::gsl-lite INTERFACE IMPORTED)
  set_target_properties(gsl::gsl-lite PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${gsl_lite_SOURCE_DIR}/include"
    INTERFACE_COMPILE_DEFINITIONS "gsl_FEATURE_GSL_COMPATIBILITY_MODE=1"
  )
endif()

# mp-units (header-only, with extras disabled)
FetchContent_Declare(
  mp_units
  GIT_REPOSITORY https://github.com/mpusz/units.git
  GIT_TAG        v2.4.0
  GIT_SHALLOW    TRUE
  GIT_PROGRESS   TRUE
  SOURCE_SUBDIR  src
)
set(MP_UNITS_BUILD_MODULES     OFF CACHE BOOL "" FORCE)
set(MP_UNITS_BUILD_TESTING     OFF CACHE BOOL "" FORCE)
set(MP_UNITS_BUILD_BENCHMARKS  OFF CACHE BOOL "" FORCE)
set(MP_UNITS_BUILD_EXAMPLES    OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(mp_units)

# ---------- Portable install dirs / rpaths ----------
include(GNUInstallDirs)
set(CMAKE_BUILD_WITH_INSTALL_RPATH OFF)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH OFF)
if(APPLE)
  set(CMAKE_INSTALL_RPATH "@loader_path;@loader_path/../${CMAKE_INSTALL_LIBDIR}")
elseif(UNIX)
  set(CMAKE_INSTALL_RPATH "$ORIGIN;$ORIGIN/../${CMAKE_INSTALL_LIBDIR}")
endif()

# ---------- Tests (global) ----------
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

# ---------- Subprojects ----------
# (comment out lines for folders you don't have yet)
add_subdirectory(Units)
add_subdirectory(Utils)
add_subdirectory(Engine)
add_subdirectory(Interstellar)
add_subdirectory(IO)
add_subdirectory(Data)

# Ensure encoding check runs before key targets (only if they actually exist)
foreach(t IN ITEMS Engine Interstellar)
  if(TARGET ${t})
    add_dependencies(${t} check-encoding)
  else()
    message(STATUS "Target '${t}' not found; skipping encoding dependency for it.")
  endif()
endforeach()

# ---------- Packaging ----------
include(InstallRequiredSystemLibraries)
set(CPACK_PACKAGE_NAME    "Interstellar")
set(CPACK_PACKAGE_VENDOR  "Imagine")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_GENERATOR "ZIP")
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
include(CPack)

# ---------- Installed-tests index ----------
if(INSTALL_TESTS)
  install(CODE [[
    set(prefix "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}")
    file(MAKE_DIRECTORY "${prefix}/tests")
    set(ctestfile "${prefix}/tests/CTestTestfile.cmake")
    file(WRITE "${ctestfile}" "")
    if(WIN32)
      set(_exe_ext ".exe")
    else()
      set(_exe_ext "")
    endif()
    function(_add_installed_test name)
      set(_exe "${CMAKE_INSTALL_PREFIX}/tests/bin/${name}${_exe_ext}")
      set(_wd  "${CMAKE_INSTALL_PREFIX}/bin")
      file(APPEND "${ctestfile}"
        "add_test(NAME Installed.${name} COMMAND \"${_exe}\" --gtest_color=yes)\n"
        "set_tests_properties(Installed.${name} PROPERTIES WORKING_DIRECTORY \"${_wd}\")\n"
      )
    endfunction()
    _add_installed_test("EngineTests")
  ]])
endif()
