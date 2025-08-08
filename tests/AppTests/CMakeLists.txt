
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
  gtest_discover_tests(AppTests
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    PROPERTIES LABELS "AppTests"
  )
else()
  message(STATUS "No AppTests sources found; skipping AppTests target")
endif()