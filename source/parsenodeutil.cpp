#include "bond/parsenodeutil.h"
#include "bond/symboltable.h"

namespace Bond
{

bool AreMatchingTypes(const Symbol *scope, const TypeSpecifier *typeA, const TypeSpecifier *typeB)
{
	if ((typeA != 0) && (typeB != 0))
	{
		const Token *primitiveTypeA = typeA->GetPrimitiveTypeToken();
		const Token *primitiveTypeB = typeB->GetPrimitiveTypeToken();
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


bool AreMatchingTypes(const Symbol *scope, const TypeDescriptor *typeA, const TypeDescriptor *typeB)
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
		if (AreMatchingTypes(scope, typeA->GetTypeSpecifier(), typeB->GetTypeSpecifier()))
		{
			return false;
		}
		// TODO: Test length.

		typeA = typeA->GetParent();
		typeB = typeB->GetParent();
	}

	return (typeA == 0) && (typeB == 0);
}


bool AreMatchingFunctionPrototypes(const Symbol *scope, const FunctionPrototype *functionA, const FunctionPrototype *functionB)
{
	// TODO
	return true;
}


bool AreComparableTypes(const TypeDescriptor *typeA, const TypeDescriptor *typeB)
{
	return
		(typeA->IsPointerType() && typeB->IsPointerType()) ||
		(typeA->IsNumericType() && typeB->IsNumericType()) ||
		(typeA->IsBooleanType() && typeB->IsBooleanType());
}

}
