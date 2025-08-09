cmake_minimum_required(VERSION 3.29)
project(InterstellarApp LANGUAGES CXX)

# ---- Sources & target ----
file(GLOB_RECURSE APP_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
add_executable(Interstellar ${APP_SOURCES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "Source Files" FILES ${APP_SOURCES})

# ---- Includes ----
target_include_directories(Interstellar PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/Engine/include
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/Engine/src>
)

# ---- Link ----
target_link_libraries(Interstellar PRIVATE
  Engine
  glfw
  glad
)

# ---- Visual Studio: make relative file paths resolve at runtime ----
if (MSVC)
  set_target_properties(Interstellar PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:Interstellar>"
  )
endif()

# ---- Copy assets for local runs (F5) ----
add_custom_command(TARGET Interstellar POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          "${CMAKE_CURRENT_SOURCE_DIR}/assets"
          "$<TARGET_FILE_DIR:Interstellar>/assets"
  COMMENT "Copying assets to output directory"
  VERBATIM
)

# ---- Install the app ----
install(TARGETS Interstellar
  RUNTIME DESTINATION bin
  ARCHIVE DESTINATION lib
)

# ---- Copy dependent DLLs (both for F5 and for install) ----
# This works on CMake >= 3.21. If VS is using an older CMake, we just warn.
if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.21")
  # After build: copy runtime DLLs next to the exe for debugging/F5
  add_custom_command(TARGET Interstellar POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:Interstellar> $<TARGET_FILE_DIR:Interstellar>
    COMMAND_EXPAND_LISTS
    COMMENT "Copying dependent DLLs next to Interstellar.exe"
  )

  # On install: include those DLLs in the package
  install(FILES $<TARGET_RUNTIME_DLLS:Interstellar> DESTINATION bin)
else()
  message(WARNING
    "CMake ${CMAKE_VERSION} < 3.21 detected: automatic dependency copying disabled. "
    "Upgrade CMake (or let Visual Studio use a newer CMake) to auto-copy dependent DLLs."
  )
endif()

# ---- Install assets beside the exe ----
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/assets/"
        DESTINATION "bin/assets"
        FILES_MATCHING
          PATTERN "*"
          PATTERN "*.psd" EXCLUDE
)

# ---- Ship PDBs (optional but handy) ----
if (MSVC)
  install(FILES "$<TARGET_PDB_FILE:Interstellar>" DESTINATION bin OPTIONAL)
endif()
