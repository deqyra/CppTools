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
    cpptools/cli/cli_streams.cpp
    cpptools/cli/cli_streams.hpp
    cpptools/cli/cli_input.hpp
    cpptools/cli/cli_command.hpp
    cpptools/cli/cli_menu.hpp
    cpptools/cli/command_sequence.hpp
    cpptools/cli/menu_command.hpp
    cpptools/container/tree.hpp
    cpptools/exceptions/make_basic_exception.hpp
    cpptools/exceptions/file_not_found_error.cpp
    cpptools/exceptions/file_not_found_error.hpp 
    cpptools/exceptions/index_out_of_bounds_error.cpp
    cpptools/exceptions/index_out_of_bounds_error.hpp
    cpptools/exceptions/range_bounds_exceeded_error.cpp
    cpptools/exceptions/range_bounds_exceeded_error.hpp 
    cpptools/exceptions/unmatched_array_size_error.cpp
    cpptools/exceptions/unmatched_array_size_error.hpp
    cpptools/exceptions/bad_syntax_error.cpp 
    cpptools/exceptions/bad_syntax_error.hpp 
    cpptools/oo/notifier.hpp
    cpptools/oo/interfaces/event_receiver.hpp
    cpptools/oo/interfaces/action_event_receiver.hpp
    cpptools/math/sine_generator.hpp
    cpptools/micro_shell/micro_shell.hpp
    cpptools/micro_shell/micro_shell_codes.hpp
    cpptools/micro_shell/micro_shell_command.hpp
    cpptools/thread/worker.cpp
    cpptools/thread/worker.hpp
    cpptools/thread/interfaces/interruptible.hpp
    cpptools/utility/hash_combine.hpp
    cpptools/utility/iterable_tools.hpp
    cpptools/utility/lambda_maker.hpp
    cpptools/utility/map_tools.hpp
    cpptools/utility/string_tools.cpp
    cpptools/utility/string_tools.hpp
    cpptools/utility/type_utils.hpp
    cpptools/utility/type_utils_impl/container_traits.hpp
    cpptools/utility/type_utils_impl/container_traits_detail.hpp
    cpptools/utility/type_utils_impl/instance_counter.hpp
    cpptools/utility/type_utils_impl/instance_counter.cpp
    cpptools/utility/type_utils_impl/type_list.hpp
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

target_include_directories(
    "${CPPTOOLS_LIB_NAME}" PUBLIC
    "${CMAKE_SOURCE_DIR}"
    "${CMAKE_SOURCE_DIR}/external/include"
)

# Build and run tests
if(NOT CPPTOOLS_SKIP_TESTS)
    enable_testing( )
    add_executable( "${CPPTOOLS_LIB_NAME}_tests"
        tests/catch2_custom_generators.hpp
        tests/debugging_tools.cpp
        tests/debugging_tools.hpp
        tests/main.cpp
        tests/cpptools/test_string_tools.cpp
        tests/cpptools/test_iterable_tools.cpp
        tests/cpptools/test_lambda_maker.cpp
        tests/cpptools/container/test_tree.cpp
        tests/cpptools/cli/cli_test_classes.cpp
        tests/cpptools/cli/cli_test_classes.hpp
        tests/cpptools/cli/test_cli_command_sequence.cpp
        tests/cpptools/cli/test_cli_input.cpp
        tests/cpptools/cli/test_cli_menu.cpp
        tests/cpptools/cli/test_cli_menu_command.cpp
        tests/cpptools/cli/test_cli_streams.cpp
        tests/cpptools/micro_shell/shell_test_classes.hpp
        tests/cpptools/micro_shell/test_micro_shell.cpp
    )

    target_include_directories(
        "${CPPTOOLS_LIB_NAME}_tests" PUBLIC
        "${CMAKE_SOURCE_DIR}"
        "${CMAKE_SOURCE_DIR}/external/include"
    )
    target_link_libraries( "${CPPTOOLS_LIB_NAME}_tests" "${CPPTOOLS_LIB_NAME}" )
    
    # Copy resource folder
    add_custom_command(
        TARGET "${CPPTOOLS_LIB_NAME}_tests"
        COMMENT "Copying resource folder"
        PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/resources ${CMAKE_BINARY_DIR}/resources
    )

    add_test(
        NAME "${CPPTOOLS_LIB_NAME}_run_tests"
        COMMAND "${CPPTOOLS_LIB_NAME}_tests"
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