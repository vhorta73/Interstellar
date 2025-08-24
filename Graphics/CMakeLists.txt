cmake_minimum_required(VERSION 3.21)

# Inherit root version; fallback for standalone
if(NOT DEFINED Interstellar_VERSION)
  set(Interstellar_VERSION 0.0.0)
endif()

project(InterstellarGraphicsProj VERSION ${Interstellar_VERSION} LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(EnsureSources)

# ---------------- Options ----------------
option(GRAPHICS_WARNINGS_AS_ERRORS "Treat warnings as errors for Graphics" OFF)
option(GRAPHICS_ENABLE_PACKAGE     "Install CMake package for Interstellar::Graphics" OFF)

# ---------------- Sources ----------------
file(GLOB_RECURSE GRAPHICS_SOURCES CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cxx"
)

# If this project is purely interface/headers, ensure a harmless TU so the lib builds cleanly
interstellar_ensure_sources(GRAPHICS_SOURCES graphics)

# ---------------- Target ----------------
add_library(InterstellarGraphics STATIC ${GRAPHICS_SOURCES})
add_library(Interstellar::Graphics ALIAS InterstellarGraphics)
target_compile_features(InterstellarGraphics PUBLIC cxx_std_20)

# Allow linking this static lib into shared libs on UNIX
if(UNIX)
  set_target_properties(InterstellarGraphics PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

# Public/Private includes
set(INTERSTELLAR_GRAPHICS_INSTALL_INCLUDE_DIR "${CMAKE_INSTALL_INCLUDEDIR}/Interstellar")
target_include_directories(InterstellarGraphics
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${INTERSTELLAR_GRAPHICS_INSTALL_INCLUDE_DIR}>
  INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>  # build-tree only (for inline/private headers)
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# If your public interfaces expose types from other projects, link them here as PUBLIC.
# Example (uncomment as needed):
# target_link_libraries(InterstellarGraphics PUBLIC Interstellar::Utils gsl::gsl-lite mp-units)

# ---------------- Warnings ----------------
if(MSVC)
  target_compile_options(InterstellarGraphics PRIVATE /permissive- /W4 $<$<BOOL:${GRAPHICS_WARNINGS_AS_ERRORS}>:/WX>)
else()
  target_compile_options(InterstellarGraphics PRIVATE -Wall -Wextra -Wpedantic $<$<BOOL:${GRAPHICS_WARNINGS_AS_ERRORS}>:-Werror>)
endif()

# ---------------- Install ----------------
install(TARGETS InterstellarGraphics
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/"
        DESTINATION "${INTERSTELLAR_GRAPHICS_INSTALL_INCLUDE_DIR}"
        FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp" PATTERN "*.hh" PATTERN "*.hxx")

if(GRAPHICS_ENABLE_PACKAGE)
  install(EXPORT InterstellarGraphicsTargets
    NAMESPACE Interstellar::
    FILE InterstellarGraphicsTargets.cmake
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/InterstellarGraphics"
  )

  write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/InterstellarGraphicsConfigVersion.cmake"
    VERSION "${PROJECT_VERSION}"
    COMPATIBILITY SameMajorVersion
  )

  # Minimal config – add find_dependency(...) here if Graphics exposes public deps.
  set(_GRAPHICS_CFG "${CMAKE_CURRENT_BINARY_DIR}/InterstellarGraphicsConfig.cmake")
  file(WRITE "${_GRAPHICS_CFG}" "include(\"\${CMAKE_CURRENT_LIST_DIR}/InterstellarGraphicsTargets.cmake\")\n")

  install(FILES
    "${_GRAPHICS_CFG}"
    "${CMAKE_CURRENT_BINARY_DIR}/InterstellarGraphicsConfigVersion.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/InterstellarGraphics"
  )
endif()

# ---------------- Tests ----------------
option(GRAPHICS_BUILD_TESTS "Build Graphics tests" ${BUILD_TESTING})
if(GRAPHICS_BUILD_TESTS AND BUILD_TESTING)
  include(SubprojectTesting)
  add_subproject_tests(TARGET InterstellarGraphics NAME Graphics)
  # Add test-only include path
  add_library(GraphicsTestSupport INTERFACE)
  target_include_directories(GraphicsTestSupport INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/tests>
  )
  target_link_libraries(GraphicsTests PRIVATE GraphicsTestSupport)
endif()


