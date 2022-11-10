#include "private/semanticanalysispass.h"
#include "bond/compiler/compilererror.h"
#include "bond/compiler/symboltable.h"

namespace Bond
{

void SemanticAnalysisPass::Analyze(TranslationUnit *translationUnitList)
{
	TraverseList(translationUnitList);
}


void SemanticAnalysisPass::Visit(TranslationUnit *translationUnit)
{
	auto globalScopeElement = mScopeStack.Push(mSymbolTable.GetGlobalScope());
	ParseNodeTraverser::Visit(translationUnit);
}


void SemanticAnalysisPass::Visit(NamespaceDefinition *namespaceDefinition)
{
	Symbol *scope = GetOrInsertSymbol(namespaceDefinition);
	auto stackElement = mScopeStack.Push(scope);
	ParseNodeTraverser::Visit(namespaceDefinition);
}


void SemanticAnalysisPass::Visit(StructDeclaration *structDeclaration)
{
	auto stackElement = mScopeStack.Push(structDeclaration);
	ParseNodeTraverser::Visit(structDeclaration);
}


void SemanticAnalysisPass::Visit(FunctionDefinition *functionDefinition)
{
	auto stackElement = mScopeStack.Push(functionDefinition);
	ParseNodeTraverser::Visit(functionDefinition);
}


void SemanticAnalysisPass::Visit(CompoundStatement *compoundStatement)
{
	auto stackElement = mScopeStack.Push(compoundStatement);
	ParseNodeTraverser::Visit(compoundStatement);
}


void SemanticAnalysisPass::Visit(SwitchSection *switchSection)
{
	auto stackElement = mScopeStack.Push(switchSection);
	ParseNodeTraverser::Visit(switchSection);
}


void SemanticAnalysisPass::Visit(ForStatement *forStatement)
{
	auto stackElement = mScopeStack.Push(forStatement);
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

	if (prev != nullptr)
	{
		mErrorBuffer.PushError(CompilerError::DUPLICATE_SYMBOL, symbol->GetName(), prev->GetName());
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

	if ((target != nullptr) && (target->GetSymbolType() != symbol->GetSymbolType()))
	{
		mErrorBuffer.PushError(CompilerError::DUPLICATE_SYMBOL, target->GetName(), symbol->GetName());
		target = nullptr;
	}

	if (target == nullptr)
	{
		parent->InsertSymbol(symbol);
		target = symbol;
	}

	return target;
}

}
