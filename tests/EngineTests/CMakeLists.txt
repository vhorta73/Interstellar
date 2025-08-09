# tests/EngineTests/CMakeLists.txt
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

  # Put the exe next to other binaries
  set_target_properties(EngineTests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests/bin"
    FOLDER "Tests"
  )

  # Discover without running during build; run at test time instead
  gtest_discover_tests(EngineTests
    DISCOVERY_MODE PRE_TEST
    WORKING_DIRECTORY $<TARGET_FILE_DIR:EngineTests>
  )
else()
  message(STATUS "No EngineTests sources found; skipping EngineTests target")
endif()
