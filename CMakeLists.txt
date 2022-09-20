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
    cpptools/cli/command.cpp
    cpptools/cli/command.hpp
    cpptools/cli/command_sequence.hpp
    cpptools/cli/input.cpp
    cpptools/cli/input.hpp
    cpptools/cli/menu.hpp
    cpptools/cli/menu_command.hpp
    cpptools/cli/shell.hpp
    cpptools/cli/streams.hpp
    cpptools/container/tree.hpp
    cpptools/exception/error_category.cpp
    cpptools/exception/error_category.hpp 
    cpptools/exception/exception.cpp
    cpptools/exception/exception.hpp
    cpptools/exception/internal_exception.hpp 
    cpptools/exception/io_exception.cpp
    cpptools/exception/io_exception.hpp
    cpptools/exception/iterator_exception.hpp 
    cpptools/exception/parameter_exception.cpp 
    cpptools/exception/parameter_exception.hpp 
    cpptools/exception/range_exception.hpp 
    cpptools/exception/range_exception.hpp 
    cpptools/math/sine_generator.hpp
    cpptools/oo/notifier.hpp
    cpptools/oo/interfaces/action_event_receiver.hpp
    cpptools/oo/interfaces/event_receiver.hpp
    cpptools/thread/worker.cpp
    cpptools/thread/worker.hpp
    cpptools/thread/interfaces/interruptible.hpp
    cpptools/utility/bitwise_enum_ops.hpp
    cpptools/utility/concepts.hpp
    cpptools/utility/hash_combine.hpp
    cpptools/utility/map.hpp
    cpptools/utility/predicate.hpp
    cpptools/utility/string.cpp
    cpptools/utility/string.hpp
    cpptools/utility/type_utils.hpp
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
        tests/cli/test_classes.cpp
        tests/cli/test_classes.hpp
        tests/cli/test_command_sequence.cpp
        tests/cli/test_input.cpp
        tests/cli/test_menu.cpp
        tests/cli/test_menu_command.cpp
        tests/cli/test_shell.cpp
        tests/cli/test_streams.cpp
        tests/container/test_tree.cpp
        tests/utility/test_string.cpp
        tests/utility/test_predicate.cpp
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
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/tests/resources ${CMAKE_BINARY_DIR}/resources
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