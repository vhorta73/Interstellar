cmake_minimum_required(VERSION 3.21)

# Inherit root version; fallback for standalone builds
if(NOT DEFINED Interstellar_VERSION)
  set(Interstellar_VERSION 0.0.0)
endif()

project(ScenesProj VERSION ${Interstellar_VERSION} LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(EnsureSources)  # shared empty-TU helper

# ---------------- Sources ----------------
file(GLOB_RECURSE SCENES_SOURCES CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)
# If empty, inject an empty TU so the target shows up in IDEs
interstellar_ensure_sources(SCENES_SOURCES scenes)

# ---------------- Library ----------------
add_library(Scenes STATIC ${SCENES_SOURCES})
add_library(Interstellar::Scenes ALIAS Scenes)
target_compile_features(Scenes PUBLIC cxx_std_20)

# Allow linking this static lib into shared libs on UNIX
if(UNIX)
  set_target_properties(Scenes PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

# Public/Private includes
# Put headers under: Scenes/include/Interstellar/Scenes/...
set(INTERSTELLAR_SCENES_INSTALL_INCLUDE_DIR "${CMAKE_INSTALL_INCLUDEDIR}/Interstellar")
target_include_directories(Scenes
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${INTERSTELLAR_SCENES_INSTALL_INCLUDE_DIR}>
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# ---------------- Warnings ----------------
option(SCENES_WARNINGS_AS_ERRORS "Treat warnings as errors for Scenes" OFF)
if(MSVC)
  target_compile_options(Scenes PRIVATE /permissive- /W4 $<$<BOOL:${SCENES_WARNINGS_AS_ERRORS}>:/WX>)
else()
  target_compile_options(Scenes PRIVATE -Wall -Wextra -Wpedantic $<$<BOOL:${SCENES_WARNINGS_AS_ERRORS}>:-Werror>)
endif()

# ---------------- Dependencies ----------------
# Only glm
include(FetchContent)
if(NOT TARGET glm::glm)
  FetchContent_Declare(glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        0.9.9.8
    GIT_SHALLOW    TRUE
    GIT_PROGRESS   TRUE
  )
endif()
FetchContent_MakeAvailable(glm)

target_link_libraries(Scenes
  PUBLIC
    glm::glm
    Interstellar::Graphics
    Interstellar::Universe
    Interstellar::Engine
    Interstellar::Input
    Interstellar::Renderers
)

# ---------------- Install ----------------
option(SCENES_ENABLE_PACKAGE "Install CMake package for Interstellar::Scenes" OFF)

install(TARGETS Scenes
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# Install public headers under include/Interstellar
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/"
        DESTINATION "${INTERSTELLAR_SCENES_INSTALL_INCLUDE_DIR}"
        FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp" PATTERN "*.hh" PATTERN "*.hxx")

if(SCENES_ENABLE_PACKAGE)
  install(EXPORT ScenesTargets
    NAMESPACE Interstellar::
    FILE ScenesTargets.cmake
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/InterstellarScenes"
  )

  write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/InterstellarScenesConfigVersion.cmake"
    VERSION "${PROJECT_VERSION}"
    COMPATIBILITY SameMajorVersion
  )

  file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/InterstellarScenesConfig.cmake" [=[
include(CMakeFindDependencyMacro)
find_dependency(glm)
include("${CMAKE_CURRENT_LIST_DIR}/ScenesTargets.cmake")
]=])

  install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/InterstellarScenesConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/InterstellarScenesConfigVersion.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/InterstellarScenes"
  )
endif()

# ---------------- Tests ----------------
option(SCENES_BUILD_TESTS "Build Scenes tests" ${BUILD_TESTING})
if(SCENES_BUILD_TESTS AND BUILD_TESTING)
  include(SubprojectTesting)
  add_subproject_tests(TARGET Scenes NAME Scenes)
endif()
