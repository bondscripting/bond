#include "bond/parsenodes.h"
#include "bond/parsenodeutil.h"
#include "bond/stringutil.h"
#include <string.h>

namespace Bond
{

bool Symbol::IsTypeDefinition() const
{
	SymbolType type = GetSymbolType();
	return (type == TYPE_ENUM) || (type == TYPE_STRUCT);
}


const Symbol *Symbol::FindSymbol(const char *name) const
{
	const bu32_t hashCode = StringHash(name);
	const Symbol *symbol = mSymbolList;

	while ((symbol != 0) && !symbol->Matches(hashCode, name))
	{
		symbol = symbol->mNextSymbol;
	}

	return symbol;
}


Symbol *Symbol::FindSymbol(const Token *name)
{
	Symbol *symbol = mSymbolList;

	while ((symbol != 0) && !symbol->Matches(name))
	{
		symbol = symbol->mNextSymbol;
	}

	return symbol;
}


const Symbol *Symbol::FindSymbol(const Token *name) const
{
	const Symbol *symbol = mSymbolList;

	while ((symbol != 0) && !symbol->Matches(name))
	{
		symbol = symbol->mNextSymbol;
	}

	return symbol;
}


Symbol *Symbol::FindSymbol(const QualifiedIdentifier *identifier)
{
	Symbol *symbol = FindQualifiedSymbol(identifier);

	if ((symbol == 0) && (mParentSymbol != 0))
	{
		symbol = mParentSymbol->FindSymbol(identifier);
	}

	return symbol;
}


const Symbol *Symbol::FindSymbol(const QualifiedIdentifier *identifier) const
{
	const Symbol *symbol = FindQualifiedSymbol(identifier);

	if ((symbol == 0) && (mParentSymbol != 0))
	{
		symbol = mParentSymbol->FindSymbol(identifier);
	}

	return symbol;
}


Symbol *Symbol::FindQualifiedSymbol(const QualifiedIdentifier *identifier)
{
	Symbol *symbol = 0;

	if (identifier->IsTerminal())
	{
		symbol = FindSymbol(identifier->GetName());
	}
	else if (GetSymbolType() == TYPE_NAMESPACE)
	{
		Symbol *nextScope = FindSymbol(identifier->GetName());
		const QualifiedIdentifier *nextIdentifier = identifier->GetNextIdentifier();
		symbol = nextScope->FindQualifiedSymbol(nextIdentifier);
	}

	return symbol;
}


const Symbol *Symbol::FindQualifiedSymbol(const QualifiedIdentifier *identifier) const
{
	const Symbol *symbol = 0;

	if (identifier->IsTerminal())
	{
		symbol = FindSymbol(identifier->GetName());
	}
	else
	{
		const Symbol *nextScope = FindSymbol(identifier->GetName());
		const QualifiedIdentifier *nextIdentifier = identifier->GetNextIdentifier();
		symbol = nextScope->FindQualifiedSymbol(nextIdentifier);
	}

	return symbol;
}


void Symbol::InsertSymbol(Symbol *symbol)
{
	symbol->SetParentSymbol(this);
	symbol->mNextSymbol = mSymbolList;
	mSymbolList = symbol;
}


bool Symbol::Matches(bu32_t hashCode, const char *name) const
{
	const Token *n = GetName();
	return !IsAnonymous() && (hashCode == n->GetHashCode()) && (strcmp(name, n->GetText()) == 0);
}


bool Symbol::Matches(const Token *name) const
{
	const Token *n = GetName();
	return !IsAnonymous() && (name->GetHashCode() == n->GetHashCode()) && (strcmp(name->GetText(), n->GetText()) == 0);
}


const Token *TypeDescriptor::GetContextToken() const
{
	if (mTypeSpecifier != 0)
	{
		return mTypeSpecifier->GetContextToken();
	}
	if (mParent != 0)
	{
		return mParent->GetContextToken();
	}
	return 0;
}


void TypeDescriptor::ConvertToArray(Expression *expressionList)
{
	const bu32_t constFlag = mFlags & FLAG_CONST;
	mFlags = ((mFlags | FLAG_LVALUE) << PARENT_SHIFT) | FLAG_ARRAY | constFlag;
	mLengthExpressionList = expressionList;
}


TypeDescriptor TypeDescriptor::GetDereferencedType() const
{
	TypeDescriptor typeDescriptor;

	if (IsArrayType())
	{
		Expression *nextLength = static_cast<Expression *>(mLengthExpressionList->GetNextNode());
		if (nextLength != 0)
		{
			typeDescriptor = *this;
			typeDescriptor.mLengthExpressionList = nextLength;
		}
		else if (HasFlattenedParent())
		{
			typeDescriptor = *this;
			typeDescriptor.mFlags = mFlags >> PARENT_SHIFT;
			typeDescriptor.mLengthExpressionList = 0;
		}
		else
		{
			typeDescriptor = *mParent;
		}
	}
	else if (IsPointerIntrinsicType())
	{
		if (HasFlattenedParent())
		{
			typeDescriptor = *this;
			typeDescriptor.mFlags = mFlags >> PARENT_SHIFT;
			if (typeDescriptor.IsArrayType())
			{
				typeDescriptor.mLengthExpressionList = static_cast<Expression *>(mLengthExpressionList->GetNextNode());
			}
		}
		else
		{
			typeDescriptor = *mParent;
		}
	}

	return typeDescriptor;
}


TypeDescriptor TypeDescriptor::GetArrayValueType() const
{
	TypeDescriptor typeDescriptor;

	if (IsArrayType())
	{
		if (HasFlattenedParent())
		{
			typeDescriptor = *this;
			typeDescriptor.mFlags = mFlags >> PARENT_SHIFT;
			typeDescriptor.mLengthExpressionList = 0;
		}
		else
		{
			typeDescriptor = *mParent;
		}
	}
	return typeDescriptor;
}


bool TypeDescriptor::IsResolved() const
{
	const Expression *lengthExpressionList = mLengthExpressionList;
	while (lengthExpressionList != 0)
	{
		if (!lengthExpressionList->GetTypeAndValue().IsValueDefined())
		{
			return false;
		}
		lengthExpressionList = static_cast<const Expression *>(lengthExpressionList->GetNextNode());
	}
	return (mParent == 0) || mParent->IsResolved();
}


bu32_t TypeDescriptor::GetSize(bu32_t pointerSize) const
{
	if (IsValueType())
	{
		return mTypeSpecifier->GetSize(pointerSize);
	}
	else if (IsPointerIntrinsicType())
	{
		return pointerSize;
	}
	else if (IsArrayType())
	{
		const TypeDescriptor parent = GetDereferencedType();
		const bu32_t parentSize = parent.GetSize(pointerSize);
		const bu32_t arraySize = mLengthExpressionList->GetTypeAndValue().GetUIntValue();
		const bu32_t size = arraySize * parentSize;
		return size;
	}
	return 0;
}


Token::TokenType TypeDescriptor::GetPrimitiveType() const
{
	if (IsValueType())
	{
		return mTypeSpecifier->GetPrimitiveType();
	}
	return Token::INVALID;
}


bool TypeDescriptor::IsBooleanType() const
{
	if (IsValueType())
	{
		return mTypeSpecifier->IsBooleanType();
	}
	return false;
}


bool TypeDescriptor::IsIntegerType() const
{
	if (IsValueType())
	{
		return mTypeSpecifier->IsIntegerType();
	}
	return false;
}


bool TypeDescriptor::IsNumericType() const
{
	if (IsValueType())
	{
		return mTypeSpecifier->IsNumericType();
	}
	return false;
}


bool TypeDescriptor::IsVoidType() const
{
	if (IsValueType())
	{
		return mTypeSpecifier->IsVoidType();
	}
	return false;
}


TypeDescriptor TypeDescriptor::GetBoolType()
{
	return TypeDescriptor(&BOOL_TYPE_SPECIFIER, FLAG_VALUE);
}


TypeDescriptor TypeDescriptor::GetCharType()
{
	return TypeDescriptor(&CHAR_TYPE_SPECIFIER, FLAG_VALUE);
}


TypeDescriptor TypeDescriptor::GetIntType()
{
	return TypeDescriptor(&INT_TYPE_SPECIFIER, FLAG_VALUE);
}


TypeDescriptor TypeDescriptor::GetUIntType()
{
	return TypeDescriptor(&UINT_TYPE_SPECIFIER, FLAG_VALUE);
}


TypeDescriptor TypeDescriptor::GetFloatType()
{
	return TypeDescriptor(&FLOAT_TYPE_SPECIFIER, FLAG_VALUE);
}


TypeDescriptor TypeDescriptor::GetStringType()
{
	return TypeDescriptor(&CHAR_TYPE_SPECIFIER, ((FLAG_VALUE | FLAG_CONST | FLAG_LVALUE) << PARENT_SHIFT) | FLAG_POINTER);
}


TypeDescriptor TypeDescriptor::GetPointerType(const TypeSpecifier *specifier)
{
	return TypeDescriptor(specifier, ((FLAG_VALUE | FLAG_LVALUE) << PARENT_SHIFT) | FLAG_POINTER);
}


TypeDescriptor TypeDescriptor::GetConstPointerType(const TypeSpecifier *specifier)
{
	return TypeDescriptor(specifier, ((FLAG_VALUE | FLAG_CONST | FLAG_LVALUE) << PARENT_SHIFT) | FLAG_POINTER);
}


const Token *TypeSpecifier::GetContextToken() const
{
	if (mPrimitiveType != 0)
	{
		return mPrimitiveType;
	}
	if (mIdentifier != 0)
	{
		return mIdentifier->GetContextToken();
	}
	return 0;
}


bu32_t TypeSpecifier::GetSize(bu32_t pointerSize) const
{
	switch (GetPrimitiveType())
	{
		case Token::KEY_BOOL:
			return BOND_BOOL_SIZE;
		case Token::KEY_CHAR:
			return BOND_CHAR_SIZE;
		case Token::KEY_FLOAT:
			return BOND_FLOAT_SIZE;
		case Token::KEY_INT:
			return BOND_INT_SIZE;
		case Token::KEY_UINT:
			return BOND_UINT_SIZE;

		default:
		{
			const StructDeclaration *structDeclaration = CastNode<StructDeclaration>(mDefinition);
			if (structDeclaration != 0)
			{
				return structDeclaration->GetSize();
			}
		}
	}
	return 0;
}


Token::TokenType TypeSpecifier::GetPrimitiveType() const
{
	if (mPrimitiveType != 0)
	{
		return mPrimitiveType->GetTokenType();
	}
	return Token::INVALID;
}


bool TypeSpecifier::IsBooleanType() const
{
	if (mPrimitiveType != 0)
	{
		return BOOLEAN_TYPE_SPECIFIERS_TYPESET.Contains(mPrimitiveType->GetTokenType());
	}
	return false;
}


bool TypeSpecifier::IsIntegerType() const
{
	if (mPrimitiveType != 0)
	{
		return INTEGER_TYPE_SPECIFIERS_TYPESET.Contains(mPrimitiveType->GetTokenType());
	}
	return false;
}


bool TypeSpecifier::IsNumericType() const
{
	if (mPrimitiveType != 0)
	{
		return NUMERIC_TYPE_SPECIFIERS_TYPESET.Contains(mPrimitiveType->GetTokenType());
	}
	return false;
}


bool TypeSpecifier::IsVoidType() const
{
	if (mPrimitiveType != 0)
	{
		return VOID_TYPE_SPECIFIERS_TYPESET.Contains(mPrimitiveType->GetTokenType());
	}
	return false;
}


const Token *Initializer::GetContextToken() const
{
	if (mExpression != 0)
	{
		return mExpression->GetContextToken();
	}
	else if (mInitializerList != 0)
	{
		return mInitializerList->GetContextToken();
	}
	return 0;
}


bool Initializer::IsResolved() const
{
	if (mExpression != 0)
	{
		return mExpression->GetTypeAndValue().IsResolved();
	}
	else
	{
		const Initializer *initializer = mInitializerList;
		while (initializer != 0)
		{
			if (!initializer->IsResolved())
			{
				return false;
			}
			initializer = static_cast<const Initializer*>(initializer->GetNextNode());
		}
	}

	return true;
}


const TypeSpecifier BOOL_TYPE_SPECIFIER(&BOOL_TOKEN);
const TypeSpecifier CHAR_TYPE_SPECIFIER(&CHAR_TOKEN);
const TypeSpecifier INT_TYPE_SPECIFIER(&INT_TOKEN);
const TypeSpecifier UINT_TYPE_SPECIFIER(&UINT_TOKEN);
const TypeSpecifier FLOAT_TYPE_SPECIFIER(&FLOAT_TOKEN);

}
