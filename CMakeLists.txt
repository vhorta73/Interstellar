cmake_minimum_required(VERSION 3.29)
project(Interstellar VERSION 1.0 LANGUAGES CXX)

# Silence MSVC deprecation warnings for stdext iterators
if(MSVC)
  add_compile_definitions(_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING)
endif()

option(BUILD_TESTING "Enable building of tests" ON)
option(BUILD_DOCS "Build API documentation with Doxygen" OFF)

# Optional: silence deprecation/dev warnings coming from third-party deps
option(SILENCE_THIRD_PARTY_WARNINGS "Turn off deprecation/developer warnings from third-party CMake projects" ON)
if(SILENCE_THIRD_PARTY_WARNINGS)
  # Deprecation warnings (covers messages from external CMakeLists like nlohmann_json)
  set(CMAKE_WARN_DEPRECATED OFF CACHE BOOL "" FORCE)
  # Developer warnings (noisy in FetchContent projects)
  set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS 1 CACHE INTERNAL "")
endif()

# C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Put installs under the build dir by default, but let users override on CLI
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/package" CACHE PATH "Install path prefix" FORCE)
endif()

# Helpful on MSVC: ensure runtime choice is explicit (consistent CRT)
if(MSVC AND NOT DEFINED CMAKE_MSVC_RUNTIME_LIBRARY)
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
endif()

# Testing (fetch googletest only if we actually build tests)
include(CTest)
if(BUILD_TESTING)
  include(FetchContent)
  FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.14.0
  )
  # On Windows, ensure matched CRT linkage
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(googletest)

  # Vulkan requirement for SOME tests; don’t hard fail the whole build by default
  if(NOT DEFINED ENV{VULKAN_SDK})
    message(WARNING "VULKAN_SDK is not set; Vulkan-dependent tests may be skipped.")
  endif()

  add_subdirectory(tests)
endif()

# Projects
add_subdirectory(Engine)
add_subdirectory(Interstellar)

# Docs (optional; not part of ALL to avoid slowing normal builds)
if(BUILD_DOCS)
  find_package(Doxygen REQUIRED)
  set(DOXYGEN_IN  "${CMAKE_SOURCE_DIR}/docs/Doxyfile")
  if(NOT EXISTS "${DOXYGEN_IN}")
    message(FATAL_ERROR "Doxygen config not found: ${DOXYGEN_IN}")
  endif()

  add_custom_target(Documentation
    COMMENT "Generating API docs with Doxygen"
    VERBATIM
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    COMMAND "${DOXYGEN_EXECUTABLE}" "${DOXYGEN_IN}"
  )
endif()

# Packaging (CPack)
include(InstallRequiredSystemLibraries)  # MSVC redist if available
set(CPACK_PACKAGE_NAME "Interstellar")
set(CPACK_PACKAGE_VENDOR "YourCompany")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_GENERATOR "ZIP")               # or NSIS/WIX if you want an installer later
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
include(CPack)
