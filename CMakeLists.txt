###############################################################################
#                                                                             #
#                             CMake configuration                             #
#                                                                             #
###############################################################################

cmake_minimum_required( VERSION 3.11 )

if( ${CMAKE_VERSION} VERSION_LESS 3.12 )
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} )
endif(  )



###############################################################################
#                                                                             #
#                                   Toggles                                   #
#                                                                             #
###############################################################################

option( BUILD_STATIC_LIBS "Whether to build library as static or shared library" OFF )
option( BUILD_RELEASE "Whether to build for a debug or release profile" OFF)
option( CPPTOOLS_SKIP_TESTS "Whether or not to skip tests" OFF )

if ( ${BUILD_RELEASE} )
    set( CMAKE_BUILD_TYPE "Release" )
else(  )
    set( CMAKE_BUILD_TYPE "Debug" )
endif(  )

if( ${BUILD_STATIC_LIBS} )
    set( STATIC_OR_SHARED "STATIC" )
else(  )
    set( STATIC_OR_SHARED "SHARED" )
    set( BUILD_SHARED_LIBS TRUE )
endif(  )

if( WIN32 AND ( STATIC_OR_SHARED STREQUAL "SHARED" ) )
    set( CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON )
endif(  )

# Install target directory
if( CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT )
    set( CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" )
    message( STATUS "Overriding default install prefix to: ${CMAKE_INSTALL_PREFIX}" )
else(  )
    message( STATUS "Provided install prefix: ${CMAKE_INSTALL_PREFIX}" )
endif( )

if( NOT DEFINED CPPTOOLS_EXPORT_LOCATION )
    set( CPPTOOLS_EXPORT_LOCATION "${CMAKE_BINARY_DIR}/export" )
endif(  )
message( STATUS "Export location for export targets: ${CPPTOOLS_EXPORT_LOCATION}" )



###############################################################################
#                                                                             #
#                              Compiler options                               #
#                                                                             #
###############################################################################

set( CMAKE_CXX_STANDARD 20 )

if( UNIX )
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -g")
endif( )



###############################################################################
#                                                                             #
#                             Project definition                              #
#                                                                             #
###############################################################################

project( "CppTools"
    DESCRIPTION "A collection of handy C++ tools developed for use in other projects"
    LANGUAGES C CXX
)

set(CPPTOOLS_LIB_NAME "cpptools")

set( CPPTOOLS_SOURCE_FILES
    tools/cli/cli_command.hpp
    tools/cli/cli_input.hpp
    tools/cli/cli_menu.hpp
    tools/cli/cli_streams.cpp
    tools/cli/cli_streams.hpp
    tools/cli/command_sequence.hpp
    tools/cli/menu_command.hpp
    tools/container/tree_node.hpp
    tools/container/tree.hpp
    tools/exceptions/bad_syntax_error.cpp 
    tools/exceptions/bad_syntax_error.hpp 
    tools/exceptions/file_not_found_error.cpp
    tools/exceptions/file_not_found_error.hpp 
    tools/exceptions/index_out_of_bounds_error.cpp
    tools/exceptions/index_out_of_bounds_error.hpp
    tools/exceptions/make_basic_exception.hpp
    tools/exceptions/range_bounds_exceeded_error.cpp
    tools/exceptions/range_bounds_exceeded_error.hpp 
    tools/exceptions/unmatched_array_size_error.cpp
    tools/exceptions/unmatched_array_size_error.hpp
    tools/math/sine_generator.hpp
    tools/micro_shell/micro_shell_codes.hpp
    tools/micro_shell/micro_shell_command.hpp
    tools/micro_shell/micro_shell.hpp
    tools/oo/notifier.hpp
    tools/oo/interfaces/event_receiver.hpp
    tools/oo/interfaces/action_event_receiver.hpp
    tools/thread/worker.cpp
    tools/thread/worker.hpp
    tools/thread/interfaces/interruptible.hpp
    tools/utility/bitwise_enum_ops.hpp
    tools/utility/hash_combine.hpp
    tools/utility/map.hpp
    tools/utility/predicate.hpp
    tools/utility/ranges.hpp
    tools/utility/string.cpp
    tools/utility/string.hpp
    tools/utility/type.hpp
    tools/utility/type_impl/instance_counter.hpp
    tools/utility/type_impl/instance_counter.cpp
    tools/utility/type_impl/type_list.hpp
)

###############################################################################
#                                                                             #
#                             Target definitions                              #
#                                                                             #
###############################################################################

# Generate script file to copy headers
file( WRITE ${CMAKE_BINARY_DIR}/cp_${CPPTOOLS_LIB_NAME}_headers.cmake
"file( COPY \"${CMAKE_CURRENT_SOURCE_DIR}/tools\"
    DESTINATION \"${CPPTOOLS_EXPORT_LOCATION}/include\"
    FILES_MATCHING
    PATTERN \"*.h\"
    PATTERN \"*.hpp\"
    PATTERN \"*.hxx\"
)"
)

# Build target
add_library( "${CPPTOOLS_LIB_NAME}" ${STATIC_OR_SHARED}
    ${CPPTOOLS_SOURCE_FILES}
)

# Custom target to export lib
add_custom_target("export_${CPPTOOLS_LIB_NAME}_lib")
add_dependencies("export_${CPPTOOLS_LIB_NAME}_lib" ${CPPTOOLS_LIB_NAME})
add_custom_command(
    TARGET "export_${CPPTOOLS_LIB_NAME}_lib"
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${CPPTOOLS_LIB_NAME}> ${CPPTOOLS_EXPORT_LOCATION}/lib/
)

# Custom target to export core headers
add_custom_target("export_${CPPTOOLS_LIB_NAME}_headers")
add_custom_command(
    TARGET "export_${CPPTOOLS_LIB_NAME}_headers"
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_BINARY_DIR}/cp_${CPPTOOLS_LIB_NAME}_headers.cmake
)

# Build and run tests
if( !CPPTOOLS_SKIP_TESTS )
    enable_testing( )
    add_executable( "${CPPTOOLS_LIB_NAME}_build_tests"
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

    target_include_directories( "${CPPTOOLS_LIB_NAME}_build_tests" PUBLIC "${CMAKE_SOURCE_DIR}" )
    target_link_libraries( "${CPPTOOLS_LIB_NAME}_build_tests" "${CPPTOOLS_LIB_NAME}" )
    
    # Copy resource folder
    add_custom_command(
        TARGET "${CPPTOOLS_LIB_NAME}_build_tests"
        COMMENT "Copying resource folder"
        PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/resources ${CMAKE_BINARY_DIR}/resources
    )

    add_test(
        NAME "${CPPTOOLS_LIB_NAME}_run_tests"
        COMMAND "${CPPTOOLS_LIB_NAME}_build_tests"
    )
endif(  )

# Install targets
install(
    TARGETS "${CPPTOOLS_LIB_NAME}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/lib"
)

install(
    DIRECTORY "${CMAKE_SOURCE_DIR}/tools"
    DESTINATION "${CMAKE_INSTALL_PREFIX}/include"
    FILES_MATCHING 
    PATTERN "*.h"
    PATTERN "*.hpp"
    PATTERN "*.hxx"
)