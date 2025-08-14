cmake_minimum_required(VERSION 3.21)

# Inherit root version; fallback for standalone
if(NOT DEFINED Interstellar_VERSION)
  set(Interstellar_VERSION 0.0.0)
endif()

project(InterstellarLogging VERSION ${Interstellar_VERSION} LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# ---------------- Options ----------------
# OFF (default) = dev/monorepo: install binaries+headers only (no export)
# ON            = distribution: export CMake package and require system spdlog
option(LOGGING_ENABLE_PACKAGE "Install CMake package for Interstellar::Logging" OFF)

# ---------------- Dependencies ----------------
if(LOGGING_ENABLE_PACKAGE)
  # Package mode: do NOT rely on FetchContent spdlog; require an installed package.
  find_package(spdlog CONFIG REQUIRED)  # provides spdlog::spdlog
else()
  # Dev mode: we assume root already did FetchContent for spdlog/fmt.
  if(NOT TARGET spdlog::spdlog)
    message(FATAL_ERROR "Expected spdlog::spdlog to be available from the root CMake.")
  endif()
endif()

# ---------------- Sources ----------------
# Small helper to ensure a TU exists even if sources are temporarily empty.
# If you already have cmake/EnsureSources.cmake in the root, you can include it instead.
function(_ensure_sources var subname)
  if("${${var}}" STREQUAL "")
    set(_ph "${CMAKE_CURRENT_BINARY_DIR}/${subname}_placeholder.cpp")
    file(WRITE "${_ph}" "namespace{static void interstellar_${subname}_placeholder_0001(){} }")
    set(${var} "${_ph}" PARENT_SCOPE)
    message(STATUS "[EnsureSources] Injected placeholder: ${_ph}")
  endif()
endfunction()

file(GLOB_RECURSE LOGGING_SOURCES CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cxx"
)
_ensure_sources(LOGGING_SOURCES logging)

# ---------------- Target ----------------
add_library(InterstellarLogging STATIC ${LOGGING_SOURCES})
add_library(Interstellar::Logging ALIAS InterstellarLogging)

target_compile_features(InterstellarLogging PUBLIC cxx_std_20)

# Allow linking this static lib into shared libs on UNIX
if(UNIX)
  set_target_properties(InterstellarLogging PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

# Public headers under include/Interstellar
set(INTERSTELLAR_LOGGING_INSTALL_INCLUDE_DIR "${CMAKE_INSTALL_INCLUDEDIR}/Interstellar")

target_include_directories(InterstellarLogging
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${INTERSTELLAR_LOGGING_INSTALL_INCLUDE_DIR}>
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Link spdlog publicly so consumers of Logging get it transitively
target_link_libraries(InterstellarLogging PUBLIC spdlog::spdlog)

# Warnings
if(MSVC)
  target_compile_options(InterstellarLogging PRIVATE /permissive- /W4)
else()
  target_compile_options(InterstellarLogging PRIVATE -Wall -Wextra -Wpedantic)
endif()

# ---------------- Install ----------------
# Always install the built binary + headers (useful in dev too)
install(TARGETS InterstellarLogging
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/"
        DESTINATION "${INTERSTELLAR_LOGGING_INSTALL_INCLUDE_DIR}"
        FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp" PATTERN "*.hh" PATTERN "*.hxx")

# Only export a CMake package when explicitly enabled (distribution use case)
if(LOGGING_ENABLE_PACKAGE)
  install(EXPORT InterstellarLoggingTargets
    NAMESPACE Interstellar::
    FILE InterstellarLoggingTargets.cmake
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/InterstellarLogging"
  )

  write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/InterstellarLoggingConfigVersion.cmake"
    VERSION "${PROJECT_VERSION}"
    COMPATIBILITY SameMajorVersion
  )

  # In package mode, rely on system packages
  file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/InterstellarLoggingConfig.cmake" [=[
include(CMakeFindDependencyMacro)
find_dependency(spdlog CONFIG REQUIRED)
include("${CMAKE_CURRENT_LIST_DIR}/InterstellarLoggingTargets.cmake")
]=])

  install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/InterstellarLoggingConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/InterstellarLoggingConfigVersion.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/InterstellarLogging"
  )
endif()

# ---------------- Tests (via helper) ----------------
option(LOGGING_BUILD_TESTS "Build Logging tests" ${BUILD_TESTING})
if(LOGGING_BUILD_TESTS AND BUILD_TESTING)
  list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
  include(SubprojectTesting)
  add_subproject_tests(TARGET InterstellarLogging NAME Logging)
endif()
