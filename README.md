# cpptools
A collection of C++ tools.  

Currently made up of the following:
| Module             | Component                                  | Summary                                                                                                                                                                                |
|--------------------|--------------------------------------------|----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| **CLI**            | [**`cli`**][cli]                           | Convenient CLI menu builder. Bundled with basic building blocks and a (mostly failproof) input model. See project [**PicrossEngine**][picross_cli] for an example usage.               |
|                    | [**`shell`**][shell]                       | Shell emulator taking in user input to run registered commands. Works with the input model from [**`cli`**][cli]. See project [**PicrossEngine**][picross_shell] for an example usage. |
| **Containers**     | [**`tree`**][tree]                         | An arbitrary tree to store hierarchies of objects. STL-compatible.                                                                                                                     |
| **Exceptions**     | [**`exception`**][ex]                      | An exception model and a collection of exception types with convenient macros to throw them.                                                                                           |
| **Math stuff**     | [**`sine_generator`**][sine]               | Get the real time value of a running sine of given frequency, with pausing and resuming at will.                                                                                       |
| **OO helpers**     | [**`notifier`**][notifier]                 | A callback subscriber manager to help implementing observer patterns.                                                                                                                  |
|                    | [**`event_receiver`**][event]              | Interface for an entity able to process events of a certain type.                                                                                                                      |
|                    | [**`action_event_receiver`**][action]      | Interface for an entity able to start and stop perfoming actions of a certain type.                                                                                                    |
| **Thread helpers** | [**`interruptible`**][interruptible]       | Interface for an entity able to run a threaded task and handle requests to pause, resume and stop.                                                                                     |
|                    | [**`worker`**][worker]                     | A proposed, all-purpose implementation of the **`interruptible`** interface.                                                                                                           |
| **Utilities**      | [**`bitwise_enum_ops`**][bitwise_enum_ops] | Scoped, concept-enabled operator overloads for bitwise operations on enum types.                                                                                                       |
|                    | [**`concepts`**][concepts]                 | A collection of concepts.                                                                                                                                                              |
|                    | [**`hash_combine`**][hash_combine]         | A janky implementation of a hash function to help defining an `std::hash` specialization for custom types. Best not to use for serious applications.                                   |
|                    | [**`map`**][map]                           | A collection of tools that apply to maps.                                                                                                                                              |
|                    | [**`predicate`**][predicate]               | A library of parametric lambda functions for common numerical comparisons and functional combinatorics.                                                                                |
|                    | [**`string`**][string]                     | Basic string processing: tokenization, char removal, formatting iterables and so forth.                                                                                                |
|                    | [**`type_utils`**][type_utils]             | Custom type traits.                                                                                                                                                                    |

See [**`list.todo`**][todo] for a quick overview of what is likely to be worked
on at present.

[cli]:              https://github.com/deqyra/CppTools/tree/master/cpptools/cli
[picross_cli]:      https://github.com/deqyra/PicrossEngine/blob/master/main.cpp#L48
[shell]:            https://github.com/deqyra/CppTools/tree/master/cpptools/cli/shell.hpp
[picross_shell]:    https://github.com/deqyra/PicrossEngine/blob/master/picross_cli/cli_modify_grid_command.cpp#L41
[tree]:             https://github.com/deqyra/CppTools/blob/master/cpptools/tree.hpp
[ex]:               https://github.com/deqyra/CppTools/blob/master/cpptools/exception/exception.hpp
[sine]:             https://github.com/deqyra/CppTools/blob/master/cpptools/sine_generator.hpp
[notifier]:         https://github.com/deqyra/CppTools/blob/master/cpptools/oo/notifier.hpp
[event]:            https://github.com/deqyra/CppTools/blob/master/cpptools/oo/interfaces/event_receiver.hpp
[action]:           https://github.com/deqyra/CppTools/blob/master/cpptools/oo/interfaces/action_event_receiver.hpp
[interruptible]:    https://github.com/deqyra/CppTools/blob/master/cpptools/thread/interfaces/interruptible.hpp
[worker]:           https://github.com/deqyra/CppTools/blob/master/cpptools/thread/worker.hpp
[bitwise_enum_ops]: https://github.com/deqyra/CppTools/blob/master/cpptools/utility/bitwise_enum_ops.hpp
[concepts]:         https://github.com/deqyra/CppTools/blob/master/cpptools/utility/concepts.hpp
[hash_combine]:     https://github.com/deqyra/CppTools/blob/master/cpptools/utility/hash_combine.hpp
[map]:              https://github.com/deqyra/CppTools/blob/master/cpptools/utility/map.hpp
[predicate]:        https://github.com/deqyra/CppTools/blob/master/cpptools/utility/predicate.hpp
[string]:           https://github.com/deqyra/CppTools/blob/master/cpptools/utility/string.hpp
[type_utils]:       https://github.com/deqyra/CppTools/blob/master/cpptools/utility/type_utils.hpp
[todo]:             https://github.com/deqyra/CppTools/blob/master/list.todo