#ifndef CPPTOOLS__PARSE__C_PREPROCESSOR__C_PREPROCESSOR_TOKEN_TYPE_HPP
#define CPPTOOLS__PARSE__C_PREPROCESSOR__C_PREPROCESSOR_TOKEN_TYPE_HPP

namespace CppTools
{

enum class CPreprocessorTokenType
{
    DirectiveDefine,
    DirectiveInclude,
    ConditionIf,
    ConditionElif,
    ConditionElse,
    ConditionEndif,
    OperatorDefined,
    OperatorNot,
    OperatorAnd,
    OperatorOr,
    OperatorBitwiseAnd,
    OperatorBitwiseOr,
    OperatorPlus,
    OperatorMinus,
    OperatorTimes,
    OperatorDivide,
    Macro,
    Comment,
    Text
};

}//namespace CppTools

#endif//CPPTOOLS__PARSE__C_PREPROCESSOR__C_PREPROCESSOR_TOKEN_TYPE_HPP