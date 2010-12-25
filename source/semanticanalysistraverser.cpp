#include "bond/allocator.h"
#include "bond/parseerror.h"
#include "bond/parsenodes.h"
#include "bond/semanticanalysistraverser.h"
#include <new>

namespace Bond
{

void SemanticAnalysisTraverser::Visit(TranslationUnit *translationUnit)
{
	ScopeStack::Element globalScopeElement(mScopeStack, mSymbolTable.GetGlobalScope());
	ParseNodeTraverser::Visit(translationUnit);
}


void SemanticAnalysisTraverser::Visit(NamespaceDefinition *namespaceDefinition)
{
	ScopeStack::Element stackElement(mScopeStack, namespaceDefinition->GetSymbol());
	ParseNodeTraverser::Visit(namespaceDefinition);
}


void SemanticAnalysisTraverser::Visit(StructDeclaration *structDeclaration)
{
	ScopeStack::Element stackElement(mScopeStack, structDeclaration->GetSymbol());
	ParseNodeTraverser::Visit(structDeclaration);
}


Symbol *SemanticAnalysisTraverser::InsertSymbol(Symbol::Type type, const Token *name, const ParseNode *definition, Symbol *parent)
{
	Symbol *symbol = parent->FindSymbol(name);

	if (symbol != 0)
	{
		mErrorBuffer.PushError(ParseError::DUPLICATE_SYMBOL, name, symbol->GetName());
	}

	symbol = CreateSymbol(type, name, definition, parent);
	parent->InsertSymbol(symbol);

	return symbol;
}


Symbol *SemanticAnalysisTraverser::GetOrInsertSymbol(Symbol::Type type, const Token *name, const ParseNode *definition, Symbol *parent)
{
	Symbol *symbol = parent->FindSymbol(name);

	if ((symbol != 0) && (symbol->GetType() != type))
	{
		mErrorBuffer.PushError(ParseError::DUPLICATE_SYMBOL, name, symbol->GetName());
		symbol = 0;
	}

	if (symbol == 0)
	{
		symbol = CreateSymbol(type, name, definition, parent);
		parent->InsertSymbol(symbol);
	}

	return symbol;
}


Symbol *SemanticAnalysisTraverser::CreateSymbol(Symbol::Type type, const Token *name, const ParseNode *definition, Symbol *parent)
{
	return new (mAllocator.Alloc<Symbol>()) Symbol(type, name, definition, parent);
}

}
