# cpptools
A collection of C++ tools.  

Non-exhaustive summary of the cool stuff in there:
| Thing                                      | Summary                                                                                                                                                                                |
|--------------------------------------------|----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| [**`tree`**][tree]                         | An allocator-aware arbitrary tree to store hierarchies of objects. STL-compatible, comes with traversal utilities.                                                                     |
| [**`exception`**][ex]                      | An exception model and a collection of exception types with convenient macros to throw them.                                                                                           |
| [**`cli`**][cli]                           | Convenient CLI menu builder. Bundled with basic building blocks and a (mostly failproof) input model. See project [**PicrossEngine**][picross_cli] for an example usage.               |
| [**`shell`**][shell]                       | Shell emulator taking in user input to run registered commands. Works with the input model from [**`cli`**][cli]. See project [**PicrossEngine**][picross_shell] for an example usage. |
| [**`interruptible`**][interruptible]       | Interface for an entity able to run a threaded task and handle requests to pause, resume and stop.                                                                                     |
| [**`worker`**][worker]                     | A proposed, all-purpose implementation of the **`interruptible`** interface.                                                                                                           |
| [**`bitwise_enum_ops`**][bitwise_enum_ops] | Scoped, concept-enabled operator overloads for bitwise operations on enum types.                                                                                                       |
| [**`concepts`**][concepts]                 | A collection of concepts.                                                                                                                                                              |
| [**`predicate`**][predicate]               | A library of parametric lambda functions for common numerical comparisons and functional combinatorics.                                                                                |
| [**`string`**][string]                     | Basic string processing: tokenization, char removal, etc.                                                                                                |

See [**`list.todo`**][todo] for a quick overview of what is likely to be worked
on at present.

[cli]:              https://github.com/deqyra/CppTools/tree/master/cpptools/cli
[picross_cli]:      https://github.com/deqyra/PicrossEngine/blob/master/main.cpp#L48
[shell]:            https://github.com/deqyra/CppTools/tree/master/cpptools/cli/shell.hpp
[picross_shell]:    https://github.com/deqyra/PicrossEngine/blob/master/picross_cli/cli_modify_grid_command.cpp#L41
[tree]:             https://github.com/deqyra/CppTools/blob/master/cpptools/container
[ex]:               https://github.com/deqyra/CppTools/blob/master/cpptools/exception/exception.hpp
[sine]:             https://github.com/deqyra/CppTools/blob/master/cpptools/math/sine_generator.hpp
[interruptible]:    https://github.com/deqyra/CppTools/blob/master/cpptools/thread/interfaces/interruptible.hpp
[worker]:           https://github.com/deqyra/CppTools/blob/master/cpptools/thread/worker.hpp
[bitwise_enum_ops]: https://github.com/deqyra/CppTools/blob/master/cpptools/utility/bitwise_enum_ops.hpp
[concepts]:         https://github.com/deqyra/CppTools/blob/master/cpptools/utility/concepts.hpp
[predicate]:        https://github.com/deqyra/CppTools/blob/master/cpptools/utility/predicate.hpp
[string]:           https://github.com/deqyra/CppTools/blob/master/cpptools/utility/string.hpp
[todo]:             https://github.com/deqyra/CppTools/blob/master/list.todo
