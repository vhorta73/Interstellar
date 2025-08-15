cmake_minimum_required(VERSION 3.21)

# Inherit the root version; fall back for standalone use
if(NOT DEFINED Interstellar_VERSION)
  set(Interstellar_VERSION 0.0.0)
endif()

project(InterstellarUnits VERSION ${Interstellar_VERSION} LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# ---- Library (header-only) ----
add_library(InterstellarUnits INTERFACE)
add_library(Interstellar::Units ALIAS InterstellarUnits)

# Public headers during build live in repo "include/", installed under include/Interstellar
set(INTERSTELLAR_UNITS_INSTALL_INCLUDE_DIR "${CMAKE_INSTALL_INCLUDEDIR}/Interstellar")

target_include_directories(InterstellarUnits INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${INTERSTELLAR_UNITS_INSTALL_INCLUDE_DIR}>
)

# If headers use C++20 features, declare it for consumers
target_compile_features(InterstellarUnits INTERFACE cxx_std_20)

# ---- Scalar type configuration (cache-safe & validated) ----
set(UNITS_SCALAR "double" CACHE STRING "Scalar type for Interstellar::Units")
set_property(CACHE UNITS_SCALAR PROPERTY STRINGS "float;double")

# Validate
set(_ok_scalars float double)
list(FIND _ok_scalars "${UNITS_SCALAR}" _idx)
if(_idx EQUAL -1)
  message(FATAL_ERROR "UNITS_SCALAR must be one of: ${_ok_scalars}; got: '${UNITS_SCALAR}'")
endif()

# Ensure cache type is STRING
set(UNITS_SCALAR "${UNITS_SCALAR}" CACHE STRING "Scalar type for Interstellar::Units" FORCE)
target_compile_definitions(InterstellarUnits INTERFACE ISL_MP_UNITS_HAVE_UNDERSCORE=1)
target_compile_definitions(InterstellarUnits INTERFACE INTERSTELLAR_UNITS_SCALAR=${UNITS_SCALAR})

# ---- mp-units (link to whichever target name exists) ----
set(_mpu_target "")
foreach(_cand mp-units::units mp-units::mp-units mp-units units)
  if(TARGET ${_cand})
    set(_mpu_target ${_cand})
    break()
  endif()
endforeach()
if(NOT _mpu_target)
  message(FATAL_ERROR "mp-units target not found. Ensure it's available before add_subdirectory(Units).")
endif()
target_link_libraries(InterstellarUnits INTERFACE ${_mpu_target})

# ---- Install (headers + export + package files) ----
install(TARGETS InterstellarUnits
  EXPORT InterstellarUnitsTargets
)

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/"
  DESTINATION "${INTERSTELLAR_UNITS_INSTALL_INCLUDE_DIR}"
  FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp" PATTERN "*.hh" PATTERN "*.hxx"
)

install(EXPORT InterstellarUnitsTargets
  NAMESPACE Interstellar::
  FILE InterstellarUnitsTargets.cmake
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/InterstellarUnits"
)

# Package config (find_package(InterstellarUnits))
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/InterstellarUnitsConfigVersion.cmake"
  VERSION "${PROJECT_VERSION}"
  COMPATIBILITY SameMajorVersion
)
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/InterstellarUnitsConfig.cmake" [=[
# Minimal config: import targets; dependencies like mp-units are in the link interface
include("${CMAKE_CURRENT_LIST_DIR}/InterstellarUnitsTargets.cmake")
]=])
install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/InterstellarUnitsConfig.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/InterstellarUnitsConfigVersion.cmake"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/InterstellarUnits"
)

# ---- Tests (via helper) ----
option(UNITS_BUILD_TESTS "Build Units tests" ${BUILD_TESTING})
if(UNITS_BUILD_TESTS AND BUILD_TESTING)
  include(SubprojectTesting)  # provided by root via CMAKE_MODULE_PATH
  add_subproject_tests(TARGET InterstellarUnits NAME Units)
  if(TARGET UnitsTests)
    target_include_directories(UnitsTests PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}/tests
      ${CMAKE_CURRENT_SOURCE_DIR}/tests/helpers
  )
  endif()
endif()
