# CppTools
A collection of C++ developed for my own use.  

Currently made up of the following:
| Tool                                  | Description                                                                                                                                                                                                                               |
|---------------------------------------|-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| [**Quick exception macros**][make_ex] | Preprocessor macros to declare and define exceptions on the fly. Used by all files in [`cpptools/exceptions`][ex_folder].                                                                                                                 |
| [**`Tree`**][tree]                    | A templated structure storing objects hierarchically. Development still underway, usage might be a bit clunky.                                                                                                                            |
| [**`Notifier`**][notifier]            | A callback subscriber manager whose purpose is to make it easier to implement observer patterns.                                                                                                                                          |
| [**`LambdaMaker`**][lambda]           | A library of parametric lambda functions for common numerical comparisons (is greater than, between values, etc).                                                                                                                         |
| [**`CLI`**][cli]                      | Sort of a CLI framework aiming at making CLI menus easily. Comes together with basic building blocks and a (mostly failproof) input model. See my project [**PicrossEngine**][picross_cli] for an example usage.                          |
| [**`MicroShell`**][shell]             | Sort of a shell emulator able to prompt the user for commands which were registered beforehand. Complies to and works with the input model from [**`CLI`**][cli]. See my project [**PicrossEngine**][picross_shell] for an example usage. |
| [**`SineGenerator`**][sine]           | Instantly and efficiently get the real time value of a running sine of a given frequency, pausing and resuming at will.                                                                                                                   |
| [**`EnumMap`**][enum_map]             | A specialization of `std::unordered_map` able to take in enum literals as keys.                                                                                                                                                           |
| [**`StringTools`**][string_tools]     | A collection of tools related to string processing: simple parsing, special char removal, tokenizing, helpful checks, printing out iterables and so forth.                                                                                |
| [**`IterableTools`**][iter_tools]     | A collection of tools to perform common operations on iterables, numerical or otherwise: find maximum element, sum elements, ...                                                                                                          |

See [**`list.todo`**][todo] for a quick overview of what is likely to be worked
on at present.

[make_ex]:          https://github.com/deqyra/CppTools/blob/master/cpptools/make_basic_exception.hpp
[ex_folder]:        https://github.com/deqyra/CppTools/tree/master/cpptools/exceptions
[tree]:             https://github.com/deqyra/CppTools/blob/master/cpptools/tree.hpp
[notifier]:         https://github.com/deqyra/CppTools/blob/master/cpptools/notifier.hpp
[lambda]:           https://github.com/deqyra/CppTools/blob/master/cpptools/lambda_maker.hpp
[cli]:              https://github.com/deqyra/CppTools/tree/master/cpptools/cli
[picross_cli]:      https://github.com/deqyra/PicrossEngine/blob/master/main.cpp#L48
[shell]:            https://github.com/deqyra/CppTools/tree/master/cpptools/micro_shell
[picross_shell]:    https://github.com/deqyra/PicrossEngine/blob/master/picross_cli/cli_modify_grid_command.cpp#L41
[sine]:             https://github.com/deqyra/CppTools/blob/master/cpptools/sine_generator.hpp
[enum_map]:         https://github.com/deqyra/CppTools/blob/master/cpptools/enum_map.hpp
[string_tools]:     https://github.com/deqyra/CppTools/blob/master/cpptools/string_tools.hpp
[iter_tools]:       https://github.com/deqyra/CppTools/blob/master/cpptools/iter_tools.hpp
[todo]:             https://github.com/deqyra/CppTools/blob/master/list.todo