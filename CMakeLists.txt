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

set( CMAKE_CXX_STANDARD 20 )



################################################################################
#                                                                              #
#                             Target definitions                               #
#                                                                              #
################################################################################

# Build target
add_library( cpptools
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
    cpptools/utility/merge_strategy.hpp
    cpptools/utility/predicate.hpp
    cpptools/utility/string.cpp
    cpptools/utility/string.hpp
    cpptools/utility/to_string.cpp
    cpptools/utility/to_string.hpp
)

if( NOT ${BUILD_RELEASE} )
    target_compile_definitions( cpptools PUBLIC
        CPPTOOLS_ENABLE_DEBUG_MASTER_SWITCH=1
    )
endif( )

target_include_directories(
    cpptools PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
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