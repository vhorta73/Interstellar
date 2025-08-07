# tests/EngineTests/CMakeLists.txt
file(GLOB_RECURSE ENGINE_TEST_SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)
if(ENGINE_TEST_SOURCES)
  add_executable(EngineTests ${ENGINE_TEST_SOURCES})
  target_include_directories(EngineTests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/Engine/include
  )
  target_link_libraries(EngineTests PRIVATE Engine gtest_main)
  add_test(NAME EngineTests COMMAND EngineTests)
else()
  message(STATUS "No EngineTests sources found; skipping EngineTests target")
endif()
