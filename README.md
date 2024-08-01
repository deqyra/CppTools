# cpptools
A collection of C++ tools.  

Non-exhaustive summary of the cool stuff in there (ordered by decreasing coolness, to my own appreciation):
| Thing                                      | Summary                                                                                                                                                                                                                                        |
|--------------------------------------------|------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| [**`tree`**][tree]                         | An allocator-aware arbitrary tree to store hierarchies of objects. STL-compatible, comes with traversal utilities.                                                                                                                             |
| [**`ranges`**][ranges]                     | A collection of range adaptors.                                                                                                                                                                                                                |
| [**`exception`**][ex]                      | An exception model and a collection of exception types with convenient macros to throw them.                                                                                                                                                   |
| [Predicates][predicate]                    | A collection of predicate generators for common numerical comparisons and functional combinatorics.                                                                                                                                            |
| [**`bitwise_enum_ops`**][bitwise_enum_ops] | Scoped, concept-enabled operator overloads for bitwise operations on enum types.                                                                                                                                                               |
| [**`cli`**][cli]                           | Convenient CLI menu builder. Bundled with basic building blocks and a (mostly failproof) input model. See project [**PicrossEngine**][picross_cli] for an example usage. Also packs a **[command-line argument parsing facility][arg_parse]**. |
| [**`shell`**][shell]                       | Shell emulator taking in user input to run registered commands. Works with the input model from [**`cli`**][cli]. See project [**PicrossEngine**][picross_shell] for an example usage.                                                         |
| [**`interruptible`**][interruptible]       | Interface for an entity able to run a threaded task and handle requests to pause, resume and stop.                                                                                                                                             |
| [**`worker`**][worker]                     | A proposed, all-purpose implementation of the **`interruptible`** interface.                                                                                                                                                                   |
| [String toolbox][string]                   | Basic string processing: tokenization, char removal, etc.                                                                                                |

See [**`list.todo`**][todo] for a quick overview of what is likely to be worked
on at present.

### Troubleshooting

> **The unit tests for stuff in `cli` fail: _file "xxx" not found_.**  

Make sure you're running the test executable through CTest. If for some reason
you need to run the test executable manually, make sure that folder `resources`
(located in folder `tests`) is copied next to the unit tests executable. CMake
should be doing that as a post-build task though, so if you're encountering that
problem as part of building through CMake, please file an issue.

[tree]:             https://github.com/deqyra/CppTools/blob/master/cpptools/container
[ranges]:           https://github.com/deqyra/CppTools/blob/master/cpptools/utility/ranges.hpp
[ex]:               https://github.com/deqyra/CppTools/blob/master/cpptools/exception/exception.hpp
[cli]:              https://github.com/deqyra/CppTools/tree/master/cpptools/cli
[picross_cli]:      https://github.com/deqyra/PicrossEngine/blob/master/main.cpp#L48
[arg_parse]:        https://github.com/deqyra/CppTools/blob/master/cpptools/cli/argument_parsing.hpp
[shell]:            https://github.com/deqyra/CppTools/tree/master/cpptools/cli/shell.hpp
[picross_shell]:    https://github.com/deqyra/PicrossEngine/blob/master/picross_cli/cli_modify_grid_command.cpp#L41
[interruptible]:    https://github.com/deqyra/CppTools/blob/master/cpptools/thread/interfaces/interruptible.hpp
[worker]:           https://github.com/deqyra/CppTools/blob/master/cpptools/thread/worker.hpp
[bitwise_enum_ops]: https://github.com/deqyra/CppTools/blob/master/cpptools/utility/bitwise_enum_ops.hpp
[concepts]:         https://github.com/deqyra/CppTools/blob/master/cpptools/utility/concepts.hpp
[predicate]:        https://github.com/deqyra/CppTools/blob/master/cpptools/utility/predicate.hpp
[string]:           https://github.com/deqyra/CppTools/blob/master/cpptools/utility/string.hpp
[todo]:             https://github.com/deqyra/CppTools/blob/master/list.todo
