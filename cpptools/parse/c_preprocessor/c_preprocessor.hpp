#ifndef CPPTOOLS__PARSE__C_PREPROCESSOR__C_PREPROCESSOR_HPP
#define CPPTOOLS__PARSE__C_PREPROCESSOR__C_PREPROCESSOR_HPP

#include <string>
#include <unordered_map>
#include <vector>

#include "../../tree.hpp"
#include "../token.hpp"
#include "c_preprocessor_token.hpp"

/// @brief Perform preprocessing operations on C / C++ source files.
class CPreprocessor
{
    public:
        using CPreprocessorToken = Token<CPreprocessorTokenType>;
        using IncludeLookupTable = std::unordered_map<std::string, std::string>;

        /// @param source Path to the file to parse.
        ///
        /// @exception If the source code contains invalid preprocessor syntax,
        /// the function will throw a BadSyntaxError.
        CPreprocessor(std::string source);

        /// @brief Parse source code.
        ///
        /// @param source Source code to parse.
        ///
        /// @exception If the source code contains invalid preprocessor syntax,
        /// the function will throw a BadSyntaxError.
        void parseSource(const std::string& source);

        /// @brief Recursively expand all macros present in the parsed source 
        /// code.
        void expandMacros();

        /// @brief Process conditional blocks, deleting those whose conditional
        /// statement evaluates to false, along with their content. This  
        /// effectively removes tokens from the parsing tree entirely. This 
        /// operation may expand macros if required.
        void evaluateConditionals();

        /// @brief Set the table of file names to look for upon processing 
        /// include directives.
        void setIncludeLookupTable(IncludeLookupTable lookupTable);

        /// @brief Add a path to a folder to look into for files to include.
        void addIncludePath(std::string folder);

        /// @brief Remove a registered include path.
        void removeIncludePath(std::string folder);

        /// @brief Process ALL include directives in the source. Calling 
        /// evaluateConditionals() beforehand might be appropriate in most 
        /// cases.
        /// For each include argument:
        /// - If an entry is present in the include lookup table for that 
        /// argument, the value of that entry will be used as the path to the
        /// file to include (see setIncludeLookupTable()).
        /// - Otherwise, the argument is directly interpreted as the path to the
        /// file, which is looked up in the include folders in the order in 
        /// which they were added (see addIncludePath()).
        ///
        /// @param parseIncludedSource Whether or not to parse the newly 
        /// included text.
        /// @param skipAngleBrackets Whether or not to skip include directives
        /// whose arguments is given between angle brackets. System headers will
        /// NOT be automatically looked up; make sure to provide appropriate
        /// include paths or a lookup table if you set this option to false (see
        /// respectively addIncludePath() and setIncludeLookupTable()).
        ///
        /// @exception If the file for an include directive could not be located
        /// according to the rules as explained above, the function will throw 
        /// a FileNotFoundError.
        void processIncludeDirectives(bool parseIncludedSource, bool skipAngleBrackets = true);

        /// @brief Get all tokens in the parsing tree, ordered in the way they
        /// were encountered in the source code.
        ///
        /// @return An array filled with the parsed tokens.
        std::vector<CPreprocessorToken> getTokens();

        /// @brief Get all tokens of a certain type in the parsing tree, ordered
        /// in the way they were encountered in the source code.
        ///
        /// @return An array filled with the parsed tokens of a given type.
        std::vector<CPreprocessorToken> getTokens(CPreprocessorTokenType type);

        /// @brief Remove tokens of a certain type from the parsing tree.
        void stripTokens(CPreprocessorTokenType type);

        /// @brief Get source code reflecting the current state of the parsing 
        /// tree. If the source code output from CPreprocessor instance A was to
        /// be parsed by CPreprocessor instance B, it would always be true that
        /// A.getTokens() == B.getTokens().
        ///
        /// @return A string whose content is representative of the current 
        /// state of the parsing tree.
        std::string getText();

        /// @brief Dump the output source code from getText() to a file.
        /// 
        /// @param folder Path to the folder in which to create the dump file.
        void dumpText(std::string folder);

    private:
        /// @brief Tokens parsed from the input source, organized in a tree.
        Tree<CPreprocessorToken> _tokens;

        /// @brief Structure mapping include argument values to the 
        /// correspongind file paths.
        IncludeLookupTable _includeLookupTable;

        /// @brief Array of paths to folders in which to look for files to 
        /// include.
        std::vector<std::string> _includePaths;
};

#endif//CPPTOOLS__PARSE__C_PREPROCESSOR__C_PREPROCESSOR_HPP