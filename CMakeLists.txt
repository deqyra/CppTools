################################################################################
#                                                                              #
#                             CMake configuration                              #
#                                                                              #
################################################################################

cmake_minimum_required(VERSION 3.11)

################################################################################
#                                                                              #
#                                   Toggles                                    #
#                                                                              #
################################################################################

option( CPPTOOLS_SKIP_TESTS "Whether or not to skip tests" OFF )

if( WIN32 AND BUILD_SHARED_LIBS )
    set( CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON )
endif( )

set( CMAKE_EXPORT_COMPILE_COMMANDS ON )



################################################################################
#                                                                              #
#                             Project definition                               #
#                                                                              #
################################################################################

project( cpptools
    DESCRIPTION "C++ tools developed for use in other projects"
    LANGUAGES CXX
)

set( CMAKE_CXX_STANDARD 23 )



################################################################################
#                                                                              #
#                                  cpptools                                    #
#                                                                              #
################################################################################

include( extras/CppTools.cmake )
cpptools_configure_debug_macros( CPPTOOLS ${CMAKE_CURRENT_SOURCE_DIR}/cpptools/_internal )

add_subdirectory( cpptools )

################################################################################
#                                                                              #
#                                 Unit tests                                   #
#                                                                              #
################################################################################

# Build and run tests
if( NOT CPPTOOLS_SKIP_TESTS )
    add_subdirectory( tests )
endif(  )

# Install targets
install(
    TARGETS cpptools
    LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/lib"
)

install(
    DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/tools"
    DESTINATION "${CMAKE_INSTALL_PREFIX}/include"
    FILES_MATCHING 
    PATTERN "*.h"
    PATTERN "*.hpp"
    PATTERN "*.hxx"
)

################################################################################
#                                                                              #
#                                 Unit tests                                   #
#                                                                              #
################################################################################

add_custom_target(
    format
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/scripts/run_clang_format.cmake
)