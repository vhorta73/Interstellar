
cmake_minimum_required(VERSION 3.29)
project(InterstellarApp LANGUAGES CXX)

# Application sources
file(GLOB_RECURSE APP_SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)
add_executable(Interstellar ${APP_SOURCES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "Source Files" FILES ${APP_SOURCES})

# Includes
target_include_directories(Interstellar PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/Engine/include
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/Engine/src>
)

# Link against Engine
target_link_libraries(Interstellar PRIVATE Engine)

# Copy assets
add_custom_command(TARGET Interstellar PRE_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/assets"
    "$<TARGET_FILE_DIR:Interstellar>/assets"
  COMMENT "Copying assets to output directory"
)