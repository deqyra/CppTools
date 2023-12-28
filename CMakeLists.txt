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

option( BUILD_STATIC_LIBS "Whether to build library as static or shared library" ON )
option( BUILD_RELEASE "Whether to build for a debug or release profile" OFF )
option( CPPTOOLS_SKIP_TESTS "Whether or not to skip tests" OFF )

if( ${BUILD_RELEASE} )
    set( CMAKE_BUILD_TYPE "Release" )
else( )
    set( CMAKE_BUILD_TYPE "Debug" )
endif( )

if( ${BUILD_STATIC_LIBS} )
    set( STATIC_OR_SHARED "STATIC" )
else( )
    set( STATIC_OR_SHARED "SHARED" )
    set( BUILD_SHARED_LIBS TRUE )
endif( )

if( WIN32 AND ( STATIC_OR_SHARED STREQUAL "SHARED" ) )
    set( CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON )
endif( )

set( CMAKE_EXPORT_COMPILE_COMMAND ON )



################################################################################
#                                                                              #
#                             Project definition                               #
#                                                                              #
################################################################################

project( cpptools
    DESCRIPTION "C++ tools developed for use in other projects"
    LANGUAGES CXX
)

set( CMAKE_CXX_STANDARD 20 )



################################################################################
#                                                                              #
#                             Target definitions                               #
#                                                                              #
################################################################################

# Build target
add_library( cpptools ${STATIC_OR_SHARED}
    cpptools/_internal/debug_log.cpp
    cpptools/_internal/debug_log.hpp
    cpptools/_internal/debug_macros.hpp
    cpptools/_internal/utility_macros.hpp
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
    cpptools/container/iterator/iterator_adapter.hpp
    cpptools/container/tree/node.hpp
    cpptools/container/tree/traversal.hpp
    cpptools/container/tree/unsafe_tree.hpp
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
    cpptools/utility/merging_strategy.hpp
    cpptools/utility/predicate.hpp
    cpptools/utility/string.cpp
    cpptools/utility/string.hpp
    cpptools/utility/to_string.cpp
    cpptools/utility/to_string.hpp
    cpptools/utility/type_utils.hpp
    cpptools/utility/type_utils_impl/type_list.hpp
)

if( NOT ${BUILD_RELEASE} )
    target_compile_definitions( cpptools PUBLIC
        CPPTOOLS_ENABLE_DEBUG_MASTER_SWITCH=1
    )
endif( )

target_include_directories(
    cpptools PUBLIC
    ${CMAKE_SOURCE_DIR}
)

################################################################################
#                                                                              #
#                                 Unit tests                                   #
#                                                                              #
################################################################################

# Build and run tests
if( NOT CPPTOOLS_SKIP_TESTS )
    add_subdirectory(tests)
endif(  )

# Install targets
install(
    TARGETS cpptools
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
