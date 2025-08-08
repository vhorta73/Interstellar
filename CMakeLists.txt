
cmake_minimum_required(VERSION 3.29)
project(Interstellar VERSION 1.0 LANGUAGES CXX)

# Silence MSVC deprecation warnings for stdext iterators
if(MSVC)
  add_compile_definitions(_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING)
endif()

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

# Enable CTest for test discovery
include(CTest)

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

find_package(Doxygen REQUIRED)

set(DOXYGEN_IN "${CMAKE_SOURCE_DIR}/docs/Doxyfile")
set(DOXYGEN_OUT "${CMAKE_BINARY_DIR}/docs")

# Optional: sanity check that it exists at configure time
if(NOT EXISTS "${DOXYGEN_IN}")
  message(FATAL_ERROR "Doxygen config not found: ${DOXYGEN_IN}")
endif()

add_custom_target(Documentation ALL
  # NOTE: COMMAND takes the executable and each argument separately.
  COMMENT "Generating API docs with Doxygen"
  VERBATIM
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
  COMMAND "${DOXYGEN_EXECUTABLE}" "${DOXYGEN_IN}"
)