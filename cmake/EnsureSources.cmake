# cmake/EnsureSources.cmake
include_guard(GLOBAL)

# interstellar_ensure_sources(<SOURCES_VAR> <placeholder_basename>)
# - If <SOURCES_VAR> is empty, creates a *real file* in the current binary dir
#   using file(GENERATE) during CMake's generate step (always present for the build).
# - Applies /wd4505 on MSVC just for that placeholder.
function(interstellar_ensure_sources SOURCES_VAR PLACEHOLDER_BASENAME)
  set(_sources "${${SOURCES_VAR}}")

  if(NOT _sources STREQUAL "")
    # Nothing to do; return list as-is
    set(${SOURCES_VAR} "${_sources}" PARENT_SCOPE)
    return()
  endif()

  # Ensure the binary dir exists (paranoid)
  file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

  # Use file(GENERATE) so the file is guaranteed to exist after the generate step
  set(_placeholder "${CMAKE_CURRENT_BINARY_DIR}/${PLACEHOLDER_BASENAME}_placeholder.cpp")
  file(GENERATE OUTPUT "${_placeholder}" CONTENT "// empty placeholder TU (intentionally blank)\n")

  # Do NOT mark as GENERATED; it is a plain source that already exists by build time
  if(MSVC)
    set_source_files_properties("${_placeholder}" PROPERTIES COMPILE_OPTIONS "/wd4505")
  endif()

  set(${SOURCES_VAR} "${_placeholder}" PARENT_SCOPE)
  message(STATUS "[EnsureSources] Injected placeholder: ${_placeholder}")
endfunction()
