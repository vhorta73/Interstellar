cmake_minimum_required(VERSION 3.29)
project(InterstellarApp LANGUAGES CXX)

# 1. Gather application sources
file(GLOB_RECURSE APP_SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# 2. Define the executable
add_executable(Interstellar ${APP_SOURCES})

# 3. Source grouping for IDEs
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "Source Files" FILES ${APP_SOURCES})

# 4. Include paths
target_include_directories(Interstellar
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/Engine/include
)

# 5. Link against Engine
target_link_libraries(Interstellar PRIVATE Engine)

# 6. Copy assets directory to output on build
add_custom_command(TARGET Interstellar PRE_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/assets"
    "$<TARGET_FILE_DIR:Interstellar>/assets"
  COMMENT "Copying assets to output directory"
)
