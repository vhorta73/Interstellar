file(GLOB_RECURSE APP_TEST_SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)
if(APP_TEST_SOURCES)
  add_executable(AppTests ${APP_TEST_SOURCES})
  target_include_directories(AppTests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/Interstellar/include
  )
  target_link_libraries(AppTests PRIVATE Engine gtest_main)
  add_test(NAME AppTests COMMAND AppTests)
else()
  message(STATUS "No AppTests sources found; skipping AppTests target")
endif()
