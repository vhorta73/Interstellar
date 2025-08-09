# tests/AppTests/CMakeLists.txt
include(GoogleTest)

file(GLOB_RECURSE APP_TEST_SOURCES src/*.cpp)
if(APP_TEST_SOURCES)
  add_executable(AppTests ${APP_TEST_SOURCES})
  target_include_directories(AppTests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/Interstellar/include
    ${CMAKE_SOURCE_DIR}/Engine/include
  )
  target_link_libraries(AppTests PRIVATE Engine gtest_main)
  set_target_properties(AppTests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests/bin"
    FOLDER "Tests"
  )
  gtest_discover_tests(AppTests
    DISCOVERY_MODE PRE_TEST
    WORKING_DIRECTORY $<TARGET_FILE_DIR:AppTests>
  )
  
  if (INSTALL_TESTS)
    install(TARGETS AppTests RUNTIME DESTINATION tests/bin)
    if (MSVC)
      install(FILES "$<TARGET_PDB_FILE:AppTests>" DESTINATION tests/bin OPTIONAL)
    endif()
  endif()

else()
  message(STATUS "No AppTests sources found; skipping AppTests target")
endif()
