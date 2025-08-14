# cmake/SubprojectTesting.cmake
include_guard(GLOBAL)

function(add_subproject_tests)
  if(NOT BUILD_TESTING)
    return()
  endif()

  set(options)
  set(oneValueArgs TARGET NAME)
  set(multiValueArgs LIBS INCLUDES DEFINES LABELS)
  cmake_parse_arguments(SPT "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  if(NOT SPT_TARGET OR NOT SPT_NAME)
    message(FATAL_ERROR "add_subproject_tests requires TARGET and NAME")
  endif()

  # Require googletest to be available (top-level should FetchContent it)
  if(NOT TARGET GTest::gtest_main)
    message(FATAL_ERROR
      "GTest::gtest_main not found. Ensure the root CMake lists FetchContent_MakeAvailable(googletest) when BUILD_TESTING=ON.")
  endif()

  # Collect sources under <subproj>/tests
  set(_tests_dir "${CMAKE_CURRENT_SOURCE_DIR}/tests")
  if(NOT EXISTS "${_tests_dir}")
    message(STATUS "[Tests] No tests/ folder for ${SPT_NAME}, skipping")
    return()
  endif()

  file(GLOB_RECURSE _test_sources CONFIGURE_DEPENDS
    "${_tests_dir}/*_test.cpp"
    "${_tests_dir}/*Tests.cpp"
    "${_tests_dir}/*.test.cpp"
  )

  if(_test_sources STREQUAL "")
    message(STATUS "[Tests] No test sources found in ${_tests_dir} for ${SPT_NAME}, skipping")
    return()
  endif()

  # ---- Target ----
  set(_exe "${SPT_NAME}Tests")
  add_executable(${_exe} ${_test_sources})
  set_target_properties(${_exe} PROPERTIES FOLDER "Tests/${SPT_NAME}")

  # C++ level for tests (match project standard; default to 20)
  target_compile_features(${_exe} PRIVATE cxx_std_20)

  # Warnings (respect global WARNINGS_AS_ERRORS if it exists)
  if(MSVC)
    target_compile_options(${_exe} PRIVATE /permissive- /W4 $<$<BOOL:${WARNINGS_AS_ERRORS}>:/WX>)
  else()
    target_compile_options(${_exe} PRIVATE -Wall -Wextra -Wpedantic $<$<BOOL:${WARNINGS_AS_ERRORS}>:-Werror>)
  endif()

  # Link to the subproject under test + gtest + optional libs
  target_link_libraries(${_exe} PRIVATE
    ${SPT_TARGET}
    GTest::gtest_main
    ${SPT_LIBS}
  )

  # Optional include dirs and compile defs just for tests
  if(SPT_INCLUDES)
    target_include_directories(${_exe} PRIVATE ${SPT_INCLUDES})
  endif()
  if(SPT_DEFINES)
    target_compile_definitions(${_exe} PRIVATE ${SPT_DEFINES})
  endif()

  # If tests/include exists, add it
  set(_tests_include "${_tests_dir}/include")
  if(EXISTS "${_tests_include}")
    target_include_directories(${_exe} PRIVATE "${_tests_include}")
  endif()

  # Nice VS experience
  if(MSVC)
    set_target_properties(${_exe} PROPERTIES
      VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:${_exe}>"
    )
  endif()

  # ---- CTest discovery ----
  include(GoogleTest)
  # Build a default label set: the suite name + any user labels
  set(_labels "${SPT_NAME}")
  if(SPT_LABELS)
    list(APPEND _labels ${SPT_LABELS})
  endif()
  gtest_discover_tests(${_exe}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    DISCOVERY_TIMEOUT 60
    PROPERTIES LABELS "${_labels}"
  )

  # ---- Optional: install test executables (works with your installed CTest index) ----
  if(INSTALL_TESTS)
    include(GNUInstallDirs)
    install(TARGETS ${_exe}
      RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}/tests/bin
      ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/tests/bin
      LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/tests/bin
    )
  endif()
endfunction()
