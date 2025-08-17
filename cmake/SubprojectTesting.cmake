# cmake/SubprojectTesting.cmake
include_guard(GLOBAL)

function(add_subproject_tests)
  if(NOT BUILD_TESTING)
    return()
  endif()

  set(options)
  set(oneValueArgs TARGET NAME)
  # Allow callers to pass extra search roots (DIRS) and custom patterns
  set(multiValueArgs LIBS INCLUDES DEFINES LABELS PATTERNS DIRS)
  cmake_parse_arguments(SPT "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  if(NOT SPT_TARGET OR NOT SPT_NAME)
    message(FATAL_ERROR "add_subproject_tests requires TARGET and NAME")
  endif()

  if(NOT TARGET GTest::gtest_main)
    message(FATAL_ERROR "GTest::gtest_main not found; root must FetchContent_MakeAvailable(googletest) when BUILD_TESTING=ON.")
  endif()

  # ---- Candidate test roots ----
  set(_dirs)
  if(SPT_DIRS)
    list(APPEND _dirs ${SPT_DIRS})
  endif()
  # Common defaults (relative to this subproject)
  list(APPEND _dirs
    "${CMAKE_CURRENT_SOURCE_DIR}/tests"
    "${CMAKE_CURRENT_SOURCE_DIR}/test"
    "${CMAKE_CURRENT_SOURCE_DIR}/unit"
    "${CMAKE_CURRENT_SOURCE_DIR}/unittests"
  )

  # Keep only existing dirs
  set(_search_roots)
  foreach(d IN LISTS _dirs)
    if(EXISTS "${d}")
      list(APPEND _search_roots "${d}")
    endif()
  endforeach()

  if(_search_roots STREQUAL "")
    message(STATUS "[Tests] ${SPT_NAME}: no test directories found (looked for tests/, test/, unit/, unittests/). Skipping.")
    return()
  endif()

  # ---- Filename patterns ----
  set(_patterns ${SPT_PATTERNS})
  if(NOT _patterns)
    # Broad but reasonable defaults (cpp/cc/cxx)
    set(_patterns
      "*_test.cpp"  "*_test.cc"  "*_test.cxx"
      "*Tests.cpp"  "*Tests.cc"  "*Tests.cxx"
      "*Test.cpp"   "*Test.cc"   "*Test.cxx"
      "*.test.cpp"  "*.test.cc"  "*.test.cxx"
    )
  endif()

  # ---- Collect sources from all roots ----
  set(_test_sources)
  foreach(root IN LISTS _search_roots)
    foreach(p IN LISTS _patterns)
      file(GLOB_RECURSE _hit CONFIGURE_DEPENDS "${root}/${p}")
      list(APPEND _test_sources ${_hit})
    endforeach()
  endforeach()

  # Last-chance fallback: any C++ in the roots
  if(_test_sources STREQUAL "")
    foreach(root IN LISTS _search_roots)
      file(GLOB_RECURSE _fallback CONFIGURE_DEPENDS
        "${root}/*.cpp" "${root}/*.cc" "${root}/*.cxx")
      list(APPEND _test_sources ${_fallback})
    endforeach()
  endif()

  list(REMOVE_DUPLICATES _test_sources)
  list(LENGTH _test_sources _num)
  if(_num EQUAL 0)
    message(STATUS "[Tests] ${SPT_NAME}: found 0 test source(s) under: ${_search_roots}. Skipping.")
    return()
  else()
    message(STATUS "[Tests] ${SPT_NAME}: found ${_num} test source(s) under: ${_search_roots}")
  endif()

  # ---- Target ----
  set(_exe "${SPT_NAME}Tests")
  add_executable(${_exe} ${_test_sources})
  # Register this test exe in a global list so the root can make an aggregate build target.
  set_property(GLOBAL APPEND PROPERTY INTERSTELLAR_TEST_TARGETS "${_exe}")

  set_target_properties(${_exe} PROPERTIES FOLDER "Tests/${SPT_NAME}")
  target_compile_features(${_exe} PRIVATE cxx_std_20)

  if(MSVC)
    target_compile_options(${_exe} PRIVATE /permissive- /W4 $<$<BOOL:${WARNINGS_AS_ERRORS}>:/WX>)
  else()
    target_compile_options(${_exe} PRIVATE -Wall -Wextra -Wpedantic $<$<BOOL:${WARNINGS_AS_ERRORS}>:-Werror>)
  endif()

  target_link_libraries(${_exe} PRIVATE
    ${SPT_TARGET}
    GTest::gtest_main
    ${SPT_LIBS}
  )

  if(SPT_INCLUDES)
    target_include_directories(${_exe} PRIVATE ${SPT_INCLUDES})
  endif()
  if(SPT_DEFINES)
    target_compile_definitions(${_exe} PRIVATE ${SPT_DEFINES})
  endif()
  foreach(root IN LISTS _search_roots)
    if(EXISTS "${root}/include")
      target_include_directories(${_exe} PRIVATE "${root}/include")
    endif()
  endforeach()

  if(MSVC)
    set_target_properties(${_exe} PROPERTIES
      VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:${_exe}>")
  endif()

  # ---- Discovery ----
  include(GoogleTest)
  set(_labels "${SPT_NAME}")
  if(SPT_LABELS)
    list(APPEND _labels ${SPT_LABELS})
  endif()

  gtest_discover_tests(${_exe}
    WORKING_DIRECTORY "$<TARGET_FILE_DIR:${_exe}>"
    DISCOVERY_TIMEOUT 60
    DISCOVERY_MODE PRE_TEST
    PROPERTIES LABELS "${_labels}"
  )

  # Umbrella fallback test (ensures visibility even if discovery flakes)
  add_test(NAME ${_exe}.all COMMAND $<TARGET_FILE:${_exe}> --gtest_color=yes)
  set_tests_properties(${_exe}.all PROPERTIES
    LABELS "${_labels}"
    WORKING_DIRECTORY "$<TARGET_FILE_DIR:${_exe}>"
  )

  # ---- Install (optional) ----
  if(INSTALL_TESTS)
    include(GNUInstallDirs)
    install(TARGETS ${_exe}
      RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}/tests/bin
      ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/tests/bin
      LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/tests/bin
    )
  endif()
endfunction()
