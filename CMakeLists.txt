################################################################################
#                                                                              #
#                             CMake configuration                              #
#                                                                              #
################################################################################

cmake_minimum_required(VERSION 3.24)



################################################################################
#                                                                              #
#                                   Toggles                                    #
#                                                                              #
################################################################################

option( CPPTOOLS_SKIP_TESTS "Whether or not to skip tests" OFF )

set( CMAKE_EXPORT_COMPILE_COMMANDS ON )



################################################################################
#                                                                              #
#                             Project definition                               #
#                                                                              #
################################################################################

project( cpptools
    DESCRIPTION "C++ tools developed for use in other projects"
    LANGUAGES CXX
    VERSION 0.1.4
)

message( VERBOSE "         cpptools v${CMAKE_PROJECT_VERSION}" )
message( VERBOSE "----------------------------------" )
message( VERBOSE "https://github.com/deqyra/CppTools" )

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



################################################################################
#                                                                              #
#                                Clang format                                  #
#                                                                              #
################################################################################

add_custom_target(
    format
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/scripts/run_clang_format.cmake
)