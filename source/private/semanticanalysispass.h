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

	using ParseNodeTraverser::Visit;
	virtual void Visit(TranslationUnit *translationUnit) override;
	virtual void Visit(NamespaceDefinition *namespaceDefinition) override;
	virtual void Visit(StructDeclaration *structDeclaration) override;
	virtual void Visit(FunctionDefinition *functionDefinition) override;
	virtual void Visit(CompoundStatement *compoundStatement) override;
	virtual void Visit(SwitchSection *switchSection) override;
	virtual void Visit(ForStatement *forStatement) override;

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
