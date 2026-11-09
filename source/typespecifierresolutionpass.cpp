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


void TypeSpecifierResolutionPass::Visit(FunctionDefinition *functionDefinition)
{
	// Only the prototype is resolved here, so that every signature is known before any body is
	// analyzed. Bodies are resolved by the TypeEvaluationPass in source order, since local names are
	// only visible after their declaration. Parameters aren't in the function's scope yet, so
	// resolving the prototype from the enclosing scope is equivalent to resolving it from the
	// function's scope.
	Traverse(functionDefinition->GetPrototype());
}


void TypeSpecifierResolutionPass::Visit(TypeSpecifier *typeSpecifier)
{
	ResolveTypeSpecifier(typeSpecifier);
}

}
