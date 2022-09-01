# cpptools
A collection of C++ tools.  

Currently made up of the following:
|                                       |                                                                                                                                                                                                |
|---------------------------------------|------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| [**Exceptions**][ex]                  | A collection of exception types and macros to throw them conveniently.                                                                                                                         |
| [**`tree`**][tree]                    | An arbitrary tree to store hierarchies of objects.                                                                                                                                             |
| [**`Notifier`**][notifier]            | A callback subscriber manager to help implementing observer patterns.                                                                                                                          |
| [**`LambdaMaker`**][lambda]           | A library of parametric lambda functions for common numerical comparisons.                                                                                                                     |
| [**`CLI`**][cli]                      | Convenient CLI menu builder. Bundled with basic building blocks and a (mostly failproof) input model. See my project [**PicrossEngine**][picross_cli] for an example usage.                    |
| [**`MicroShell`**][shell]             | Tiny shell emulator taking in user input to run registered commands. Works with the input model from [**`CLI`**][cli]. See my project [**PicrossEngine**][picross_shell] for an example usage. |
| [**`SineGenerator`**][sine]           | Get the real time value of a running sine of given frequency, pausing and resuming at will.                                                                                                    |
| [**`StringTools`**][string_tools]     | Basic string processing: tokenization, char removal, formatting iterables and so forth.                                                                                                        |
| [**`IterableTools`**][iter_tools]     | Common operations on iterables, numerical or otherwise: find maximum element, sum elements, ...                                                                                                |

See [**`list.todo`**][todo] for a quick overview of what is likely to be worked
on at present.

[ex]:               https://github.com/deqyra/CppTools/blob/master/cpptools/make_basic_exception.hpp
[tree]:             https://github.com/deqyra/CppTools/blob/master/cpptools/tree.hpp
[notifier]:         https://github.com/deqyra/CppTools/blob/master/cpptools/notifier.hpp
[lambda]:           https://github.com/deqyra/CppTools/blob/master/cpptools/lambda_maker.hpp
[cli]:              https://github.com/deqyra/CppTools/tree/master/cpptools/cli
[picross_cli]:      https://github.com/deqyra/PicrossEngine/blob/master/main.cpp#L48
[shell]:            https://github.com/deqyra/CppTools/tree/master/cpptools/micro_shell
[picross_shell]:    https://github.com/deqyra/PicrossEngine/blob/master/picross_cli/cli_modify_grid_command.cpp#L41
[sine]:             https://github.com/deqyra/CppTools/blob/master/cpptools/sine_generator.hpp
[string_tools]:     https://github.com/deqyra/CppTools/blob/master/cpptools/string_tools.hpp
[iter_tools]:       https://github.com/deqyra/CppTools/blob/master/cpptools/iter_tools.hpp
[todo]:             https://github.com/deqyra/CppTools/blob/master/list.todo