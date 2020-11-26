# CMake configuration
cmake_minimum_required( VERSION 3.1...3.15 )

if( ${CMAKE_VERSION} VERSION_LESS 3.12 )
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} )
endif(  )

set( CMAKE_CXX_COMPILER_WORKS 1 )   # Skips the compiler test, which is platform-dependent
project( "CppTools"
    DESCRIPTION "A collection of C++ tools developed for use by deqyra's other projects"
    LANGUAGES C CXX
)

# Toggles
option( BUILD_STATIC_LIBS "Whether to build library as static or shared library" OFF )
option( BUILD_RELEASE "Whether to build for a debug or release profile" OFF)
option( CPPTOOLS_SKIP_TESTS "Whether or not to skip tests" OFF )

if ( ${BUILD_RELEASE} )
    set( CMAKE_BUILD_TYPE "Release" )
else(  )
    set( CMAKE_BUILD_TYPE "Debug" )
endif(  )

if( WIN32 )
    message( STATUS "Windows detected, forcing static libs." )
    set( BUILD_STATIC_LIBS ON )
endif(  )

if( ${BUILD_STATIC_LIBS} )
    set( STATIC_OR_SHARED "STATIC" )
else(  )
    set( STATIC_OR_SHARED "SHARED" )
endif(  )

# C++17 requires at least gcc 7 to be compiled
set( CMAKE_CXX_STANDARD 17 )

if( UNIX )
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -g")
endif( )
    
# Setup directories
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )
include_directories( ${CMAKE_SOURCE_DIR}/include )

# Project info
set( CPPTOOLS_HEADER_FILES
    "cpptools/enum_map.hpp"
    "cpptools/iterable_tools.hpp"
    "cpptools/lambda_maker.hpp"
    "cpptools/make_basic_exception.hpp"
    "cpptools/notifier.hpp"
    "cpptools/sine_generator.hpp"
    "cpptools/string_tools.hpp"
    "cpptools/tree.hpp"
    "cpptools/tree_node.hpp"
    "cpptools/cli/cli_streams.hpp"
    "cpptools/cli/cli_input.hpp"
    "cpptools/cli/cli_command.hpp"
    "cpptools/cli/cli_menu.hpp"
    "cpptools/cli/menu_command.hpp"
    "cpptools/cli/command_sequence.hpp"
    "cpptools/exceptions/bad_syntax_error.hpp "
    "cpptools/exceptions/file_not_found_error.hpp "
    "cpptools/exceptions/index_out_of_bounds_error.hpp"
    "cpptools/exceptions/range_bounds_exceeded_error.hpp "
    "cpptools/exceptions/unmatched_array_size_error.hpp"
    "cpptools/micro_shell/micro_shell.hpp"
    "cpptools/micro_shell/micro_shell_codes.hpp"
    "cpptools/micro_shell/micro_shell_command.hpp"
    "cpptools/parse/token.hpp"
    "cpptools/parse/c_preprocessor/c_preprocessor.hpp"
    "cpptools/parse/c_preprocessor/c_preprocessor_token_type.hpp"
)

set( CPPTOOLS_SOURCE_FILES
    "cpptools/string_tools.cpp"
    "cpptools/exceptions/index_out_of_bounds_error.cpp"
    "cpptools/exceptions/file_not_found_error.cpp"
    "cpptools/exceptions/range_bounds_exceeded_error.cpp"
    "cpptools/exceptions/unmatched_array_size_error.cpp"
    "cpptools/cli/cli_streams.cpp"
)

# Copy resource folder
    add_custom_target( "copy_resources" )
    add_custom_command( TARGET "copy_resources"
                        COMMENT "Copying resource folder"
                        POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/resources ${CMAKE_BINARY_DIR}/resources )

# Build project
    add_library( "cpptools" ${STATIC_OR_SHARED}
        ${CPPTOOLS_HEADER_FILES}
        ${CPPTOOLS_SOURCE_FILES}
    )

# Build and run tests
if( !CPPTOOLS_SKIP_TESTS )
    enable_testing( )
    add_executable( "cpptools_tests"
        tests/catch2_custom_generators.hpp
        tests/debugging_tools.cpp
        tests/debugging_tools.hpp
        tests/main.cpp
        tests/tools/test_string_tools.cpp
        tests/tools/test_iterable_tools.cpp
        tests/tools/test_lambda_maker.cpp
        tests/tools/cli/cli_test_classes.cpp
        tests/tools/cli/cli_test_classes.hpp
        tests/tools/cli/test_cli_command_sequence.cpp
        tests/tools/cli/test_cli_input.cpp
        tests/tools/cli/test_cli_menu.cpp
        tests/tools/cli/test_cli_menu_command.cpp
        tests/tools/cli/test_cli_streams.cpp
        tests/tools/micro_shell/shell_test_classes.hpp
        tests/tools/micro_shell/test_micro_shell.cpp
    )

    target_include_directories( "cpptools_tests" PUBLIC "${CMAKE_SOURCE_DIR}" )
    target_link_libraries( "cpptools_tests" "cpptools" )
    add_dependencies( "cpptools_tests" "copy_resources" )

    set( TESTS_RUNTIME_NAME "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/cpptools_tests" )
    if( WIN32 )
        set( TESTS_RUNTIME_NAME "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}/cpptools_tests.exe" )
    endif(  )

    add_test(
        NAME "tests"
        COMMAND "cpptools_tests"
    )

    add_custom_command(
        TARGET "cpptools_tests"
        COMMENT "Running tests"
        POST_BUILD
        COMMAND ${TESTS_RUNTIME_NAME}
    )
endif(  )

# Install
if( NOT DEFINED CPPTOOLS_EXTERNAL_INSTALL_PREFIX OR NOT ${CPPTOOLS_EXTERNAL_INSTALL_PREFIX} )
    set( CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" )
    message( STATUS "Using default install prefix: ${CMAKE_INSTALL_PREFIX}" )
else(  )
    message( STATUS "Provided install prefix: ${CMAKE_INSTALL_PREFIX}" )
endif( )

install(
    TARGETS "cpptools"
    LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cpptools"
)

install(
    DIRECTORY "${CMAKE_SOURCE_DIR}/cpptools"
    DESTINATION "${CMAKE_INSTALL_PREFIX}/include"
    FILES_MATCHING 
    PATTERN "*.h"
    PATTERN "*.hpp"
    PATTERN "*.hxx"
)