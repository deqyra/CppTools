#ifndef TOOLS__MICRO_SHELL__MICRO_SHELL_HPP
#define TOOLS__MICRO_SHELL__MICRO_SHELL_HPP

#include <string>
#include <deque>
#include <memory>
#include <vector>
#include <algorithm>
#include "micro_shell_command.hpp"
#include "micro_shell_codes.hpp"
#include "../cli/cli_streams.hpp"
#include "../cli/cli_input.hpp"
#include "../string_tools.hpp"
#include "../exceptions/index_out_of_bounds_error.hpp"

template<typename CustomState>
class MicroShell
{
    using PicrossShellCommand = MicroShellCommand<CustomState>;
    using CommandPtr = std::shared_ptr<PicrossShellCommand>;
    using ChainIter = typename std::deque<CommandPtr>::iterator;

    private:    // Attributes
        // Command chain.
        std::deque<CommandPtr> _chain;
        // Custom exit command (can be nullptr).
        CommandPtr _exitCommand;

        // Lambda generating another lambda, which is parameterized on a name to check against.
        // The returned lambda tells whether the name of an input shell command is the same as the parameterized name.
        static constexpr auto _lambda_commandNameIs = [] (std::string nameToCheck)
        {
            return [nameToCheck] (CommandPtr c) -> bool
            {
                return c->name() == nameToCheck;
            };
        };

    public:     // Public methods
        MicroShell();

    // Command chain manipulation methods
        void clearCommands();
        void addCommand(CommandPtr command);
        void removeCommand(int index);
        void removeCommand(const std::string& name);
        void setExitCommand(CommandPtr command);

    // Getter methods
        CommandPtr getCommand(int index);
        CommandPtr getCommand(const std::string& name);
        CommandPtr getExitCommand();

    // Useful checks
        int indexOf(const std::string& name);
        bool hasCommand(const std::string& name);

        // Run the shell.
        int run(CustomState& state, CLIStreams& streams = CLIInput::defaultStreams);

    private:    // Private methods
        // Process user input.
        int processInput(const std::string& input, CustomState& state, CLIStreams& streams);
        // Generate a docstring descriptive of all contained commands.
        std::string globalHelpString();
        // Generate the docstring for a given command.
        std::string commandHelpString(const std::string& commandName);
        // Handle exit procedure.
        int handleExit(const std::string& command, CustomState& state, CLIStreams& streams);
};

template<typename CustomState>
MicroShell<CustomState>::MicroShell() :
    _chain(),
    _exitCommand(nullptr)
{

}

template<typename CustomState>
void MicroShell<CustomState>::addCommand(MicroShell<CustomState>::CommandPtr command)
{
    // Do not accept several commands with the same name, nor any command named 'exit' or 'help'.
    if (hasCommand(command->name()) || command->name() == EXIT_KEYWORD || command->name() == HELP_KEYWORD)
    {
        std::string s = "MicroShell<" + std::to_string(typeid(CustomState).name()) + ">: Command \"" + command->name() + "\" already exists in the shell and cannot be added.";
        throw std::invalid_argument(s.c_str());
    }

    _chain.push_back(command);
}

template<typename CustomState>
void MicroShell<CustomState>::removeCommand(const std::string& name)
{
    // Find the index of the command with the provided name.
    int index = indexOf(name);

    // Throw if not found.
    if (index == -1)
    {
        std::string s = "MicroShell<" + std::to_string(typeid(CustomState).name()) + ">: Command \"" + name + "\" is not part of the shell and cannot be removed.";
        throw std::invalid_argument(s.c_str());
    }

    // Remove command.
    removeCommand(index);
}

template<typename CustomState>
void MicroShell<CustomState>::removeCommand(int index)
{
    // Try to erase the command at provided index.
    if (_chain.begin() + index >= _chain.end())
    {
        std::string s = "MicroShell<" + std::to_string(typeid(CustomState).name()) + ">: Index " + std::to_string(index) + " is out of bounds, cannot remove command.";
        throw IndexOutOfBoundsError(s);
    }

    _chain.erase(_chain.begin() + index);
}

template<typename CustomState>
void MicroShell<CustomState>::clearCommands()
{
    _chain.clear();
}

template<typename CustomState>
int MicroShell<CustomState>::indexOf(const std::string& name)
{
    // Find a command with given name.
    MicroShell<CustomState>::ChainIter it = std::find_if(_chain.begin(), _chain.end(), _lambda_commandNameIs(name));

    // Calculate and return index, or -1 if not found.
    return it != _chain.end() ? (it - _chain.begin()) : -1;
}

template<typename CustomState>
bool MicroShell<CustomState>::hasCommand(const std::string& name)
{
    // Command exists if a valid index is found.
    return indexOf(name) != -1;
}

