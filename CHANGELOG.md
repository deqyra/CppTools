**Note:** all C++ names mentioned hereafter are relative to namespace `tools`, file names are relative to folder `cpptools`.

# v1.1

### What's new

- [Range view library][ranges_v1.1], featuring:
    - `ranges::dereference_view`
    - `ranges::reverse_pair_view`
    - `ranges::reverse_map_pair_view`
    - `ranges::member_view`
    - For each of these views, CPOs acting as range adaptor objects and range adaptor closure objects in namespace `views`
- [Command-line argument parsing facility][arg_parse_v1.1] in namespace `cli`
    - New exception category `arg_parse_exception`
- New exception category `lookup_exception` (replaces inaptly named exception category `range_exception`)
- New functions `narrow` and `widen` in header [`utility/string.hpp`][string_v1.1]
- Minor adjustments:
    - Unit tests for (nearly) everything which didn't have them
    - Fixed wrong offset calculation in `contiguous_storage`
    - Fixed implementation of `to_string`
    - Moved various small function definitions into headers and made them `inline`
- Breaking changes:
    - Exception category `range_exception` was removed (in favor of new category `lookup_exception`)
    - Header `interruptible.hpp` was moved from directory `thread/interface` to directory `thread`
    - Header `allocator.hpp` was moved from directory `utility` to directory `utility/detail`
    - Removed namespace `cli::input`, all names that were declared in it are now in namespace `cli`
    - Removed namespace `string`, all names that were declared in it are now in namespace `::tools`
    - Concept `stringable` was moved from header `utility/concepts.hpp` to header `utility/to_string.hpp`
    - Class template `sine_generator` was moved out of namespace `math`
    - `clamped_range` and `wrapping_range` were renamed to `clamped_value` and `wrapping_value` respectively
    - Removed operator overloads on both `clamped_value` and `wrapping_value` which led to confusing semantics
    - Fixed incorrect arithmetic on both `clamped_value` and `wrapping_value`
    - Reordered constructor parameters of `monitored_value`
- CMake:
    - Fixed a warning flag being wrongly passed to MSVC, causing build to fail
    - Export CppTools CMake module as part of installed package, its path is automatically appended to CMake module paths when the installed package is imported

# v1.0

### Major features
| Thing                          | Description                                                                                                          | View source                                                           |
| ------------------------------ | -------------------------------------------------------------------------------------------------------------------- | --------------------------------------------------------------------- |
| `bitwise_enum_ops`             | Namespace containing concept-enabled operator overloads to perform bitwise operation on enums                        | [v1.0][bitwise_enum_ops_v1.0]   / [latest][bitwise_enum_ops_latest]   |
| `clamped_range`                | Wrapper for a value to be constrained in a certain range, clamping it to its bounds whenever they should be exceeded | [v1.0][clamped_range_v1.0]      / [latest][clamped_value_latest]      |
| `contiguous_storage`           | Wrapper that provides type-safe access to unitialized memory                                                         | [v1.0][contiguous_storage_v1.0] / [latest][contiguous_storage_latest] |
| `exception`                    | Extensible CRTP-based exception model with convenient macros to throw them                                           | [v1.0][exception_v1.0]          / [latest][exception_latest]          |
| `interruptible`                | Interface for an interruptible task-runner                                                                           | [v1.0][interruptible_v1.0]      / [latest][interruptible_latest]      |
| `monitored_value`              | Wrapping around a value, which raises a externally provided flag whenever the value is modified                      | [v1.0][monitored_value_v1.0]    / [latest][monitored_value_latest]    |
| Predicates                     | Collection of arithmetic and logic predicate generators                                                              | [v1.0][pred_v1.0]               / [latest][pred_latest]               |
| String toolbox                 | Collection of common operations to be performed on strings                                                           | [v1.0][string_v1.0]             / [latest][string_latest]             |
| `tree`                         | Arbitrary tree with complete allocator-awareness                                                                     | [v1.0][tree_v1.0]               / [latest][tree_latest]               |
| `worker`                       | Proposed implementation of `interruptible`                                                                           | [v1.0][worker_v1.0]             / [latest][worker_latest]             |
| `wrapping_range`               | Wrapper for a value to be constrained in a certain range, wrapping around whenever bounds should be exceeded         | [v1.0][wrapping_range_v1.0]     / [latest][wrapping_value_latest]     |

### Other features
| Thing                          | Description                                                                                                          | View source                                                           |
| ------------------------------ | -------------------------------------------------------------------------------------------------------------------- | --------------------------------------------------------------------- |
| Heterogenous lookup facilities | Collection of types which can be used to enable hetoregenous lookup on associative containers                        | [v1.0][heterogenous_v1.0]       / [latest][heterogenous_latest]       |
| Merge strategies               | Collection of operations describing how an object can be merged into another                                         | [v1.0][merge_v1.0]              / [latest][merge_latest]              |
| Parameter deduction facitilies | Functions to be used in unevaluated contexts to deduce in a tuple the argument types of another function             | [v1.0][deduce_parameters_v1.0]  / [latest][deduce_parameters_latest]  |
| Unary operations               | Collection of generic operations that can be handy to name rather than to write out                                  | [v1.0][unary_v1.0]              / [latest][unary_latest]              |

