#include "bond/compilererror.h"
#include "bond/parsenodes.h"
#include "private/typespecifierresolutionpass.h"

namespace Bond
{

void TypeSpecifierResolutionPass::Visit(TypeSpecifier *typeSpecifier)
{
	const QualifiedIdentifier *identifier = typeSpecifier->GetIdentifier();
	if (identifier != NULL)
	{
		const Symbol *symbol = GetSymbol(identifier);
		if (symbol == NULL)
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


void TypeSpecifierResolutionPass::Visit(CompoundStatement *compoundStatement)
{
	Symbol *parent = GetCurrentScope();
	compoundStatement->SetParentSymbol(parent);
	SemanticAnalysisPass::Visit(compoundStatement);
}


void TypeSpecifierResolutionPass::Visit(SwitchSection *switchSection)
{
	Symbol *parent = GetCurrentScope();
	switchSection->SetParentSymbol(parent);
	SemanticAnalysisPass::Visit(switchSection);
}


void TypeSpecifierResolutionPass::Visit(ForStatement *forStatement)
{
	Symbol *parent = GetCurrentScope();
	forStatement->SetParentSymbol(parent);
	SemanticAnalysisPass::Visit(forStatement);
}

}
