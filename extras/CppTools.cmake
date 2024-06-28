#[[

This CMake module defines function cpptools_configure_debug_macros, which can be
used to tailor and import CppTools' debug macros in another project.

#]]

### cpptools_configure_debug_macros: configure CppTools' debug macros to make them importable in another project
### PROJECT_MACRO_NAME: string that all configured debug macros will start with
### OUTPUT_DIR:         path to put the configured debug headers in
function( cpptools_configure_debug_macros PROJECT_MACRO_NAME OUTPUT_DIR )
    set( CONFIGURE_DEBUG_PROJECT_MACRO ${PROJECT_MACRO_NAME} )
    configure_file( ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../cpptools/_internal/config/debug_macros.config.hpp       ${OUTPUT_DIR}/debug_macros.hpp )
    configure_file( ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../cpptools/_internal/config/undef_debug_macros.config.hpp ${OUTPUT_DIR}/undef_debug_macros.hpp )
    unset( CONFIGURE_DEBUG_PROJECT_MACRO )
endfunction( )