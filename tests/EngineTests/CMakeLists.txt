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

  set_target_properties(EngineTests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests/bin"
    FOLDER "Tests"
  )

  gtest_discover_tests(EngineTests
    DISCOVERY_MODE PRE_TEST
    WORKING_DIRECTORY $<TARGET_FILE_DIR:EngineTests>
  )

  if(INSTALL_TESTS)
    install(TARGETS EngineTests RUNTIME DESTINATION tests/bin)
    if(MSVC)
      install(FILES "$<TARGET_PDB_FILE:EngineTests>" DESTINATION tests/bin OPTIONAL)
    endif()
  endif()
else()
  message(STATUS "No EngineTests sources found; skipping EngineTests target")
endif()
