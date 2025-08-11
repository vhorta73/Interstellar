cmake_minimum_required(VERSION 3.21)
project(InterstellarUnits LANGUAGES CXX)

# ---- Sources (simple + placeholder if empty) ----
file(GLOB_RECURSE UNITS_SOURCES CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cxx"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc"
)
if(UNITS_SOURCES STREQUAL "")
  set(_units_placeholder "${CMAKE_CURRENT_BINARY_DIR}/interstellar_units_placeholder.cpp")
  file(WRITE "${_units_placeholder}" "/* placeholder */ void interstellar_units_placeholder(){}")
  list(APPEND UNITS_SOURCES "${_units_placeholder}")
endif()

# ---- Library ----
add_library(InterstellarUnits STATIC ${UNITS_SOURCES})
add_library(Interstellar::Units ALIAS InterstellarUnits)
target_compile_features(InterstellarUnits PUBLIC cxx_std_20)

# ---- Includes ----
target_include_directories(InterstellarUnits
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# ---- Warnings (kept lightweight) ----
option(UNITS_WARNINGS_AS_ERRORS "Treat warnings as errors for Interstellar::Units" OFF)
if(MSVC)
  target_compile_options(InterstellarUnits PRIVATE /permissive- /W4 $<$<BOOL:${UNITS_WARNINGS_AS_ERRORS}>:/WX>)
else()
  target_compile_options(InterstellarUnits PRIVATE -Wall -Wextra -Wpedantic $<$<BOOL:${UNITS_WARNINGS_AS_ERRORS}>:-Werror>)
endif()

# ---- Detect mp-units target name ----
set(_mpu_target "")
foreach(_cand mp-units::units mp-units::mp-units mp-units units)
  if(TARGET ${_cand})
    set(_mpu_target ${_cand})
    break()
  endif()
endforeach()

if(NOT _mpu_target)
  message(FATAL_ERROR
    "mp-units target not found. Ensure FetchContent_MakeAvailable(mp_units) ran "
    "BEFORE add_subdirectory(Units), or install/find mp-units."
  )
endif()

# ---- Link dependencies ----
target_link_libraries(InterstellarUnits
  PUBLIC
    ${_mpu_target}
)


# ---- Install (optional, compact) ----
include(GNUInstallDirs)
install(TARGETS InterstellarUnits
  EXPORT InterstellarUnitsTargets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(EXPORT InterstellarUnitsTargets
  NAMESPACE Interstellar::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/InterstellarUnits
)
