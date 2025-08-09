# Added by top-level: include(CTest) and googletest already provided
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/EngineTests/CMakeLists.txt")
  add_subdirectory(EngineTests)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/AppTests/CMakeLists.txt")
  add_subdirectory(AppTests)
endif()
