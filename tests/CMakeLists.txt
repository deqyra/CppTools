################################################################################
#                                                                              #
#                                 Unit tests                                   #
#                                                                              #
################################################################################


if( ${CMAKE_VERSION} VERSION_GREATER_EQUAL 3.24 )
    cmake_policy( SET CMP0135 OLD )
endif( )

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

include( FetchContent )

FetchContent_Declare(
    Catch2
    URL https://github.com/catchorg/Catch2/archive/refs/tags/v3.5.1.zip
)

FetchContent_MakeAvailable(Catch2)
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)

include( CTest )
include( Catch )
enable_testing( )

add_executable( cpptools_tests
    catch2_custom_generators.hpp
    debugging_tools.cpp
    debugging_tools.hpp
    cli/test_classes.cpp
    cli/test_classes.hpp
    cli/test_command_sequence.cpp
    cli/test_input.cpp
    cli/test_menu.cpp
    cli/test_menu_command.cpp
    cli/test_shell.cpp
    cli/test_streams.cpp
    container/stress_test_tree.cpp
    container/test_tree.cpp
    container/tree_test_utilities.cpp
    container/tree_test_utilities.hpp
    utility/test_string.cpp
    utility/test_predicate.cpp
    utility/test_ranges.cpp
)
target_include_directories( cpptools_tests PUBLIC
    ${CMAKE_SOURCE_DIR}
)
target_link_libraries( cpptools_tests
    cpptools_static Catch2::Catch2WithMain
)

catch_discover_tests(cpptools_tests
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/tests"
)