template<typename CustomState>
void MicroShell<CustomState>::setExitCommand(MicroShell<CustomState>::CommandPtr command)
{
    _exitCommand = command;
}

template<typename CustomState>
typename MicroShell<CustomState>::CommandPtr MicroShell<CustomState>::getCommand(int index)
{
    // Try to fetch the command at provided index.
    if (index >= _chain.size())
    {
        std::string s = "MicroShell<" + std::to_string(typeid(CustomState).name()) + ">: Index " + std::to_string(index) + " is out of bounds, cannot fetch command.";
        throw IndexOutOfBoundsError(s);
    }

    return _chain.at(index);
}

template<typename CustomState>
typename MicroShell<CustomState>::CommandPtr MicroShell<CustomState>::getCommand(const std::string& name)
{
    // Try to fetch the command at provided index.
    int index = indexOf(name);
    if (index == -1)
    {
        std::string s = "MicroShell<" + std::to_string(typeid(CustomState).name()) + ">: Command with name " + name + " is not part of the shell and cannot be fetched.";
        throw IndexOutOfBoundsError(s);
    }

    return getCommand(index);
}

template<typename CustomState>
typename MicroShell<CustomState>::CommandPtr MicroShell<CustomState>::getExitCommand()
{
    return _exitCommand;
}

template<typename CustomState>
int MicroShell<CustomState>::run(CustomState& state, CLIStreams& streams)
{
    // Run the shell: prompt the user repeatedly and interpret the commands that were entered.
    int shellCode;
    do
    {
        std::string command = CLIInput::askForInput<std::string>("$ ", streams);
        shellCode = processInput(command, state, streams);
    } while (shellCode != SHELL_EXIT);      // Exit when the proper signal is returned.

    return SHELL_EXIT;
}

template<typename CustomState>
int MicroShell<CustomState>::processInput(const std::string& input, CustomState& state, CLIStreams& streams)
{
    // Tokenise the string on spaces to extract the command and its arguments.
    std::vector<std::string> tokens = StringTools::tokenizeString(input, ' ', true);
    if (!tokens.size()) return SHELL_COMMAND_NOT_FOUND;

    // If help was requested, respond accordingly.
    if (tokens[0] == HELP_KEYWORD)
    {
        // If more than one token was extracted, the second one is
        // probably the command the user want to get help about.
        if (tokens.size() > 1)
        {
            streams.out() << commandHelpString(tokens[1]) << std::endl;;
        }
        // Otherwise, display an informative list of commands.
        else
        {
            streams.out() << globalHelpString() << std::endl;
        }

        return SHELL_COMMAND_SUCCESS;
    }

    // If exit was entered, handle the exit procedure.
    if (tokens[0] == EXIT_KEYWORD)
    {
        return handleExit(input, state, streams);
    }

    // Otherwise, search for a command sharing the name of the first token...
    int index = indexOf(tokens[0]);
    if (index == - 1)
    {
        streams.out() << tokens[0] << ": command not found.\n";
        return SHELL_COMMAND_NOT_FOUND;
    }

    // ...and run that command.
    try
    {
        return _chain[index]->processInput(input, state, streams);
    }
    catch(const std::exception& e)
    {
        // Informative error logging.
        streams.err() << "Exception thrown by command \"" + _chain[index]->name() + "\":\n";
        streams.err() << e.what() << '\n';
        streams.out() << "Warning shell state may be corrupted.\n";
        streams.out() << "Resuming normally..." << std::endl;
        return SHELL_COMMAND_FAILURE;
    }    
}

template<typename CustomState>
std::string MicroShell<CustomState>::globalHelpString()
{
    // List available commands.
    std::string s = "Available commands:\n";
    for (auto it = _chain.begin(); it != _chain.end(); it++)
    {
        s += "  - " + (*it)->name() + ": " + (*it)->description() + "\n";
    }
    s += "Type 'help <command>' to get help about one command.\n";

    return s;
}

template<typename CustomState>
std::string MicroShell<CustomState>::commandHelpString(const std::string& commandName)
{
    // Find command.
    int index = indexOf(commandName);

    std::string s;
    // Handle command not found.
    if (index == -1)
    {
        s = "Command '" + commandName + "' not found, help cannot be displayed.\n";
        return s;
    }

    // Get the docstring of the command which was requested.
    s = "'" + commandName + "' help:\n";
    s += _chain[index]->help();

    return s;
}

template<typename CustomState>
int MicroShell<CustomState>::handleExit(const std::string& command, CustomState& state, CLIStreams& streams)
{
    // If not exit command was provided, just exit immediately.
    if (!_exitCommand)
    {
        return SHELL_EXIT;
    }
    // Otherwise run the provided exit command.
    return _exitCommand->processInput(command, state, streams);
}

#endif//TOOLS__MICRO_SHELL__MICRO_SHELL_HPP