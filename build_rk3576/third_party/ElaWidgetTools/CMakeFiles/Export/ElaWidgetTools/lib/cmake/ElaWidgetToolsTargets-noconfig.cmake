#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Ela::WidgetTools" for configuration ""
set_property(TARGET Ela::WidgetTools APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(Ela::WidgetTools PROPERTIES
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/ElaWidgetTools/lib/libElaWidgetTools.so"
  IMPORTED_SONAME_NOCONFIG "libElaWidgetTools.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS Ela::WidgetTools )
list(APPEND _IMPORT_CHECK_FILES_FOR_Ela::WidgetTools "${_IMPORT_PREFIX}/ElaWidgetTools/lib/libElaWidgetTools.so" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
