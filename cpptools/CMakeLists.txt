# Compile library
add_library( cpptools_objects OBJECT
    _internal/assume.hpp
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

set_target_properties( cpptools_objects PROPERTIES
    WINDOWS_EXPORT_ALL_SYMBOLS ON
)
target_compile_definitions( cpptools_objects PRIVATE
    CPPTOOLS_DLL_EXPORT=1
)

# Usage interface
add_library( cpptools_interface INTERFACE )

if( ${CMAKE_BUILD_TYPE} STREQUAL Debug )
    target_compile_definitions( cpptools_interface INTERFACE
        CPPTOOLS_ENABLE_DEBUG_MASTER_SWITCH=1
    )
else( )
    target_compile_definitions( cpptools_interface INTERFACE
        CPPTOOLS_ENABLE_DEBUG_MASTER_SWITCH=0
    )
endif( )

# Static library
add_library( cpptools_static STATIC
    $<TARGET_OBJECTS:cpptools_objects>
)
target_link_libraries( cpptools_static PUBLIC
    cpptools_interface
)

# Shared library
add_library( cpptools_shared SHARED
    $<TARGET_OBJECTS:cpptools_objects>
)
target_link_libraries( cpptools_shared PUBLIC
    cpptools_interface
)
target_compile_definitions( cpptools_shared PUBLIC
    CPPTOOLS_DLL_IMPORT=1
)

# Install
include( GNUInstallDirs )

target_include_directories( cpptools_objects PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..> # usage from source tree
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>  # usage from install site
)

install( TARGETS cpptools_static cpptools_shared cpptools_interface
    EXPORT cpptools-config 
    LIBRARY DESTINATION "${CMAKE_INSTALL_BINDIR}/$<CONFIG>"
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}/$<CONFIG>"
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}/$<CONFIG>"
)

install( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.hpp"
    PATTERN "*.config.hpp" EXCLUDE
    PATTERN "_internal/config/" EXCLUDE
)

set( INSTALL_CMAKE_DIR ${CMAKE_INSTALL_LIBDIR}/cpptools/cmake ) # this supports side-by-side multilib (x86 vs x64) setups for free so why not

set( CPPTOOLS_STATIC_OR_SHARED static )
if ( ${BUILD_SHARED_LIBS} )
    set( CPPTOOLS_STATIC_OR_SHARED shared )
endif( )

set( CPPTOOLS_TEMPLATE_CONFIG ${CMAKE_CURRENT_LIST_DIR}/../cmake/cpptools-config.config.cmake )

include( CMakePackageConfigHelpers )
configure_package_config_file(${CPPTOOLS_TEMPLATE_CONFIG} cpptools-config.cmake INSTALL_DESTINATION ${INSTALL_CMAKE_DIR})

install( EXPORT cpptools-config
    NAMESPACE cpptools::
    DESTINATION ${INSTALL_CMAKE_DIR}
)

# Versioning support
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/cpptools-config-version.cmake
    COMPATIBILITY ExactVersion
)

install( FILES ${CMAKE_CURRENT_BINARY_DIR}/cpptools-config-version.cmake
    DESTINATION ${INSTALL_CMAKE_DIR}
)
