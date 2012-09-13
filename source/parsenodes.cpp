#include "bond/math.h"
#include "bond/opcodes.h"
#include "bond/parsenodes.h"
#include "bond/parsenodeutil.h"
#include "bond/stringutil.h"

namespace Bond
{

bool Symbol::IsTypeDefinition() const
{
	SymbolType type = GetSymbolType();
	return (type == TYPE_ENUM) || (type == TYPE_STRUCT);
}


void Symbol::SetParentSymbol(Symbol *parent)
{
	mParentSymbol = parent;
	mGlobalHashCode = ComputeGlobalHashCode();
}


bu32_t Symbol::ComputeGlobalHashCode() const
{
	bu32_t hash = STRING_HASH_SEED;
	if (!IsAnonymous())
	{
		const HashedString &name = GetName()->GetHashedText();
		if (mParentSymbol != NULL)
		{
			hash = mParentSymbol->GetGlobalHashCode();
			hash = StringHash("::", hash);
			hash = StringHash(name.GetLength(), name.GetString(), hash);
		}
		else
		{
			hash = name.GetHashCode();
		}
	}
	return hash;
}


Symbol *Symbol::FindSymbol(const HashedString &name)
{
	Symbol *symbol = mSymbolList;

	while ((symbol != NULL) && !symbol->Matches(name))
	{
		symbol = symbol->mNextSymbol;
	}

	return symbol;
}


const Symbol *Symbol::FindSymbol(const HashedString &name) const
{
	const Symbol *symbol = mSymbolList;

	while ((symbol != NULL) && !symbol->Matches(name))
	{
		symbol = symbol->mNextSymbol;
	}

	return symbol;
}


Symbol *Symbol::FindSymbol(const QualifiedIdentifier *identifier)
{
	Symbol *symbol = FindQualifiedSymbol(identifier);

	if ((symbol == NULL) && (mParentSymbol != NULL))
	{
		symbol = mParentSymbol->FindSymbol(identifier);
	}

	return symbol;
}


const Symbol *Symbol::FindSymbol(const QualifiedIdentifier *identifier) const
{
	const Symbol *symbol = FindQualifiedSymbol(identifier);

	if ((symbol == NULL) && (mParentSymbol != NULL))
	{
		symbol = mParentSymbol->FindSymbol(identifier);
	}

	return symbol;
}


Symbol *Symbol::FindQualifiedSymbol(const QualifiedIdentifier *identifier)
{
	Symbol *symbol = NULL;

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
	const Symbol *symbol = NULL;

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


bool Symbol::Matches(const HashedString &name) const
{
	const Token *n = GetName();
	return !IsAnonymous() && (n->GetHashedText() == name);
}


const Token *TypeDescriptor::GetContextToken() const
{
	if (mTypeSpecifier != NULL)
	{
		return mTypeSpecifier->GetContextToken();
	}
	if (mParent != NULL)
	{
		return mParent->GetContextToken();
	}
	return NULL;
}


void TypeDescriptor::ConvertToArray(Expression *expressionList)
{
	const bu32_t constFlag = mFlags & FLAG_CONST;
	mFlags = ((mFlags | FLAG_ADDRESSABLE) << PARENT_SHIFT) | FLAG_ARRAY | constFlag;
	mLengthExpressionList = expressionList;
}


void TypeDescriptor::ConvertToPointerIntrinsic()
{
	if (IsArrayType())
	{
		mFlags = (mFlags << PARENT_SHIFT) | FLAG_POINTER;
	}
}


TypeDescriptor TypeDescriptor::GetDereferencedType() const
{
	TypeDescriptor typeDescriptor;

	if (IsArrayType())
	{
		Expression *nextLength = NextNode(mLengthExpressionList);
		if (nextLength != NULL)
		{
			typeDescriptor = *this;
			typeDescriptor.mLengthExpressionList = nextLength;
		}
		else if (HasFlattenedParent())
		{
			typeDescriptor = *this;
			typeDescriptor.mFlags = mFlags >> PARENT_SHIFT;
			typeDescriptor.mLengthExpressionList = NULL;
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
				typeDescriptor = typeDescriptor.GetDereferencedType();
			}
		}
		else
		{
			typeDescriptor = *mParent;
		}
	}

	return typeDescriptor;
}


TypeDescriptor TypeDescriptor::GetArrayElementType() const
{
	TypeDescriptor typeDescriptor;

	if (IsArrayType())
	{
		if (HasFlattenedParent())
		{
			typeDescriptor = *this;
			typeDescriptor.mFlags = mFlags >> PARENT_SHIFT;
			typeDescriptor.mLengthExpressionList = NULL;
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
	while (lengthExpressionList != NULL)
	{
		if (!lengthExpressionList->GetTypeAndValue().IsValueDefined())
		{
			return false;
		}
		lengthExpressionList = NextNode(lengthExpressionList);
	}
	return
		((mParent == NULL) || mParent->IsResolved()) &&
		((mTypeSpecifier == NULL) || (mTypeSpecifier->IsResolved()));
}


bu32_t TypeDescriptor::GetStackSize(PointerSize pointerSize) const
{
	if (IsPointerType())
	{
		return BOND_SLOT_SIZE;
	}
	else
	{
		return AlignUp(GetSize(pointerSize), static_cast<bu32_t>(BOND_SLOT_SIZE));
	}
}


bu32_t TypeDescriptor::GetSize(PointerSize pointerSize) const
{
	if (IsValueType())
	{
		return mTypeSpecifier->GetSize(pointerSize);
	}
	else if (IsPointerIntrinsicType())
	{
		return GetPointerSize(pointerSize);
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


bu32_t TypeDescriptor::GetAlignment(PointerSize pointerSize) const
{
	if (IsValueType())
	{
		return mTypeSpecifier->GetAlignment();
	}
	else if (IsPointerIntrinsicType())
	{
		return GetSize(pointerSize);
	}
	else if (IsArrayType())
	{
		const TypeDescriptor elementType = GetArrayElementType();
		return elementType.GetAlignment(pointerSize);
	}
	return 0;
}


bu32_t TypeDescriptor::GetSignatureType() const
{
	if (IsPointerType())
	{
		return SIG_POINTER;
	}
	return mTypeSpecifier->GetSignatureType();
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


bool TypeDescriptor::IsLeast32IntegerType() const
{
	if (IsValueType())
	{
		return mTypeSpecifier->IsLeast32IntegerType();
	}
	return false;
}


bool TypeDescriptor::IsMost32IntegerType() const
{
	if (IsValueType())
	{
		return mTypeSpecifier->IsMost32IntegerType();
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


TypeDescriptor TypeDescriptor::GetLongType()
{
	return TypeDescriptor(&LONG_TYPE_SPECIFIER, FLAG_VALUE);
}


TypeDescriptor TypeDescriptor::GetULongType()
{
	return TypeDescriptor(&ULONG_TYPE_SPECIFIER, FLAG_VALUE);
}


TypeDescriptor TypeDescriptor::GetFloatType()
{
	return TypeDescriptor(&FLOAT_TYPE_SPECIFIER, FLAG_VALUE);
}


TypeDescriptor TypeDescriptor::GetDoubleType()
{
	return TypeDescriptor(&DOUBLE_TYPE_SPECIFIER, FLAG_VALUE);
}


TypeDescriptor TypeDescriptor::GetStringType()
{
	return TypeDescriptor(&CHAR_TYPE_SPECIFIER, ((FLAG_VALUE | FLAG_CONST | FLAG_ADDRESSABLE) << PARENT_SHIFT) | FLAG_POINTER);
}


TypeDescriptor TypeDescriptor::GetPointerType(const TypeSpecifier *specifier)
{
	return TypeDescriptor(specifier, ((FLAG_VALUE | FLAG_ADDRESSABLE) << PARENT_SHIFT) | FLAG_POINTER);
}


TypeDescriptor TypeDescriptor::GetConstPointerType(const TypeSpecifier *specifier)
{
	return TypeDescriptor(specifier, ((FLAG_VALUE | FLAG_CONST | FLAG_ADDRESSABLE) << PARENT_SHIFT) | FLAG_POINTER);
}


TypeDescriptor TypeDescriptor::GetNullType()
{
	return TypeDescriptor(NULL, FLAG_NULL);
}


const Token *TypeSpecifier::GetContextToken() const
{
	if (mPrimitiveType != NULL)
	{
		return mPrimitiveType;
	}
	if (mIdentifier != NULL)
	{
		return mIdentifier->GetContextToken();
	}
	return NULL;
}


bool TypeSpecifier::IsResolved() const
{
	return (mDefinition == NULL) || (mDefinition->IsResolved());
}


bu32_t TypeSpecifier::GetSize(PointerSize pointerSize) const
{
	switch (GetPrimitiveType())
	{
		case Token::KEY_VOID:
			return BOND_VOID_SIZE;
		case Token::KEY_BOOL:
			return BOND_BOOL_SIZE;
		case Token::KEY_CHAR:
			return BOND_CHAR_SIZE;
		case Token::KEY_UCHAR:
			return BOND_UCHAR_SIZE;
		case Token::KEY_SHORT:
			return BOND_SHORT_SIZE;
		case Token::KEY_USHORT:
			return BOND_USHORT_SIZE;
		case Token::KEY_INT:
			return BOND_INT_SIZE;
		case Token::KEY_UINT:
			return BOND_UINT_SIZE;
		case Token::KEY_LONG:
			return BOND_LONG_SIZE;
		case Token::KEY_ULONG:
			return BOND_ULONG_SIZE;
		case Token::KEY_FLOAT:
			return BOND_FLOAT_SIZE;
		case Token::KEY_DOUBLE:
			return BOND_DOUBLE_SIZE;

		default:
		{
			const StructDeclaration *structDeclaration = CastNode<StructDeclaration>(mDefinition);
			if (structDeclaration != NULL)
			{
				return structDeclaration->GetSize();
			}
		}
	}
	return 0;
}


bu32_t TypeSpecifier::GetAlignment() const
{
	switch (GetPrimitiveType())
	{
		case Token::KEY_VOID:
			return BOND_VOID_SIZE;
		case Token::KEY_BOOL:
			return BOND_BOOL_SIZE;
		case Token::KEY_CHAR:
			return BOND_CHAR_SIZE;
		case Token::KEY_UCHAR:
			return BOND_UCHAR_SIZE;
		case Token::KEY_SHORT:
			return BOND_SHORT_SIZE;
		case Token::KEY_USHORT:
			return BOND_USHORT_SIZE;
		case Token::KEY_INT:
			return BOND_INT_SIZE;
		case Token::KEY_UINT:
			return BOND_UINT_SIZE;
		case Token::KEY_LONG:
			return BOND_LONG_SIZE;
		case Token::KEY_ULONG:
			return BOND_ULONG_SIZE;
		case Token::KEY_FLOAT:
			return BOND_FLOAT_SIZE;
		case Token::KEY_DOUBLE:
			return BOND_DOUBLE_SIZE;

		default:
		{
			const StructDeclaration *structDeclaration = CastNode<StructDeclaration>(mDefinition);
			if (structDeclaration != NULL)
			{
				return structDeclaration->GetAlignment();
			}
		}
	}
	return 0;
}


bu32_t TypeSpecifier::GetSignatureType() const
{
	switch (GetPrimitiveType())
	{
		case Token::KEY_BOOL:
			return SIG_BOOL;
		case Token::KEY_CHAR:
			return SIG_CHAR;
		case Token::KEY_UCHAR:
			return SIG_UCHAR;
		case Token::KEY_SHORT:
			return SIG_SHORT;
		case Token::KEY_USHORT:
			return SIG_USHORT;
		case Token::KEY_INT:
			return SIG_INT;
		case Token::KEY_UINT:
			return SIG_UINT;
		case Token::KEY_LONG:
			return SIG_LONG;
		case Token::KEY_ULONG:
			return SIG_ULONG;
		case Token::KEY_FLOAT:
			return SIG_FLOAT;
		case Token::KEY_DOUBLE:
			return SIG_DOUBLE;

		default:
		{
			const StructDeclaration *structDeclaration = CastNode<StructDeclaration>(mDefinition);
			if (structDeclaration != NULL)
			{
				return SIG_STRUCT;
			}
		}
	}
	return SIG_VOID;
}


Token::TokenType TypeSpecifier::GetPrimitiveType() const
{
	if (mPrimitiveType != NULL)
	{
		return mPrimitiveType->GetTokenType();
	}
	return Token::INVALID;
}


bool TypeSpecifier::IsBooleanType() const
{
	if (mPrimitiveType != NULL)
	{
		return BOOLEAN_TYPE_SPECIFIERS_TYPESET.Contains(mPrimitiveType->GetTokenType());
	}
	return false;
}


bool TypeSpecifier::IsIntegerType() const
{
	if (mPrimitiveType != NULL)
	{
		return INTEGER_TYPE_SPECIFIERS_TYPESET.Contains(mPrimitiveType->GetTokenType());
	}
	return false;
}


bool TypeSpecifier::IsLeast32IntegerType() const
{
	if (mPrimitiveType != NULL)
	{
		return LEAST32_INTEGER_TYPE_SPECIFIERS_TYPESET.Contains(mPrimitiveType->GetTokenType());
	}
	return false;
}


bool TypeSpecifier::IsMost32IntegerType() const
{
	if (mPrimitiveType != NULL)
	{
		return MOST32_INTEGER_TYPE_SPECIFIERS_TYPESET.Contains(mPrimitiveType->GetTokenType());
	}
	return false;
}


bool TypeSpecifier::IsNumericType() const
{
	if (mPrimitiveType != NULL)
	{
		return NUMERIC_TYPE_SPECIFIERS_TYPESET.Contains(mPrimitiveType->GetTokenType());
	}
	return false;
}


bool TypeSpecifier::IsVoidType() const
{
	if (mPrimitiveType != NULL)
	{
		return VOID_TYPE_SPECIFIERS_TYPESET.Contains(mPrimitiveType->GetTokenType());
	}
	return false;
}


bool StructDeclaration::IsResolved() const
{
	return (mVariant == VARIANT_REFERENCE) || (mSize > 0);
}


const Token *Initializer::GetContextToken() const
{
	if (mExpression != NULL)
	{
		return mExpression->GetContextToken();
	}
	else if (mInitializerList != NULL)
	{
		return mInitializerList->GetContextToken();
	}
	return NULL;
}


bool Initializer::IsResolved() const
{
	if (mExpression != NULL)
	{
		return mExpression->GetTypeAndValue().IsResolved();
	}
	else
	{
		const Initializer *initializer = mInitializerList;
		while (initializer != NULL)
		{
			if (!initializer->IsResolved())
			{
				return false;
			}
			initializer = NextNode(initializer);
		}
	}

	return true;
}


const TypeSpecifier BOOL_TYPE_SPECIFIER(&BOOL_TOKEN);
const TypeSpecifier CHAR_TYPE_SPECIFIER(&CHAR_TOKEN);
const TypeSpecifier INT_TYPE_SPECIFIER(&INT_TOKEN);
const TypeSpecifier UINT_TYPE_SPECIFIER(&UINT_TOKEN);
const TypeSpecifier LONG_TYPE_SPECIFIER(&LONG_TOKEN);
const TypeSpecifier ULONG_TYPE_SPECIFIER(&ULONG_TOKEN);
const TypeSpecifier FLOAT_TYPE_SPECIFIER(&FLOAT_TOKEN);
const TypeSpecifier DOUBLE_TYPE_SPECIFIER(&DOUBLE_TOKEN);

}
