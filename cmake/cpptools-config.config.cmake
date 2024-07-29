@PACKAGE_INIT@

include( "${CMAKE_CURRENT_LIST_DIR}/cpptools-targets.cmake" )

check_required_components( cpptools )

if( NOT TARGET cpptools::cpptools )
    add_library( cpptools::cpptools ALIAS cpptools::cpptools_@CPPTOOLS_STATIC_OR_SHARED@ )
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../extras")
