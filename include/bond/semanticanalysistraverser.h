#ifndef BOND_SEMANTICANALYSISTRAVERSER_H
#define BOND_SEMANTICANALYSISTRAVERSER_H

#include "bond/autostack.h"
#include "bond/parsenodetraverser.h"
#include "bond/symboltable.h"

namespace Bond
{

class Allocator;
class ParseErrorBuffer;

typedef AutoStack<Symbol *> ScopeStack;

class SemanticAnalysisTraverser: protected ParseNodeTraverser
{
public:
	virtual ~SemanticAnalysisTraverser() {}

protected:
	SemanticAnalysisTraverser(
			ParseErrorBuffer &errorBuffer,
			Allocator &allocator,
			SymbolTable &symbolTable,
			ScopeStack &scopeStack):
		mErrorBuffer(errorBuffer),
		mSymbolTable(symbolTable),
		mAllocator(allocator),
		mScopeStack(scopeStack)
	{}

	SemanticAnalysisTraverser(const SemanticAnalysisTraverser &other):
		mErrorBuffer(other.mErrorBuffer),
		mSymbolTable(other.mSymbolTable),
		mAllocator(other.mAllocator),
		mScopeStack(other.mScopeStack)
	{}

	virtual void Visit(TranslationUnit *translationUnit);
	virtual void Visit(NamespaceDefinition *namespaceDefinition);
	virtual void Visit(StructDeclaration *structDeclaration);

	Symbol *GetCurrentScope() { return mScopeStack.GetTop(); }

	Symbol *InsertSymbol(Symbol::Type type, const Token *name, const ParseNode *definition, Symbol *parent);
	Symbol *GetOrInsertSymbol(Symbol::Type type, const Token *name, const ParseNode *definition, Symbol *parent);
	Symbol *CreateSymbol(Symbol::Type type, const Token *name, const ParseNode *definition, Symbol *parent);

	ParseErrorBuffer &mErrorBuffer;

private:
	SymbolTable &mSymbolTable;
	Allocator &mAllocator;
	ScopeStack &mScopeStack;
};

}

#endif
