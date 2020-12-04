#include "c_preprocessor.hpp"

#include "../../string_tools.hpp"

CPreprocessor::CPreprocessor(std::string filename) :
    _filename(filename),
    _tokens()
{
	
}

void CPreprocessor::expandMacros()
{
	
}

void CPreprocessor::evaluateConditionals()
{
	
}

void CPreprocessor::setIncludeLookupTable(IncludeLookupTable lookupTable)
{
	
}

void CPreprocessor::addIncludePath(std::string folder)
{
	
}

void CPreprocessor::removeIncludePath(std::string folder)
{
	
}

void CPreprocessor::processIncludeDirectives(bool parseIncludedSource, bool skipAngleBrackets = true)
{
	
}

void CPreprocessor::stripTokens(CPreprocessorTokenType type)
{
	
}

std::string CPreprocessor::getText()
{
	
}

void CPreprocessor::dumpText(std::string folder)
{
	
}

void CPreprocessor::parseSource(const std::string& source)
{
	
}