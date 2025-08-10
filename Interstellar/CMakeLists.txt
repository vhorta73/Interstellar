cmake_minimum_required(VERSION 3.21)
project(InterstellarApp LANGUAGES CXX)

# Sources
file(GLOB_RECURSE APP_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
add_executable(Interstellar ${APP_SOURCES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "Source Files" FILES ${APP_SOURCES})

# Includes
target_include_directories(Interstellar PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/Engine/include
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/Engine/src>
)

# Link: Engine + what main.cpp includes
target_link_libraries(Interstellar PRIVATE Engine glfw glad)
if(MSVC)
  target_link_libraries(Interstellar PRIVATE opengl32)
endif()

# Warnings (your app only)
if(MSVC)
  target_compile_options(Interstellar PRIVATE /permissive- /W4)
else()
  target_compile_options(Interstellar PRIVATE -Wall -Wextra -Wpedantic)
endif()
if(WARNINGS_AS_ERRORS)
  if(MSVC)
    target_compile_options(Interstellar PRIVATE /WX)
  else()
    target_compile_options(Interstellar PRIVATE -Werror)
  endif()
endif()

# VS debugger working dir
if (MSVC)
  set_target_properties(Interstellar PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:Interstellar>"
  )
endif()

# Git describe banner (optional)
find_package(Git QUIET)
if(Git_FOUND)
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

# Copy assets for local runs
add_custom_command(TARGET Interstellar POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          "${CMAKE_CURRENT_SOURCE_DIR}/assets"
          "$<TARGET_FILE_DIR:Interstellar>/assets"
  COMMENT "Copying assets to output directory"
  VERBATIM
)

# Resilient DLL copy (no-op if there are no DLLs)
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

# Install app
include(GNUInstallDirs)
install(TARGETS Interstellar
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# Install runtime DLLs (harmless if none)
if (WIN32 AND CMAKE_VERSION VERSION_GREATER_EQUAL "3.21")
  install(FILES $<TARGET_RUNTIME_DLLS:Interstellar> DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()

# Install assets
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/assets/"
        DESTINATION "${CMAKE_INSTALL_BINDIR}/assets"
        FILES_MATCHING
          PATTERN "*"
          PATTERN "*.psd" EXCLUDE
)

# Ship PDBs (useful for release crash logs)
if (MSVC)
  install(FILES "$<TARGET_PDB_FILE:Interstellar>" DESTINATION ${CMAKE_INSTALL_BINDIR} OPTIONAL)
endif()
