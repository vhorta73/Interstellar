# tests/CMakeLists.txt
# (Top-level CMake already did include(CTest), so we don't repeat it here.)

# Add suites only if they exist
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/EngineTests/CMakeLists.txt")
  add_subdirectory(EngineTests)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/AppTests/CMakeLists.txt")
  add_subdirectory(AppTests)
endif()
