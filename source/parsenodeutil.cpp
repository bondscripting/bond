#include "bond/parsenodeutil.h"
#include "bond/symboltable.h"

namespace Bond
{

bool IsConstantTypeDescriptor(const TypeDescriptor *type)
{
	while (type != 0)
	{
		if (!type->IsConst() && (type->GetVariant() != TypeDescriptor::VARIANT_ARRAY))
		{
			return false;
		}
		type = type->GetParent();
	}
	return true;
}


bool TestMatchingTypeSpecifiers(const Symbol *scope, const TypeSpecifier *typeA, const TypeSpecifier *typeB)
{
	if ((typeA != 0) && (typeB != 0))
	{
		const Token *primitiveTypeA = typeA->GetPrimitiveType();
		const Token *primitiveTypeB = typeB->GetPrimitiveType();
		const QualifiedIdentifier *identifierA = typeA->GetIdentifier();
		const QualifiedIdentifier *identifierB = typeB->GetIdentifier();

		if ((primitiveTypeA != 0) && (primitiveTypeA != 0))
		{
			return primitiveTypeA->GetTokenType() == primitiveTypeB->GetTokenType();
		}
		else if (!((primitiveTypeA == 0) && (primitiveTypeA == 0)))
		{
			return false;
		}
		else if ((identifierA != 0) && (identifierB != 0))
		{
			const Symbol *symbolA = scope->FindSymbol(identifierA);
			const Symbol *symbolB = scope->FindSymbol(identifierB);

			return (symbolA != 0) && (symbolA == symbolB);
		}
		else if (!((identifierA == 0) && (identifierB == 0)))
		{
			return false;
		}

		return true;
	}
	else if (!((typeA == 0) && (typeB == 0)))
	{
		return false;
	}

	return true;
}


bool TestMatchingTypeDescriptors(const Symbol *scope, const TypeDescriptor *typeA, const TypeDescriptor *typeB)
{
	while ((typeA != 0) && (typeB != 0))
	{
		if (typeA->GetVariant() != typeB->GetVariant())
		{
			return false;
		}
		if (typeA->IsConst() != typeB->IsConst())
		{
			return false;
		}
		if (TestMatchingTypeSpecifiers(scope, typeA->GetTypeSpecifier(), typeB->GetTypeSpecifier()))
		{
			return false;
		}
		// TODO: Test length.

		typeA = typeA->GetParent();
		typeB = typeB->GetParent();
	}

	return (typeA == 0) && (typeB == 0);
}


bool TestMatchingFunctionPrototypes(const Symbol *scope, const FunctionPrototype *functionA, const FunctionPrototype *functionB)
{
	// TODO
	return true;
}


}
