cmake_minimum_required(VERSION 3.21)

# Inherit root version; fallback for standalone
if(NOT DEFINED Interstellar_VERSION)
  set(Interstellar_VERSION 0.0.0)
endif()

project(InterstellarInputProj VERSION ${Interstellar_VERSION} LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(EnsureSources)   # same helper you use in IO

# ---------------- Options ----------------
option(INPUT_WARNINGS_AS_ERRORS "Treat warnings as errors for Input" OFF)
option(INPUT_ENABLE_PACKAGE     "Install CMake package for Interstellar::Input (use system deps)" OFF)

# Backends (toggle as needed)
option(INPUT_BACKEND_GLFW   "Enable GLFW backend"            ON)
#option(INPUT_BACKEND_SDL    "Enable SDL2 backend"            OFF)
option(INPUT_BACKEND_WIN32  "Enable Win32 Raw Input backend" ON)  # Windows only
option(INPUT_BACKEND_XINPUT "Enable XInput backend"          ON)  # Windows only

# ---------------- Sources ----------------
file(GLOB_RECURSE INPUT_SOURCES CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cxx"
)

# Exclude backend files that are disabled (by filename hints)
#if(NOT INPUT_BACKEND_SDL)
#  list(FILTER INPUT_SOURCES EXCLUDE REGEX ".*/(SDL|Sdl)[^/]*\\.(c|cc|cpp|cxx)$")
#endif()
if(NOT INPUT_BACKEND_GLFW)
  list(FILTER INPUT_SOURCES EXCLUDE REGEX ".*/(GLFW|Glfw)[^/]*\\.(c|cc|cpp|cxx)$")
endif()
# Exclude Windows-only backends if disabled or on non-Windows
if(NOT WIN32 OR NOT INPUT_BACKEND_WIN32)
  list(FILTER INPUT_SOURCES EXCLUDE REGEX ".*/(Win32|Windows|RawInput)[^/]*\\.(c|cc|cpp|cxx)$")
endif()
if(NOT WIN32 OR NOT INPUT_BACKEND_XINPUT)
  list(FILTER INPUT_SOURCES EXCLUDE REGEX ".*/(XInput|Xinput)[^/]*\\.(c|cc|cpp|cxx)$")
endif()

# Fallback empty TU if none found (helper also applies /wd4505 on MSVC)
interstellar_ensure_sources(INPUT_SOURCES input)

# ---------------- Target ----------------
add_library(InterstellarInput STATIC ${INPUT_SOURCES})
add_library(Interstellar::Input ALIAS InterstellarInput)
target_compile_features(InterstellarInput PUBLIC cxx_std_20)

# Allow linking this static lib into shared libs on UNIX
if(UNIX)
  set_target_properties(InterstellarInput PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

# Public/Private includes
set(INTERSTELLAR_INPUT_INSTALL_INCLUDE_DIR "${CMAKE_INSTALL_INCLUDEDIR}/Interstellar")
target_include_directories(InterstellarInput
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${INTERSTELLAR_INPUT_INSTALL_INCLUDE_DIR}>
  INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>  # build-tree only
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# ---------------- Warnings ----------------
if(MSVC)
  target_compile_options(InterstellarInput PRIVATE /permissive- /W4 $<$<BOOL:${INPUT_WARNINGS_AS_ERRORS}>:/WX>)
else()
  target_compile_options(InterstellarInput PRIVATE -Wall -Wextra -Wpedantic $<$<BOOL:${INPUT_WARNINGS_AS_ERRORS}>:-Werror>)
endif()

# ---------------- Backends & Dependencies ----------------

# GLFW backend
if(INPUT_BACKEND_GLFW)
  target_compile_definitions(InterstellarInput PUBLIC INTERSTELLAR_INPUT_BACKEND_GLFW=1)

  find_package(glfw3 CONFIG QUIET)
  if(glfw3_FOUND)
    target_link_libraries(InterstellarInput PUBLIC glfw)
  else()
    include(FetchContent)
    if(NOT TARGET glfw)
      FetchContent_Declare(glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG        3.3.9
        GIT_SHALLOW    TRUE
        GIT_PROGRESS   TRUE
      )
      set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
      set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
      set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
      FetchContent_MakeAvailable(glfw)
    endif()
    target_link_libraries(InterstellarInput PUBLIC glfw)
  endif()
endif()

# SDL2 backend (opt-in)
#if(INPUT_BACKEND_SDL)
#  target_compile_definitions(InterstellarInput PUBLIC INTERSTELLAR_INPUT_BACKEND_SDL=1)
#
#  if(INPUT_ENABLE_PACKAGE)
#    find_package(SDL2 CONFIG QUIET)
#    if(NOT SDL2_FOUND)
#      find_package(SDL2 REQUIRED) # will use FindSDL2.cmake if provided
#    endif()
#  else()
#    include(FetchContent)
#    if(NOT TARGET SDL2::SDL2 AND NOT TARGET SDL2)
#      FetchContent_Declare(SDL2
#        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
#        GIT_TAG        release-2.30.5
#        GIT_SHALLOW    TRUE
#        GIT_PROGRESS   TRUE
#        CMAKE_ARGS
#          -DCMAKE_UNITY_BUILD=OFF   # avoid unity build ODR issues inside SDL
#          -DSDL_TEST=OFF
#          -DSDL2_DISABLE_INSTALL=ON
#          -DSDL_INSTALL=OFF
#      )
#      # Keep the build lean (older/newer toggles)
#      set(SDL_TEST             OFF CACHE BOOL "" FORCE)
#      set(SDL2_DISABLE_INSTALL ON  CACHE BOOL "" FORCE)
#      set(SDL_INSTALL          OFF CACHE BOOL "" FORCE)
#      FetchContent_MakeAvailable(SDL2)
#    endif()
#  endif()
#
#  # Link to whichever target SDL exposes
#  if(TARGET SDL2::SDL2)
#    target_link_libraries(InterstellarInput PUBLIC SDL2::SDL2)
#  elseif(TARGET SDL2)
#    target_link_libraries(InterstellarInput PUBLIC SDL2)
#  elseif(DEFINED SDL2_LIBRARIES)
#    target_include_directories(InterstellarInput PUBLIC ${SDL2_INCLUDE_DIRS})
#    target_link_libraries(InterstellarInput PUBLIC ${SDL2_LIBRARIES})
#  else()
#    message(FATAL_ERROR "Could not locate SDL2. Set -DINPUT_BACKEND_SDL=OFF or provide SDL2.")
#  endif()
#endif()

# Win32 Raw Input backend (Windows only)
if(WIN32 AND INPUT_BACKEND_WIN32)
  target_compile_definitions(InterstellarInput PUBLIC INTERSTELLAR_INPUT_BACKEND_WIN32=1)
  target_link_libraries(InterstellarInput PRIVATE user32)
endif()

# XInput backend (Windows only)
if(WIN32 AND INPUT_BACKEND_XINPUT)
  target_compile_definitions(InterstellarInput PUBLIC INTERSTELLAR_INPUT_BACKEND_XINPUT=1)
  if(MSVC)
    target_link_libraries(InterstellarInput PRIVATE xinput)
  else()
    target_link_libraries(InterstellarInput PRIVATE xinput9_1_0)
  endif()
endif()

# ---------------- Install ----------------
install(TARGETS InterstellarInput
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/"
        DESTINATION "${INTERSTELLAR_INPUT_INSTALL_INCLUDE_DIR}"
        FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp" PATTERN "*.hh" PATTERN "*.hxx")

if(INPUT_ENABLE_PACKAGE)
  install(EXPORT InterstellarInputTargets
    NAMESPACE Interstellar::
    FILE InterstellarInputTargets.cmake
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/InterstellarInput"
  )

  write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/InterstellarInputConfigVersion.cmake"
    VERSION "${PROJECT_VERSION}"
    COMPATIBILITY SameMajorVersion
  )

  # Generate InterstellarInputConfig.cmake with conditional dependencies
  set(_INPUT_CFG "${CMAKE_CURRENT_BINARY_DIR}/InterstellarInputConfig.cmake")
  file(WRITE "${_INPUT_CFG}" "include(CMakeFindDependencyMacro)\n")
#  if(INPUT_BACKEND_SDL)
#    file(APPEND "${_INPUT_CFG}" "find_dependency(SDL2 REQUIRED)\n")
#  endif()
  if(INPUT_BACKEND_GLFW)
    file(APPEND "${_INPUT_CFG}" "find_dependency(glfw3 REQUIRED)\n")
  endif()
  file(APPEND "${_INPUT_CFG}" "include(\"\${CMAKE_CURRENT_LIST_DIR}/InterstellarInputTargets.cmake\")\n")

  install(FILES
    "${_INPUT_CFG}"
    "${CMAKE_CURRENT_BINARY_DIR}/InterstellarInputConfigVersion.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/InterstellarInput"
  )
endif()

# ---------------- Tests ----------------
option(INPUT_BUILD_TESTS "Build Input tests" ${BUILD_TESTING})
if(INPUT_BUILD_TESTS AND BUILD_TESTING)
  include(SubprojectTesting)
  add_subproject_tests(TARGET InterstellarInput NAME Input)
endif()
