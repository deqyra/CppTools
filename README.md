# CppTools
A collection of C++ developed for my own use.  

Currently made of the following:
* [**Quick exception macros**][make_ex]: preprocessor macros to declare and 
    define exceptions on the fly. Used by all files in 
    [`tools/exceptions`][ex_folder].
* [**`Tree`**][tree]: a templated structure storing objects hierarchically.
    Development still underway, usage might be a bit clunky.
* [**`Notifier`**][notifier]: a callback subscriber manager whose purpose is to
    make it easier to implement observer patterns.
* [**`LambdaMaker`**][lambda]: a library of parametric lambda functions for
    common numerical comparisons (is greater than, between values, etc).
* [**`CPreprocessor`**][c_pre]: a basic implementation of a (probably not) 
    standard C preprocessor. Development ongoing.
* [**`CLI`**][cli]: sort of a CLI framework aiming at making CLI menus easily.
    Comes together with basic building blocks and a (mostly failproof) input 
    model. See my project [**PicrossEngine**][picross_cli] for an example usage.
* [**`MicroShell`**][shell]: sort of a shell emulator able to prompt the user 
    for commands which were registered beforehand. Complies to and works with
    the input model from [**`CLI`**][cli]. See my project 
    [**PicrossEngine**][picross_shell] for an example usage.
* [**`SineGenerator`**][sine]: instantly and efficiently get the real time 
    value of a running sine of a given frequency, pausing and resuming at will.
* [**`EnumMap`**][enum_map]: a specialization of `std::unordered_map` able to 
    take in enums as keys.
* [**`StringTools`**][string_tools]: a collection of tools related to string 
    processing: simple parsing, special char removal, tokenizing, helpful 
    checks, printing out iterables and so forth.
* [**`IterableTools`**][iter_tools]: a collection of tools to perform common 
    operations on iterables, numerical or otherwise: find maximum element,
    sum elements, ...

See [**`list.todo`**][todo] for a quick overview of what is likely to be worked
on at present.

[make_ex]:          https://github.com/deqyra/CppTools/blob/master/tools/make_basic_exception.hpp
[ex_folder]:        https://github.com/deqyra/CppTools/tree/master/tools/exceptions
[tree]:             https://github.com/deqyra/CppTools/blob/master/tools/tree.hpp
[notifier]:         https://github.com/deqyra/CppTools/blob/master/tools/notifier.hpp
[lambda]:           https://github.com/deqyra/CppTools/blob/master/tools/lambda_maker.hpp
[c_pre]:            https://github.com/deqyra/CppTools/blob/master/tools/parse/c_preprocessor/c_preprocessor.hpp
[cli]:              https://github.com/deqyra/CppTools/tree/master/tools/cli
[picross_cli]:      https://github.com/deqyra/PicrossEngine/blob/master/main.cpp#L48
[shell]:            https://github.com/deqyra/CppTools/tree/master/tools/micro_shell
[picross_shell]:    https://github.com/deqyra/PicrossEngine/blob/master/picross_cli/cli_modify_grid_command.cpp#L41
[sine]:             https://github.com/deqyra/CppTools/blob/master/tools/sine_generator.hpp
[enum_map]:         https://github.com/deqyra/CppTools/blob/master/tools/enum_map.hpp
[string_tools]:     https://github.com/deqyra/CppTools/blob/master/tools/string_tools.hpp
[iter_tools]:       https://github.com/deqyra/CppTools/blob/master/tools/iter_tools.hpp
[todo]:             https://github.com/deqyra/CppTools/blob/master/list.todo