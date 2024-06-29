# Build target
add_library( cpptools
    _internal/assume.hpp
    _internal/debug_log.cpp
    _internal/debug_log.hpp
    _internal/debug_macros.hpp
    _internal/undef_debug_macros.hpp
    _internal/utility_macros.hpp
    cli/command.cpp
    cli/command.hpp
    cli/command_sequence.hpp
    cli/input.cpp
    cli/input.hpp
    cli/menu.hpp
    cli/menu_command.hpp
    cli/shell.hpp
    cli/streams.hpp
    container/tree.hpp
    container/tree/node.hpp
    container/tree/traversal.hpp
    container/tree/unsafe_tree.hpp
    exception/error_category.cpp
    exception/error_category.hpp 
    exception/exception.cpp
    exception/exception.hpp
    exception/internal_exception.hpp 
    exception/io_exception.cpp
    exception/io_exception.hpp
    exception/iterator_exception.hpp 
    exception/parameter_exception.cpp 
    exception/parameter_exception.hpp 
    exception/range_exception.hpp 
    exception/range_exception.hpp 
    math/sine_generator.hpp
    oo/notifier.hpp
    oo/interfaces/action_event_receiver.hpp
    oo/interfaces/event_receiver.hpp
    thread/worker.cpp
    thread/worker.hpp
    thread/interfaces/interruptible.hpp
    utility/bitwise_enum_ops.hpp
    utility/concepts.hpp
    utility/hash_combine.hpp
    utility/map.hpp
    utility/merge_strategy.hpp
    utility/predicate.hpp
    utility/string.cpp
    utility/string.hpp
    utility/to_string.cpp
    utility/to_string.hpp
)

if( ${CMAKE_BUILD_TYPE} STREQUAL Debug )
    target_compile_definitions( cpptools PUBLIC
        CPPTOOLS_ENABLE_DEBUG_MASTER_SWITCH=1
    )
endif( )

target_include_directories(
    cpptools PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

export( TARGETS cpptools )
