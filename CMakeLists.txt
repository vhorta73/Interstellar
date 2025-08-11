cmake_minimum_required(VERSION 3.21)
project(Interstellar VERSION 1.0 LANGUAGES CXX)

# ---------- Options ----------
option(BUILD_TESTING "Enable building of tests" ON)
option(BUILD_DOCS "Build API documentation with Doxygen" OFF)
option(INSTALL_TESTS "Install test executables and a CTest file into the install tree" ON)
option(SILENCE_THIRD_PARTY_WARNINGS "Silence deprecation/developer warnings from third-party projects" ON)
option(WARNINGS_AS_ERRORS "Treat warnings as errors (recommended in CI)" OFF)

# ---------- Encoding-checker tool ----------
add_executable(utf8_check tools/utf8_check/main.cpp)
target_compile_features(utf8_check PRIVATE cxx_std_17) # filesystem
add_compile_definitions(gsl_FEATURE_GSL_COMPATIBILITY_MODE=1)


add_custom_target(check-encoding
  COMMAND utf8_check "${CMAKE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}"
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
  COMMENT "Checking file encoding (UTF-8 without BOM)"
)

# ---------- Global toolchain niceties ----------
if(SILENCE_THIRD_PARTY_WARNINGS)
  set(CMAKE_WARN_DEPRECATED OFF CACHE BOOL "" FORCE)
  set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS 1 CACHE INTERNAL "")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON) # LTO in Release

if(MSVC)
  add_compile_options(/utf-8)
  add_compile_definitions(_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING)
  if(NOT DEFINED CMAKE_MSVC_RUNTIME_LIBRARY)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
  endif()
  add_compile_options("$<$<CONFIG:Release>:/O2;/Oi;/Ot;/GL>")
  add_link_options("$<$<CONFIG:Release>:/LTCG>")
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<$<CONFIG:Release>:ProgramDatabase>")
endif()

# Default install prefix (presets override)
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/package" CACHE PATH "Install prefix" FORCE)
endif()

# ---------- Third-party via FetchContent ----------
include(FetchContent)

# gsl-lite shim
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
  file(WRITE "${gsl_lite_BINARY_DIR}/gsl-lite-config.cmake" [=[
    if(NOT TARGET gsl::gsl-lite)
      add_library(gsl::gsl-lite INTERFACE IMPORTED)
      set_target_properties(gsl::gsl-lite PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "@GSL_LITE_INCLUDE_DIR@")
    endif()
  ]=])
  set(GSL_LITE_INCLUDE_DIR "${gsl_lite_SOURCE_DIR}/include")
  file(READ "${gsl_lite_BINARY_DIR}/gsl-lite-config.cmake" _cfg)
  string(REPLACE "@GSL_LITE_INCLUDE_DIR@" "${GSL_LITE_INCLUDE_DIR}" _cfg "${_cfg}")
  file(WRITE "${gsl_lite_BINARY_DIR}/gsl-lite-config.cmake" "${_cfg}")
  set(gsl-lite_DIR "${gsl_lite_BINARY_DIR}" CACHE PATH "" FORCE)
endif()

# mp-units
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
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH ON)
if(APPLE)
  set(CMAKE_INSTALL_RPATH "@loader_path;@loader_path/../${CMAKE_INSTALL_LIBDIR}")
elseif(UNIX)
  set(CMAKE_INSTALL_RPATH "$ORIGIN;$ORIGIN/../${CMAKE_INSTALL_LIBDIR}")
endif()

# ---------- Tests ----------
include(CTest)
if(BUILD_TESTING)
  # GoogleTest
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
  FetchContent_MakeAvailable(googletest)

  # Add your tests tree (expects tests/CMakeLists.txt to add EngineTests etc.)
  add_subdirectory(tests)
endif()

# ---------- Subprojects ----------
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Utils/CMakeLists.txt")
  add_subdirectory(Utils)
endif()
add_subdirectory(Engine)
add_subdirectory(Interstellar)
add_subdirectory(IO)
add_subdirectory(Units)

# If Engine or Interstellar app needs IO:
# target_link_libraries(Engine PRIVATE Interstellar::IO)
# Engine needs Units headers => link it:
target_link_libraries(Engine PRIVATE Interstellar::Units)

# Run encoding check before key targets
add_dependencies(Engine        check-encoding)
add_dependencies(Interstellar  check-encoding)
if (TARGET EngineTests)
  add_dependencies(EngineTests check-encoding)
endif()

# ---------- Docs (optional) ----------
if(BUILD_DOCS)
  find_package(Doxygen REQUIRED)
  set(DOXYGEN_IN "${CMAKE_SOURCE_DIR}/docs/Doxyfile")
  if(NOT EXISTS "${DOXYGEN_IN}")
    message(FATAL_ERROR "Doxygen config not found: ${DOXYGEN_IN}")
  endif()
  add_custom_target(Documentation
    COMMENT "Generating API docs with Doxygen"
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    COMMAND "${DOXYGEN_EXECUTABLE}" "${DOXYGEN_IN}"
  )
endif()

# ---------- Packaging ----------
include(InstallRequiredSystemLibraries)
set(CPACK_PACKAGE_NAME "Interstellar")
set(CPACK_PACKAGE_VENDOR "Imagine")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_GENERATOR "ZIP")
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
include(CPack)

# ---------- Installed-tests index ----------
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
    # _add_installed_test("AppTests")
  ]])
endif()
