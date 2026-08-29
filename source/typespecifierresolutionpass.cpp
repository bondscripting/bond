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
		if (declaration != nullptr)
		{
			if (declaration->GetSizeSpecifier() == nullptr)
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

void TypeSpecifierResolutionPass::Visit(UsingDeclaration *usingDeclaration)
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


void TypeSpecifierResolutionPass::Visit(UsingDirective *usingDirective)
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
		usingDirective->SetNamespace(target);
		GetCurrentScope()->AddUsingNamespace(usingDirective);
	}
}

}
