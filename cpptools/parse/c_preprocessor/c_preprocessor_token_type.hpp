#ifndef TOOLS__PARSE__C_PREPROCESSOR__C_PREPROCESSOR_TOKEN_TYPE_HPP
#define TOOLS__PARSE__C_PREPROCESSOR__C_PREPROCESSOR_TOKEN_TYPE_HPP

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
}

#endif//TOOLS__PARSE__C_PREPROCESSOR__C_PREPROCESSOR_TOKEN_TYPE_HPP