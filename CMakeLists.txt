cmake_minimum_required(VERSION 3.29)
project(Interstellar VERSION 1.0 LANGUAGES CXX)

# Options
option(BUILD_TESTING "Enable building of tests" ON)

# C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Fetch GoogleTest automatically
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.14.0
)
# On Windows, ensure matched CRT linkage
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

# Modules
add_subdirectory(Engine)
add_subdirectory(Interstellar)
if(BUILD_TESTING)
  # Ensure Vulkan SDK is defined for tests that may use it
  if(NOT DEFINED ENV{VULKAN_SDK})
    message(FATAL_ERROR "Please set the VULKAN_SDK environment variable to your Vulkan SDK root.")
  endif()
  add_subdirectory(tests)
endif()
