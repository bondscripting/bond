#include "bond/autostack.h"
#include "bond/parsenodeutil.h"
#include "bond/parsenodetraverser.h"
#include "bond/semanticanalyzer.h"
#include <new>

namespace Bond
{

typedef AutoStack<Symbol *> ScopeStack;

class SemanticAnalysisPass: protected ParseNodeTraverser
{
public:
	virtual ~SemanticAnalysisPass() {}

	virtual void Analyze(TranslationUnit *translationUnitList);

protected:
	SemanticAnalysisPass(
			ParseErrorBuffer &errorBuffer,
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

	Symbol *GetSymbol(const Token *name);
	Symbol *GetSymbol(const QualifiedIdentifier *identifier);
	void InsertSymbol(Symbol *symbol);
	Symbol *GetOrInsertSymbol(Symbol *symbol);

	ParseErrorBuffer &mErrorBuffer;

private:
	SemanticAnalysisPass(const SemanticAnalysisPass &other);
	SemanticAnalysisPass &operator=(const SemanticAnalysisPass &other);

	Symbol *GetCurrentScope() { return mScopeStack.GetTop(); }
	void InsertSymbol(Symbol *parent, Symbol *symbol);
	Symbol *GetOrInsertSymbol(Symbol *parent, Symbol *symbol);

	ScopeStack mScopeStack;
	SymbolTable &mSymbolTable;
};


void SemanticAnalysisPass::Analyze(TranslationUnit *translationUnitList)
{
	ParseNodeTraverser::TraverseList(translationUnitList);
}


void SemanticAnalysisPass::Visit(TranslationUnit *translationUnit)
{
	ScopeStack::Element globalScopeElement(mScopeStack, mSymbolTable.GetGlobalScope());
	ParseNodeTraverser::Visit(translationUnit);
}


void SemanticAnalysisPass::Visit(NamespaceDefinition *namespaceDefinition)
{
	ScopeStack::Element stackElement(mScopeStack, namespaceDefinition);
	ParseNodeTraverser::Visit(namespaceDefinition);
}


void SemanticAnalysisPass::Visit(StructDeclaration *structDeclaration)
{
	ScopeStack::Element stackElement(mScopeStack, structDeclaration);
	ParseNodeTraverser::Visit(structDeclaration);
}


void SemanticAnalysisPass::Visit(FunctionDefinition *functionDefinition)
{
	//ScopeStack::Element stackElement(mScopeStack, functionDefinition);
	ParseNodeTraverser::Visit(functionDefinition);
}


void SemanticAnalysisPass::Visit(CompoundStatement *compoundStatement)
{
	//ScopeStack::Element stackElement(mScopeStack, compoundStatement);
	ParseNodeTraverser::Visit(compoundStatement);
}


void SemanticAnalysisPass::Visit(SwitchSection *switchSection)
{
	//ScopeStack::Element stackElement(mScopeStack, switchSection);
	ParseNodeTraverser::Visit(switchSection);
}


void SemanticAnalysisPass::Visit(ForStatement *forStatement)
{
	//ScopeStack::Element stackElement(mScopeStack, forStatement);
	ParseNodeTraverser::Visit(forStatement);
}


Symbol *SemanticAnalysisPass::GetSymbol(const Token *name)
{
	Symbol *parent = GetCurrentScope();
	return parent->FindSymbol(name);
}


Symbol *SemanticAnalysisPass::GetSymbol(const QualifiedIdentifier *identifier)
{
	Symbol *parent = GetCurrentScope();
	return parent->FindSymbol(identifier);
}


void SemanticAnalysisPass::InsertSymbol(Symbol *symbol)
{
	Symbol *parent = GetCurrentScope();
	InsertSymbol(parent, symbol);
}


void SemanticAnalysisPass::InsertSymbol(Symbol *parent, Symbol *symbol)
{
	Symbol *prev = parent->FindSymbol(symbol->GetName());

	if (prev != 0)
	{
		mErrorBuffer.PushError(ParseError::DUPLICATE_SYMBOL, symbol->GetName(), prev->GetName());
	}

	parent->InsertSymbol(symbol);
}


Symbol *SemanticAnalysisPass::GetOrInsertSymbol(Symbol *symbol)
{
	Symbol *parent = GetCurrentScope();
	return GetOrInsertSymbol(parent, symbol);
}


Symbol *SemanticAnalysisPass::GetOrInsertSymbol(Symbol *parent, Symbol *symbol)
{
	Symbol *target = parent->FindSymbol(symbol->GetName());

	if ((target != 0) && (target->GetSymbolType() != symbol->GetSymbolType()))
	{
		mErrorBuffer.PushError(ParseError::DUPLICATE_SYMBOL, target->GetName(), symbol->GetName());
		target = 0;
	}

	if (target == 0)
	{
		parent->InsertSymbol(symbol);
		target = symbol;
	}

	return symbol;
}

}


#include "semanticanalysis/toplevelsymbolpass.cpp"
#include "semanticanalysis/typeevaluationpass.cpp"
#include "semanticanalysis/valueevaluationpass.cpp"


namespace Bond
{

//------------------------------------------------------------------------------
// SemanticAnalyser
//------------------------------------------------------------------------------

void SemanticAnalyzer::Analyze(TranslationUnit *translationUnitList)
{
	mErrorBuffer.Reset();

	// Add all type declarations to the symbol table first, since they can be used prior to their declaration
	// in other typed declarations (e.g. function return type and parameter types).
	TypeAndConstantDeclarationPass typeAndConstantPass(mErrorBuffer, mSymbolTable);
	typeAndConstantPass.Analyze(translationUnitList);

	if (mErrorBuffer.HasErrors())
	{
		return;
	}

	TopLevelTypeEvaluationPass topLevelTypePass(mErrorBuffer, mSymbolTable);
	topLevelTypePass.Analyze(translationUnitList);

	if (mErrorBuffer.HasErrors())
	{
		return;
	}

	TopLevelValueEvaluationPass topLevelValuePass(mErrorBuffer, mSymbolTable);
	topLevelValuePass.Analyze(translationUnitList);

	if (mErrorBuffer.HasErrors())
	{
		return;
	}
}

}
