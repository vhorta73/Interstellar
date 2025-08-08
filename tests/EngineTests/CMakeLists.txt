# tests/AppTests/CMakeLists.txt

include(GoogleTest)

file(GLOB_RECURSE ENGINE_TEST_SOURCES src/*.cpp)
if(ENGINE_TEST_SOURCES)
  add_executable(EngineTests ${ENGINE_TEST_SOURCES})
  target_include_directories(EngineTests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/Interstellar/include
    ${CMAKE_SOURCE_DIR}/Engine/include
  )
  target_link_libraries(EngineTests PRIVATE Engine gtest_main)
  gtest_discover_tests(EngineTests
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    PROPERTIES LABELS "EngineTests"
  )
else()
  message(STATUS "No EngineTests sources found; skipping EngineTests target")
endif()




