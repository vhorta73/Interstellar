cmake_minimum_required(VERSION 3.21)
project(InterstellarData LANGUAGES CXX)

include(GNUInstallDirs)
include(EnsureSources)  # DRY helper

# ---- Sources ----
file(GLOB_RECURSE DATA_SOURCES CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cxx"
)
# If empty, inject an empty TU; on MSVC apply /wd4505 just to that file
interstellar_ensure_sources(DATA_SOURCES data)

# ---- Library ----
add_library(InterstellarData STATIC ${DATA_SOURCES})
add_library(Interstellar::Data ALIAS InterstellarData)
target_compile_features(InterstellarData PUBLIC cxx_std_20)

target_include_directories(InterstellarData
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# ---- Linkage ----
target_link_libraries(InterstellarData
  PUBLIC
    Interstellar::Units
    Interstellar::Utils
)

# ---- Warnings ----
if(MSVC)
  target_compile_options(InterstellarData PRIVATE /permissive- /W4 $<$<BOOL:${WARNINGS_AS_ERRORS}>:/WX>)
else()
  target_compile_options(InterstellarData PRIVATE -Wall -Wextra -Wpedantic $<$<BOOL:${WARNINGS_AS_ERRORS}>:-Werror>)
endif()

# ---- Install ----
install(TARGETS InterstellarData
  EXPORT InterstellarDataTargets
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/"
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# ---- Tests ----
option(DATA_BUILD_TESTS "Build Data tests" ${BUILD_TESTING})
if(DATA_BUILD_TESTS AND BUILD_TESTING)
  include(SubprojectTesting)
  add_subproject_tests(TARGET InterstellarData NAME Data)
endif()