### CMake install capabilities
- Install to a path on your CMAKE_PREFIX_PATH list
- `find_package( cpptools 1.0 REQUIRED )` should succeed and expose three targets:
  - `cpptools::cpptools_static`, which can be statically linked against
  - `cpptools::cpptools_shared`, which can be dynamically linked against
  - `cpptools::cpptools`, an alias to `cpptools::cpptools_static`
- The installed package is relocatable.

Builds without warnings on Clang 18.1.8 and MSVC 19.40, in both debug and release modes.

[ranges_v1.1]:               https://github.com/deqyra/CppTools/tree/v1.1/cpptools/utility/ranges.hpp
[arg_parse_v1.1]:            https://github.com/deqyra/CppTools/tree/v1.1/cpptools/cli/argument_parsing.hpp
[string_v1.1]:               https://github.com/deqyra/CppTools/tree/v1.1/cpptools/utility/string.hpp

[bitwise_enum_ops_v1.0]:     https://github.com/deqyra/CppTools/tree/v1.0/cpptools/utility/bitwise_enum_ops.hpp
[clamped_range_v1.0]:        https://github.com/deqyra/CppTools/tree/v1.0/cpptools/utility/clamped_range.hpp
[contiguous_storage_v1.0]:   https://github.com/deqyra/CppTools/tree/v1.0/cpptools/memory/contiguous_storage.hpp
[exception_v1.0]:            https://github.com/deqyra/CppTools/tree/v1.0/cpptools/exception/exception.hpp
[heterogenous_v1.0]:         https://github.com/deqyra/CppTools/tree/v1.0/cpptools/utility/heterogenous.hpp
[interruptible_v1.0]:        https://github.com/deqyra/CppTools/tree/v1.0/cpptools/thread/interfaces/interruptible.hpp
[merge_v1.0]:                https://github.com/deqyra/CppTools/tree/v1.0/cpptools/utility/merge_strategy.hpp
[monitored_value_v1.0]:      https://github.com/deqyra/CppTools/tree/v1.0/cpptools/utility/monitored_value.hpp
[deduce_parameters_v1.0]:    https://github.com/deqyra/CppTools/tree/v1.0/cpptools/utility/deduce_parameters.hpp
[pred_v1.0]:                 https://github.com/deqyra/CppTools/tree/v1.0/cpptools/utility/predicate.hpp
[tree_v1.0]:                 https://github.com/deqyra/CppTools/tree/v1.0/cpptools/container/tree.hpp
[string_v1.0]:               https://github.com/deqyra/CppTools/tree/v1.0/cpptools/utility/string.hpp
[unary_v1.0]:                https://github.com/deqyra/CppTools/tree/v1.0/cpptools/utility/unary.hpp
[worker_v1.0]:               https://github.com/deqyra/CppTools/tree/v1.0/cpptools/thread/worker.hpp
[wrapping_range_v1.0]:       https://github.com/deqyra/CppTools/tree/v1.0/cpptools/utility/wrapping_range.hpp
[deduce_parameters_v1.0]:    https://github.com/deqyra/CppTools/tree/v1.0/cpptools/utility/deduce_parameters.hpp

[bitwise_enum_ops_latest]:   https://github.com/deqyra/CppTools/tree/master/cpptools/utility/bitwise_enum_ops.hpp
[clamped_value_latest]:      https://github.com/deqyra/CppTools/tree/master/cpptools/utility/clamped_value.hpp
[contiguous_storage_latest]: https://github.com/deqyra/CppTools/tree/master/cpptools/memory/contiguous_storage.hpp
[exception_latest]:          https://github.com/deqyra/CppTools/tree/master/cpptools/exception/exception.hpp
[heterogenous_latest]:       https://github.com/deqyra/CppTools/tree/master/cpptools/utility/heterogenous.hpp
[interruptible_latest]:      https://github.com/deqyra/CppTools/tree/master/cpptools/thread/interruptible.hpp
[merge_latest]:              https://github.com/deqyra/CppTools/tree/master/cpptools/utility/merge_strategy.hpp
[monitored_value_latest]:    https://github.com/deqyra/CppTools/tree/master/cpptools/utility/monitored_value.hpp
[deduce_parameters_latest]:  https://github.com/deqyra/CppTools/tree/master/cpptools/utility/deduce_parameters.hpp
[pred_latest]:               https://github.com/deqyra/CppTools/tree/master/cpptools/utility/predicate.hpp
[tree_latest]:               https://github.com/deqyra/CppTools/tree/master/cpptools/container/tree.hpp
[string_latest]:             https://github.com/deqyra/CppTools/tree/master/cpptools/utility/string.hpp
[unary_latest]:              https://github.com/deqyra/CppTools/tree/master/cpptools/utility/unary.hpp
[worker_latest]:             https://github.com/deqyra/CppTools/tree/master/cpptools/thread/worker.hpp
[wrapping_value_latest]:     https://github.com/deqyra/CppTools/tree/master/cpptools/utility/wrapping_value.hpp
[deduce_parameters_latest]:  https://github.com/deqyra/CppTools/tree/master/cpptools/utility/deduce_parameters.hpp
