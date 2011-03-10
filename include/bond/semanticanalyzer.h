#ifndef BOND_SEMANTICANALYZER_H
#define BOND_SEMANTICANALYZER_H

#include "bond/parseerror.h"
#include "bond/symboltable.h"

namespace Bond
{

class SymbolTable;

class SemanticAnalyzer
{
public:
	SemanticAnalyzer() {}
	~SemanticAnalyzer() {}

	void Analyze(TranslationUnit *translationUnitList);

	bool HasErrors() const { return mErrorBuffer.HasErrors(); }
	const ParseErrorBuffer &GetErrorBuffer() const { return mErrorBuffer; }

	const SymbolTable &GetSymbolTable() const { return mSymbolTable; }

private:

	ParseErrorBuffer mErrorBuffer;
	SymbolTable mSymbolTable;
};

}

#endif
