#ifndef BOND_SEMANTICANALYZER_H
#define BOND_SEMANTICANALYZER_H

#include "bond/allocator.h"
#include "bond/parseerror.h"
#include "bond/parsenodesfwd.h"

namespace Bond
{
class SymbolTable;

class SemanticAnalyzer
{
public:
	SemanticAnalyzer(Allocator &allocator):
		mAllocator(allocator),
		mSymbolTable(0)
	{}

	~SemanticAnalyzer();
	void Dispose();

	void Analyze(TranslationUnit *translationUnitList);

	bool HasErrors() const { return mErrorBuffer.HasErrors(); }
	const ParseErrorBuffer &GetErrorBuffer() const { return mErrorBuffer; }

	const SymbolTable *GetSymbolTable() const { return mSymbolTable; }

private:
	void PopulateSymbolTable(TranslationUnit *translationUnitList, Allocator &allocator);

	ParseErrorBuffer mErrorBuffer;
	Allocator &mAllocator;
	SymbolTable *mSymbolTable;
};

}

#endif
