cmake_minimum_required(VERSION 3.21)

# Inherit root version; fallback for standalone
if(NOT DEFINED Interstellar_VERSION)
  set(Interstellar_VERSION 0.0.0)
endif()

project(InterstellarRenderersProj VERSION ${Interstellar_VERSION} LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(EnsureSources)
include(FetchContent)

# ---------------- Options ----------------
option(RENDERERS_WARNINGS_AS_ERRORS "Treat warnings as errors for Renderers" OFF)
option(RENDERERS_ENABLE_PACKAGE     "Install CMake package for Interstellar::Renderers" OFF)

# Backends
option(RENDERERS_BACKEND_OPENGL "Enable OpenGL renderer backend" ON)
option(RENDERERS_BACKEND_VULKAN "Enable Vulkan renderer backend" ON)

# ---------------- Sources ----------------
file(GLOB_RECURSE RENDERERS_SOURCES CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cxx"
)

# Exclude backend files by filename hints if a backend is disabled
if(NOT RENDERERS_BACKEND_OPENGL)
  list(FILTER RENDERERS_SOURCES EXCLUDE REGEX ".*/(GL|OpenGL|Gl)[^/]*\\.(c|cc|cpp|cxx)$")
endif()
if(NOT RENDERERS_BACKEND_VULKAN)
  list(FILTER RENDERERS_SOURCES EXCLUDE REGEX ".*/(Vk|Vulkan)[^/]*\\.(c|cc|cpp|cxx)$")
endif()

# Fallback empty TU
interstellar_ensure_sources(RENDERERS_SOURCES renderers)

# ---------------- Target ----------------
add_library(InterstellarRenderers STATIC ${RENDERERS_SOURCES})
add_library(Interstellar::Renderers ALIAS InterstellarRenderers)
target_compile_features(InterstellarRenderers PUBLIC cxx_std_20)

# Allow linking this static lib into shared libs on UNIX
if(UNIX)
  set_target_properties(InterstellarRenderers PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

# Public/Private includes
set(INTERSTELLAR_RENDERERS_INSTALL_INCLUDE_DIR "${CMAKE_INSTALL_INCLUDEDIR}/Interstellar")
target_include_directories(InterstellarRenderers
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src> 
    $<INSTALL_INTERFACE:${INTERSTELLAR_RENDERERS_INSTALL_INCLUDE_DIR}>
  INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>  # build-tree only
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Must consume the public Graphics interfaces
target_link_libraries(InterstellarRenderers
  PUBLIC
    Interstellar::Graphics
    Interstellar::Logging
  PRIVATE
    glfw
)

# ---------------- Backends & Dependencies ----------------

# --- OpenGL backend ---
if(RENDERERS_BACKEND_OPENGL)
  target_compile_definitions(InterstellarRenderers PUBLIC INTERSTELLAR_RENDERER_OPENGL=1)

  # OpenGL
  find_package(OpenGL QUIET COMPONENTS OpenGL)
  if(TARGET OpenGL::OpenGL)
    target_link_libraries(InterstellarRenderers PUBLIC OpenGL::OpenGL)
  elseif(TARGET OpenGL::GL)
    target_link_libraries(InterstellarRenderers PUBLIC OpenGL::GL)
  elseif(WIN32)
    target_link_libraries(InterstellarRenderers PUBLIC opengl32)
  elseif(APPLE)
    find_library(COCOA_FRAMEWORK Cocoa)
    find_library(OPENGL_FRAMEWORK OpenGL)
    if(OPENGL_FRAMEWORK)
      target_link_libraries(InterstellarRenderers PUBLIC ${OPENGL_FRAMEWORK} ${COCOA_FRAMEWORK})
    else()
      message(FATAL_ERROR "OpenGL framework not found on macOS. Disable RENDERERS_BACKEND_OPENGL or provide OpenGL.")
    endif()
  else()
    find_library(LIBGL GL)
    if(LIBGL)
      target_link_libraries(InterstellarRenderers PUBLIC ${LIBGL})
    else()
      message(FATAL_ERROR "Could not locate OpenGL. Disable RENDERERS_BACKEND_OPENGL or install OpenGL.")
    endif()
  endif()

  # GLAD (uses your root toggles like DISABLE_GLAD_REGEN if set)
  if(NOT TARGET glad)
    FetchContent_Declare(
      glad
      GIT_REPOSITORY https://github.com/Dav1dde/glad.git
      GIT_TAG        v2.0.6
      GIT_SHALLOW    TRUE
      GIT_PROGRESS   TRUE
    )
    # Respect unity issues inside third-party
    set(CMAKE_UNITY_BUILD OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(glad)
  endif()
  target_link_libraries(InterstellarRenderers PUBLIC glad)
endif()

# --- Vulkan backend ---
if(RENDERERS_BACKEND_VULKAN)
  target_compile_definitions(InterstellarRenderers PUBLIC INTERSTELLAR_RENDERER_VULKAN=1)

  find_package(Vulkan QUIET)
  if(Vulkan_FOUND)
    target_link_libraries(InterstellarRenderers PUBLIC Vulkan::Vulkan)
  else()
    message(FATAL_ERROR "Vulkan SDK not found. Disable RENDERERS_BACKEND_VULKAN or install Vulkan.")
  endif()
endif()

# ---------------- Warnings ----------------
if(MSVC)
  target_compile_options(InterstellarRenderers PRIVATE /permissive- /W4 $<$<BOOL:${RENDERERS_WARNINGS_AS_ERRORS}>:/WX>)
else()
  target_compile_options(InterstellarRenderers PRIVATE -Wall -Wextra -Wpedantic $<$<BOOL:${RENDERERS_WARNINGS_AS_ERRORS}>:-Werror>)
endif()

# ---------------- Install ----------------
install(TARGETS InterstellarRenderers
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/"
        DESTINATION "${INTERSTELLAR_RENDERERS_INSTALL_INCLUDE_DIR}"
        FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp" PATTERN "*.hh" PATTERN "*.hxx")

if(RENDERERS_ENABLE_PACKAGE)
  install(EXPORT InterstellarRenderersTargets
    NAMESPACE Interstellar::
    FILE InterstellarRenderersTargets.cmake
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/InterstellarRenderers"
  )

  write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/InterstellarRenderersConfigVersion.cmake"
    VERSION "${PROJECT_VERSION}"
    COMPATIBILITY SameMajorVersion
  )

  set(_RENDERERS_CFG "${CMAKE_CURRENT_BINARY_DIR}/InterstellarRenderersConfig.cmake")
  file(WRITE "${_RENDERERS_CFG}" "include(CMakeFindDependencyMacro)\n")
  file(APPEND "${_RENDERERS_CFG}" "find_dependency(InterstellarGraphics REQUIRED)\n")
  if(RENDERERS_BACKEND_VULKAN)
    file(APPEND "${_RENDERERS_CFG}" "find_dependency(Vulkan REQUIRED)\n")
  endif()
  # OpenGL has many platform names; we intentionally don't force it here.
  file(APPEND "${_RENDERERS_CFG}" "include(\"\${CMAKE_CURRENT_LIST_DIR}/InterstellarRenderersTargets.cmake\")\n")

  install(FILES
    "${_RENDERERS_CFG}"
    "${CMAKE_CURRENT_BINARY_DIR}/InterstellarRenderersConfigVersion.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/InterstellarRenderers"
  )
endif()

# ---------------- Tests ----------------
option(RENDERERS_BUILD_TESTS "Build Renderers tests" ${BUILD_TESTING})
if(RENDERERS_BUILD_TESTS AND BUILD_TESTING)
  include(SubprojectTesting)
  add_subproject_tests(TARGET InterstellarRenderers NAME Renderers)

  # If the helper creates 'RenderersTests', wire it up explicitly:
  if(TARGET RenderersTests)
    target_link_libraries(RenderersTests PRIVATE Interstellar::Renderers)
    # (Optional) belt-and-braces: add src include directly too
    target_include_directories(RenderersTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
  endif()
endif()
