#ifndef PRIVATE_BOND_SEMANTICANALYSISPASS_H
#define PRIVATE_BOND_SEMANTICANALYSISPASS_H

#include "bond/compiler/parsenodetraverser.h"
#include "bond/stl/autostack.h"

namespace Bond
{

class CompilerErrorBuffer;
class SymbolTable;
class Token;
typedef AutoStack<Symbol *> ScopeStack;

class SemanticAnalysisPass: protected ParseNodeTraverser
{
public:
	virtual ~SemanticAnalysisPass() {}

	virtual void Analyze(TranslationUnit *translationUnitList);

protected:
	SemanticAnalysisPass(
			CompilerErrorBuffer &errorBuffer,
			SymbolTable &symbolTable):
		mErrorBuffer(errorBuffer),
		mSymbolTable(symbolTable)
	{}

	virtual void Visit(TranslationUnit *translationUnit);
	virtual void Visit(NamespaceDefinition *namespaceDefinition);
	virtual void Visit(StructDeclaration *structDeclaration);
	virtual void Visit(FunctionDefinition *functionDefinition);
	virtual void Visit(CompoundStatement *compoundStatement);
	virtual void Visit(SwitchSection *switchSection);
	virtual void Visit(ForStatement *forStatement);

	Symbol *GetCurrentScope() { return mScopeStack.GetTop(); }

	Symbol *GetSymbol(const Token *name);
	Symbol *GetSymbol(const QualifiedIdentifier *identifier);
	void InsertSymbol(Symbol *symbol);
	Symbol *GetOrInsertSymbol(Symbol *symbol);

	CompilerErrorBuffer &mErrorBuffer;

private:
	SemanticAnalysisPass(const SemanticAnalysisPass &other);
	SemanticAnalysisPass &operator=(const SemanticAnalysisPass &other);

	void InsertSymbol(Symbol *parent, Symbol *symbol);
	Symbol *GetOrInsertSymbol(Symbol *parent, Symbol *symbol);

	ScopeStack mScopeStack;
	SymbolTable &mSymbolTable;
};

}

#endif
