cmake_minimum_required(VERSION 3.21)

# Inherit root version; fallback for standalone
if(NOT DEFINED Interstellar_VERSION)
  set(Interstellar_VERSION 0.0.0)
endif()

project(InterstellarIOProj VERSION ${Interstellar_VERSION} LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# ---------------- Sources ----------------
file(GLOB_RECURSE IO_SOURCES CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cxx"
)

# ODR-safe placeholder if temporarily empty
if(IO_SOURCES STREQUAL "")
  set(_placeholder "${CMAKE_CURRENT_BINARY_DIR}/io_placeholder.cpp")
  file(WRITE "${_placeholder}" "namespace{static void interstellar_io_placeholder_24680(){} }")
  list(APPEND IO_SOURCES "${_placeholder}")
endif()

# ---------------- Target ----------------
add_library(InterstellarIO STATIC ${IO_SOURCES})
add_library(Interstellar::IO ALIAS InterstellarIO)
target_compile_features(InterstellarIO PUBLIC cxx_std_20)

# Allow linking this static lib into shared libs on UNIX
if(UNIX)
  set_target_properties(InterstellarIO PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

# Public/Private includes
set(INTERSTELLAR_IO_INSTALL_INCLUDE_DIR "${CMAKE_INSTALL_INCLUDEDIR}/Interstellar")
target_include_directories(InterstellarIO
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${INTERSTELLAR_IO_INSTALL_INCLUDE_DIR}>
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# ---------------- Warnings ----------------
option(IO_WARNINGS_AS_ERRORS "Treat warnings as errors for IO" OFF)
if(MSVC)
  target_compile_options(InterstellarIO PRIVATE /permissive- /W4 $<$<BOOL:${IO_WARNINGS_AS_ERRORS}>:/WX>)
else()
  target_compile_options(InterstellarIO PRIVATE -Wall -Wextra -Wpedantic $<$<BOOL:${IO_WARNINGS_AS_ERRORS}>:-Werror>)
endif()

# ---------------- Dependencies ----------------
option(IO_ENABLE_PACKAGE "Install CMake package for Interstellar::IO (requires system packages for deps)" OFF)

if(IO_ENABLE_PACKAGE)
  # Package mode: require imported packages (no build-tree deps in export)
  find_package(simdjson CONFIG REQUIRED) # provides simdjson::simdjson
else()
  # Dev mode: FetchContent (safe for local builds; no export)
  include(FetchContent)
  if(NOT TARGET simdjson::simdjson)
    FetchContent_Declare(simdjson
      GIT_REPOSITORY https://github.com/simdjson/simdjson.git
      GIT_TAG        v3.10.1
      GIT_SHALLOW    TRUE
      GIT_PROGRESS   TRUE
    )
    FetchContent_MakeAvailable(simdjson)
  endif()
endif()

target_link_libraries(InterstellarIO
  PUBLIC
    simdjson::simdjson
)

# ---------------- Install ----------------
# Always install the built binary + headers (useful even in dev mode)
install(TARGETS InterstellarIO
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/"
        DESTINATION "${INTERSTELLAR_IO_INSTALL_INCLUDE_DIR}"
        FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp" PATTERN "*.hh" PATTERN "*.hxx")

# Only export a CMake package when explicitly enabled
if(IO_ENABLE_PACKAGE)
  install(EXPORT InterstellarIOTargets
    NAMESPACE Interstellar::
    FILE InterstellarIOTargets.cmake
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/InterstellarIO"
  )

  write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/InterstellarIOConfigVersion.cmake"
    VERSION "${PROJECT_VERSION}"
    COMPATIBILITY SameMajorVersion
  )

  file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/InterstellarIOConfig.cmake" [=[
include(CMakeFindDependencyMacro)
find_dependency(simdjson CONFIG REQUIRED)
include("${CMAKE_CURRENT_LIST_DIR}/InterstellarIOTargets.cmake")
]=])

  install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/InterstellarIOConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/InterstellarIOConfigVersion.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/InterstellarIO"
  )
endif()

# ---------------- Tests ----------------
option(IO_BUILD_TESTS "Build IO tests" ${BUILD_TESTING})
if(IO_BUILD_TESTS AND BUILD_TESTING)
  include(SubprojectTesting) # provided by root
  if(COMMAND add_subproject_tests)
    add_subproject_tests(TARGET InterstellarIO NAME IO)
  else()
    message(STATUS "SubprojectTesting not available; skipping IO test wiring.")
  endif()
endif()
