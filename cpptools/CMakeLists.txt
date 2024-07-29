include( GNUInstallDirs )
include( CMakePackageConfigHelpers )
set( INSTALL_CMAKE_DIR             "${CMAKE_INSTALL_LIBDIR}/cpptools/cmake" ) # this supports side-by-side multilib (x86 vs x64) setups for free so why not
set( CPPTOOLS_TEMPLATE_CONFIG_FILE "${CMAKE_CURRENT_LIST_DIR}/../cmake/cpptools-config.config.cmake" )
set( CPPTOOLS_OUTPUT_CONFIG_FILE   "${CMAKE_CURRENT_BINARY_DIR}/cpptools-config.cmake" )
set( CPPTOOLS_VERSION_FILE         "${CMAKE_CURRENT_BINARY_DIR}/cpptools-config-version.cmake" )

# Compile library
add_library( cpptools_objects OBJECT
    _internal/assume.hpp
    _internal/debug_log.hpp
    _internal/debug_macros.hpp
    _internal/undef_debug_macros.hpp
    _internal/utility_macros.hpp
    cli/argument_parsing.hpp
    cli/command.hpp
    cli/command_sequence.hpp
    cli/input.hpp
    cli/menu.hpp
    cli/menu_command.hpp
    cli/shell.hpp
    cli/streams.hpp
    container/tree.hpp
    container/tree/node.hpp
    container/tree/traversal.hpp
    container/tree/unsafe_tree.hpp
    exception/arg_parse_exception.hpp
    exception/error_category.hpp 
    exception/exception.hpp
    exception/internal_exception.hpp 
    exception/io_exception.hpp
    exception/iterator_exception.hpp 
    exception/parameter_exception.hpp 
    exception/lookup_exception.hpp 
    math/sine_generator.hpp
    thread/interruptible.hpp
    thread/worker.cpp
    thread/worker.hpp
    utility/bitwise_enum_ops.hpp
    utility/clamped_value.hpp
    utility/concepts.hpp
    utility/deduce_parameters.hpp
    utility/hash_combine.hpp
    utility/heterogenous_lookup.hpp
    utility/map.hpp
    utility/merge_strategy.hpp
    utility/monitored_value.hpp
    utility/predicate.hpp
    utility/ranges.hpp
    utility/string.cpp
    utility/string.hpp
    utility/to_string.hpp
    utility/type_traits.hpp
    utility/unary.hpp
    utility/wrapping_value.hpp
    utility/detail/allocator.hpp
    utility/detail/range_base.hpp
)

set( CPPTOOLS_ENABLE_DEBUG $<CONFIG:Debug,RelWithDebInfo> )

target_include_directories( cpptools_objects
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..> # usage from source tree
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>  # usage from install site
)

target_compile_definitions( cpptools_objects
    PUBLIC  CPPTOOLS_ENABLE_DEBUG_MASTER_SWITCH=${CPPTOOLS_ENABLE_DEBUG}
    PRIVATE CPPTOOLS_DLL_EXPORT=1
)

target_compile_options( cpptools_objects
    INTERFACE $<$<CXX_COMPILER_ID:MSVC>:"/wd4068;">
)

# Usage interface
add_library( cpptools_interface INTERFACE )

target_include_directories( cpptools_interface
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..> # usage from source tree
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>  # usage from install site
)

target_compile_definitions( cpptools_interface
    INTERFACE CPPTOOLS_ENABLE_DEBUG_MASTER_SWITCH=${CPPTOOLS_ENABLE_DEBUG}
)

target_compile_options( cpptools_interface
    INTERFACE $<$<CXX_COMPILER_ID:MSVC>:"/wd4068;">
)

# Static library
add_library( cpptools_static STATIC
    $<TARGET_OBJECTS:cpptools_objects>
)
target_link_libraries( cpptools_static
    PUBLIC cpptools_interface
)

# Shared library
add_library( cpptools_shared SHARED
    $<TARGET_OBJECTS:cpptools_objects>
)
target_link_libraries( cpptools_shared
    PUBLIC cpptools_interface
)
target_compile_definitions( cpptools_shared
    INTERFACE CPPTOOLS_DLL_IMPORT=1
)

# Install library
install(
    TARGETS
        cpptools_static
        cpptools_shared
        cpptools_interface
    EXPORT   cpptools-targets 
    LIBRARY  DESTINATION "${CMAKE_INSTALL_BINDIR}/$<CONFIG>"
    RUNTIME  DESTINATION "${CMAKE_INSTALL_BINDIR}/$<CONFIG>"
    ARCHIVE  DESTINATION "${CMAKE_INSTALL_LIBDIR}/$<CONFIG>"
)

# Install headers
install(
    DIRECTORY   "${CMAKE_CURRENT_SOURCE_DIR}"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    FILES_MATCHING
        PATTERN "*.hpp"
        PATTERN "*.config.hpp"      EXCLUDE
        PATTERN "_internal/config/" EXCLUDE
)

# Export targets
install( EXPORT cpptools-targets
    NAMESPACE cpptools::
    DESTINATION ${INSTALL_CMAKE_DIR}
)

# Package config
set( CPPTOOLS_STATIC_OR_SHARED static )
if ( ${BUILD_SHARED_LIBS} )
    set( CPPTOOLS_STATIC_OR_SHARED shared )
endif( )

# Configure config script
configure_package_config_file(
    ${CPPTOOLS_TEMPLATE_CONFIG_FILE} ${CPPTOOLS_OUTPUT_CONFIG_FILE}
    INSTALL_DESTINATION ${INSTALL_CMAKE_DIR}
)

# Versioning support
write_basic_package_version_file(
    ${CPPTOOLS_VERSION_FILE}
    COMPATIBILITY ExactVersion
)

# Install package config
install(
    FILES
        ${CPPTOOLS_VERSION_FILE}
        ${CPPTOOLS_OUTPUT_CONFIG_FILE}
    DESTINATION
        ${INSTALL_CMAKE_DIR}
)
