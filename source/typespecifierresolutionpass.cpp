#include "private/typespecifierresolutionpass.h"
#include "bond/compiler/compilererror.h"
#include "bond/compiler/parsenodes.h"
#include "bond/compiler/parsenodeutil.h"

namespace Bond
{

void TypeSpecifierResolutionPass::Visit(StructDeclaration *structDeclaration)
{
	if (structDeclaration->GetVariant() == StructDeclaration::VARIANT_NATIVE_STUB)
	{
		StructDeclaration *declaration = CastNode<StructDeclaration>(GetSymbol(structDeclaration->GetName()));
		if (declaration != NULL)
		{
			if (declaration->GetSizeSpecifier() == NULL)
			{
				declaration->SetSizeSpecifier(structDeclaration);
			}
			else
			{
				mErrorBuffer.PushError(CompilerError::DUPLICATE_NATIVE_STRUCT_STUB,
					structDeclaration->GetName(), declaration->GetSizeSpecifier()->GetName());
			}
		}
		else
		{
			mErrorBuffer.PushError(CompilerError::NATIVE_STRUCT_IS_NOT_DEFINED, structDeclaration->GetName());
		}
	}
	SemanticAnalysisPass::Visit(structDeclaration);
}


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
