cmake_minimum_required(VERSION 3.21)
project(InterstellarIO LANGUAGES CXX)

# ---- Sources (robust glob) ----
file(GLOB_RECURSE IO_SOURCES CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cxx"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc"
)

# If no sources yet, create a tiny placeholder so the static lib is valid
if(IO_SOURCES STREQUAL "")
  set(_io_placeholder "${CMAKE_CURRENT_BINARY_DIR}/interstellar_io_placeholder.cpp")
  file(WRITE "${_io_placeholder}" "/* placeholder */ void interstellar_io_placeholder(){}")
  list(APPEND IO_SOURCES "${_io_placeholder}")
endif()

add_library(InterstellarIO STATIC ${IO_SOURCES})
add_library(Interstellar::IO ALIAS InterstellarIO)
target_compile_features(InterstellarIO PUBLIC cxx_std_20)

# ---- Includes ----
target_include_directories(InterstellarIO
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# ---- Warnings (local to this target) ----
option(IO_WARNINGS_AS_ERRORS "Treat warnings as errors for Interstellar::IO" OFF)
if(MSVC)
  target_compile_options(InterstellarIO PRIVATE /permissive- /W4 $<$<BOOL:${IO_WARNINGS_AS_ERRORS}>:/WX>)
else()
  target_compile_options(InterstellarIO PRIVATE -Wall -Wextra -Wpedantic $<$<BOOL:${IO_WARNINGS_AS_ERRORS}>:-Werror>)
endif()

# ---- Dependencies (kept PRIVATE so Engine/Game don't see JSON libs) ----
include(FetchContent)

# simdjson (fast reader)
if(NOT TARGET simdjson::simdjson)
  FetchContent_Declare(simdjson
    GIT_REPOSITORY https://github.com/simdjson/simdjson.git
    GIT_TAG        v3.10.1
    GIT_SHALLOW    TRUE
    GIT_PROGRESS   TRUE
  )
  FetchContent_MakeAvailable(simdjson)
endif()

# nlohmann/j
