cmake_minimum_required(VERSION 3.21)

# Inherit the top-level project version if available (Interstellar VERSION 1.0)
# Fallback to 0.0.0 to keep configure robust even when built standalone.
if(NOT DEFINED Interstellar_VERSION)
  set(Interstellar_VERSION 0.0.0)
endif()

project(InterstellarUtils VERSION ${Interstellar_VERSION} LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# ---- Sources ----
file(GLOB_RECURSE UTILS_SOURCES CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cxx"
)

# Provide a harmless placeholder if the lib is temporarily empty
if(UTILS_SOURCES STREQUAL "")
  set(_placeholder "${CMAKE_CURRENT_BINARY_DIR}/utils_placeholder.cpp")
  file(WRITE "${_placeholder}" [=[
namespace { static void interstellar_utils_placeholder_12345() {} }
]=])
  list(APPEND UTILS_SOURCES "${_placeholder}")
endif()

# ---- Library ----
add_library(InterstellarUtils STATIC ${UTILS_SOURCES})
add_library(Interstellar::Utils ALIAS InterstellarUtils)

# Consumers must compile with C++20 if public headers use it
target_compile_features(InterstellarUtils PUBLIC cxx_std_20)

# Allow linking this static lib into shared libs on UNIX
if(UNIX)
  set_target_properties(InterstellarUtils PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

# Public headers live under include/Interstellar (recommended)
set(INTERSTELLAR_UTILS_INSTALL_INCLUDE_DIR "${CMAKE_INSTALL_INCLUDEDIR}/Interstellar")

target_include_directories(InterstellarUtils
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${INTERSTELLAR_UTILS_INSTALL_INCLUDE_DIR}>
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# ---- Warnings (local to this target) ----
if(MSVC)
  target_compile_options(InterstellarUtils PRIVATE /permissive- /W4)
  if(WARNINGS_AS_ERRORS)
    target_compile_options(InterstellarUtils PRIVATE /WX)
  endif()
else()
  target_compile_options(InterstellarUtils PRIVATE -Wall -Wextra -Wpedantic)
  if(WARNINGS_AS_ERRORS)
    target_compile_options(InterstellarUtils PRIVATE -Werror)
  endif()
endif()

# ---- Install (library + headers + export) ----
install(TARGETS InterstellarUtils
  EXPORT InterstellarUtilsTargets
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# Install public headers preserving folder structure
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/"
        DESTINATION "${INTERSTELLAR_UTILS_INSTALL_INCLUDE_DIR}"
        FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp" PATTERN "*.hh" PATTERN "*.hxx")

# Install CMake export file so find_package(...) can import the target
install(EXPORT InterstellarUtilsTargets
  NAMESPACE Interstellar::
  FILE InterstellarUtilsTargets.cmake
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/InterstellarUtils"
)

# ---- Package config (so find_package(InterstellarUtils) works) ----
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/InterstellarUtilsConfigVersion.cmake"
  VERSION "${PROJECT_VERSION}"
  COMPATIBILITY SameMajorVersion
)

file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/InterstellarUtilsConfig.cmake" [=[
# Minimal config; just include the exported targets.
include("${CMAKE_CURRENT_LIST_DIR}/InterstellarUtilsTargets.cmake")
]=])

install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/InterstellarUtilsConfig.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/InterstellarUtilsConfigVersion.cmake"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/InterstellarUtils"
)

# ---- Tests (via helper) ----
option(UTILS_BUILD_TESTS "Build Utils tests" ${BUILD_TESTING})
if(UTILS_BUILD_TESTS AND BUILD_TESTING)
  include(SubprojectTesting)  # provided by the root via CMAKE_MODULE_PATH
  if(COMMAND add_subproject_tests)
    # Your module requires explicit keywords:
    add_subproject_tests(TARGET InterstellarUtils NAME Utils)
  else()
    message(WARNING "SubprojectTesting module loaded but add_subproject_tests() is not defined; skipping Utils tests wiring.")
  endif()
endif()
