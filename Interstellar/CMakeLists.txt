cmake_minimum_required(VERSION 3.21)
project(InterstellarApp LANGUAGES CXX)

# ---------------- Sources ----------------
include(EnsureSources)
file(GLOB_RECURSE APP_SOURCES CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)
# DRY helper: injects an empty TU if needed (no C4505)
interstellar_ensure_sources(APP_SOURCES app)

add_executable(Interstellar ${APP_SOURCES})
target_compile_features(Interstellar PRIVATE cxx_std_20)

# IDE grouping (nice to have)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "Source Files" FILES ${APP_SOURCES})

# ---------------- Includes ----------------
# Only your own public headers. Do NOT include engine src internals.
target_include_directories(Interstellar PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

# ---------------- Linkage ----------------
# Engine should carry glfw/glad privately; the app shouldn't need to link them directly.
target_link_libraries(Interstellar
  PRIVATE
    Interstellar::Engine
    Interstellar::Scenes
    Interstellar::ECS
    Interstellar::Simulation
    Interstellar::Universe
    Interstellar::Procedural
    Interstellar::Utils
    Interstellar::IO
    Interstellar::Renderers
    Interstellar::Graphics
    Interstellar::Input
)

# ---------------- Warnings ----------------
if(MSVC)
  target_compile_options(Interstellar PRIVATE /permissive- /W4 $<$<BOOL:${WARNINGS_AS_ERRORS}>:/WX>)
else()
  target_compile_options(Interstellar PRIVATE -Wall -Wextra -Wpedantic $<$<BOOL:${WARNINGS_AS_ERRORS}>:-Werror>)
endif()

# ---------------- VS debugger working dir ----------------
if(MSVC)
  set_target_properties(Interstellar PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:Interstellar>"
  )
endif()

# ---------------- Git describe banner (optional) ----------------
find_package(Git QUIET)
if(Git_FOUND AND EXISTS "${CMAKE_SOURCE_DIR}/.git")
  execute_process(
    COMMAND ${GIT_EXECUTABLE} describe --tags --always
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE INTERSTELLAR_GIT
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
  )
  if(INTERSTELLAR_GIT)
    target_compile_definitions(Interstellar PRIVATE "INTERSTELLAR_GIT=\"${INTERSTELLAR_GIT}\"")
  endif()
endif()

# ---------------- Copy assets for local runs ----------------
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/assets")
  add_custom_command(TARGET Interstellar POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/assets"
            "$<TARGET_FILE_DIR:Interstellar>/assets"
    COMMENT "Copying assets to output directory"
    VERBATIM
  )
endif()

# ---------------- Resilient DLL copy (no-op if none) ----------------
if (WIN32 AND CMAKE_VERSION VERSION_GREATER_EQUAL "3.21")
  add_custom_command(TARGET Interstellar POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E
            $<IF:$<BOOL:$<TARGET_RUNTIME_DLLS:Interstellar>>,copy_if_different,true>
            $<$<BOOL:$<TARGET_RUNTIME_DLLS:Interstellar>>:$<TARGET_RUNTIME_DLLS:Interstellar>>
            $<$<BOOL:$<TARGET_RUNTIME_DLLS:Interstellar>>:$<TARGET_FILE_DIR:Interstellar>>
    COMMAND_EXPAND_LISTS
    COMMENT "Copying dependent DLLs (if any) next to Interstellar.exe"
    VERBATIM
  )
endif()

# ---------------- Install app ----------------
include(GNUInstallDirs)
install(TARGETS Interstellar
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
if (WIN32 AND CMAKE_VERSION VERSION_GREATER_EQUAL "3.21")
  install(FILES $<TARGET_RUNTIME_DLLS:Interstellar> DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/assets")
  install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/assets/"
          DESTINATION "${CMAKE_INSTALL_BINDIR}/assets"
          FILES_MATCHING
            PATTERN "*"
            PATTERN "*.psd" EXCLUDE)
endif()
if (MSVC)
  install(FILES "$<TARGET_PDB_FILE:Interstellar>" DESTINATION ${CMAKE_INSTALL_BINDIR} OPTIONAL)
endif()

# ---------------- Tests (via helper) ----------------
option(INTERSTELLAR_BUILD_TESTS "Build Interstellar tests" ${BUILD_TESTING})
if(INTERSTELLAR_BUILD_TESTS AND BUILD_TESTING)
  include(SubprojectTesting)
  add_subproject_tests(TARGET Interstellar NAME Interstellar)
endif()
