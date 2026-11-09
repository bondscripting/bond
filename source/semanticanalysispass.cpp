#include "private/semanticanalysispass.h"
#include "bond/compiler/compilererror.h"
#include "bond/compiler/parsenodes.h"
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
	compoundStatement->SetParentSymbol(GetCurrentScope());
	auto stackElement = mScopeStack.Push(compoundStatement);
	ParseNodeTraverser::Visit(compoundStatement);
}


void SemanticAnalysisPass::Visit(SwitchSection *switchSection)
{
	switchSection->SetParentSymbol(GetCurrentScope());
	auto stackElement = mScopeStack.Push(switchSection);
	ParseNodeTraverser::Visit(switchSection);
}


void SemanticAnalysisPass::Visit(ForStatement *forStatement)
{
	forStatement->SetParentSymbol(GetCurrentScope());
	auto stackElement = mScopeStack.Push(forStatement);
	ParseNodeTraverser::Visit(forStatement);
}


Symbol *SemanticAnalysisPass::GetSymbol(const Token *name)
{
	// Only searches the current scope's own declarations (e.g. native struct stub matching). Names
	// made visible by using-directives are not considered, unlike the qualified-identifier overload.
	Symbol *parent = GetCurrentScope();
	return parent->FindSymbol(name);
}


Symbol *SemanticAnalysisPass::GetSymbol(const QualifiedIdentifier *identifier)
{
	Symbol *parent = GetCurrentScope();
	bool isAmbiguous = false;
	Symbol *symbol = parent->FindSymbol(identifier, isAmbiguous);

	if (isAmbiguous)
	{
		mErrorBuffer.PushError(CompilerError::AMBIGUOUS_SYMBOL_REFERENCE, identifier->GetContextToken(), identifier);
	}

	return symbol;
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


void SemanticAnalysisPass::ResolveTypeSpecifier(TypeSpecifier *typeSpecifier)
{
	const QualifiedIdentifier *identifier = typeSpecifier->GetIdentifier();
	if (identifier != nullptr)
	{
		const Symbol *symbol = GetSymbol(identifier);
		if (symbol == nullptr)
		{
			mErrorBuffer.PushError(CompilerError::SYMBOL_IS_NOT_DEFINED, identifier->GetContextToken(), identifier);
		}
		else if (!symbol->IsTypeDefinition())
		{
			mErrorBuffer.PushError(CompilerError::SYMBOL_IS_NOT_A_TYPE, identifier->GetContextToken(), identifier);
		}
		else
		{
			typeSpecifier->SetDefinition(symbol);
		}
	}
}


void SemanticAnalysisPass::ResolveUsingDeclaration(UsingDeclaration *usingDeclaration)
{
	const QualifiedIdentifier *identifier = usingDeclaration->GetIdentifier();
	Symbol *target = GetSymbol(identifier);

	if (target == nullptr)
	{
		mErrorBuffer.PushError(CompilerError::SYMBOL_IS_NOT_DEFINED, identifier->GetContextToken(), identifier);
	}
	else if (target->GetSymbolType() == Symbol::TYPE_NAMESPACE)
	{
		mErrorBuffer.PushError(CompilerError::USING_NAMESPACE_REQUIRES_NAMESPACE_KEYWORD, identifier->GetContextToken(), identifier);
	}
	else
	{
		usingDeclaration->SetTarget(target);
		InsertSymbol(usingDeclaration);
	}
}


void SemanticAnalysisPass::ResolveUsingDirective(UsingDirective *usingDirective)
{
	const QualifiedIdentifier *identifier = usingDirective->GetIdentifier();
	Symbol *target = GetSymbol(identifier);

	if (target == nullptr)
	{
		mErrorBuffer.PushError(CompilerError::SYMBOL_IS_NOT_DEFINED, identifier->GetContextToken(), identifier);
	}
	else if (target->GetSymbolType() != Symbol::TYPE_NAMESPACE)
	{
		mErrorBuffer.PushError(CompilerError::EXPECTED_NAMESPACE, identifier->GetContextToken(), identifier);
	}
	else
	{
		// Find the nearest namespace that encloses both the directive and the target namespace. The
		// global scope is a namespace that encloses everything, so the search always succeeds.
		Symbol *scope = GetCurrentScope();
		Symbol *enclosingNamespace = nullptr;
		while ((scope != nullptr) && (enclosingNamespace == nullptr))
		{
			if (scope->GetSymbolType() == Symbol::TYPE_NAMESPACE)
			{
				for (const Symbol *ancestor = target; ancestor != nullptr; ancestor = ancestor->GetParentSymbol())
				{
					if (ancestor == scope)
					{
						enclosingNamespace = scope;
						break;
					}
				}
			}
			scope = scope->GetParentSymbol();
		}

		usingDirective->SetNamespace(target);
		usingDirective->SetEnclosingNamespace(enclosingNamespace);
		GetCurrentScope()->AddUsingNamespace(usingDirective);
	}
}

}
