#include "bond/compiler/parsenodes.h"
#include "bond/compiler/parsenodeutil.h"
#include "bond/systems/math.h"
#include "bond/types/opcodes.h"
#include "bond/types/stringutil.h"

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


uint32_t Symbol::GetGlobalHashCodeWithSuffix(const char *suffix) const
{
	return StringHash(suffix, mGlobalHashCode);
}


uint32_t Symbol::ComputeGlobalHashCode() const
{
	uint32_t hash = STRING_HASH_SEED;
	if (!IsAnonymous())
	{
		const HashedString &name = GetName()->GetHashedText();
		if (mParentSymbol != nullptr)
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

	while ((symbol != nullptr) && !symbol->Matches(name))
	{
		symbol = symbol->mNextSymbol;
	}

	return symbol;
}


const Symbol *Symbol::FindSymbol(const HashedString &name) const
{
	const Symbol *symbol = mSymbolList;

	while ((symbol != nullptr) && !symbol->Matches(name))
	{
		symbol = symbol->mNextSymbol;
	}

	return symbol;
}


Symbol *Symbol::FindSymbol(const QualifiedIdentifier *identifier)
{
	Symbol *symbol = FindQualifiedSymbol(identifier);

	if ((symbol == nullptr) && (mParentSymbol != nullptr))
	{
		symbol = mParentSymbol->FindSymbol(identifier);
	}

	return symbol;
}


const Symbol *Symbol::FindSymbol(const QualifiedIdentifier *identifier) const
{
	const Symbol *symbol = FindQualifiedSymbol(identifier);

	if ((symbol == nullptr) && (mParentSymbol != nullptr))
	{
		symbol = mParentSymbol->FindSymbol(identifier);
	}

	return symbol;
}


Symbol *Symbol::FindQualifiedSymbol(const QualifiedIdentifier *identifier)
{
	Symbol *symbol = nullptr;

	if (identifier->IsTerminal())
	{
		symbol = FindSymbol(identifier->GetName());
	}
	else if (GetSymbolType() == TYPE_NAMESPACE)
	{
		Symbol *nextScope = FindSymbol(identifier->GetName());
		if (nextScope != nullptr)
		{
			const QualifiedIdentifier *nextIdentifier = identifier->GetNextIdentifier();
			symbol = nextScope->FindQualifiedSymbol(nextIdentifier);
		}
	}

	return symbol;
}


const Symbol *Symbol::FindQualifiedSymbol(const QualifiedIdentifier *identifier) const
{
	const Symbol *symbol = nullptr;

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
	if (mTypeSpecifier != nullptr)
	{
		return mTypeSpecifier->GetContextToken();
	}
	if (mParent != nullptr)
	{
		return mParent->GetContextToken();
	}
	return nullptr;
}


void TypeDescriptor::ConvertToArray(Expression *expressionList)
{
	const uint32_t constFlag = mFlags & FLAG_CONST;
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
		if (nextLength != nullptr)
		{
			typeDescriptor = *this;
			typeDescriptor.mLengthExpressionList = nextLength;
		}
		else if (HasFlattenedParent())
		{
			typeDescriptor = *this;
			typeDescriptor.mFlags = mFlags >> PARENT_SHIFT;
			typeDescriptor.mLengthExpressionList = nullptr;
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
			typeDescriptor.mLengthExpressionList = nullptr;
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
	while (lengthExpressionList != nullptr)
	{
		if (!lengthExpressionList->GetTypeAndValue().IsValueDefined())
		{
			return false;
		}
		lengthExpressionList = NextNode(lengthExpressionList);
	}
	return
		((mParent == nullptr) || mParent->IsResolved()) &&
		((mTypeSpecifier == nullptr) || (mTypeSpecifier->IsResolved()));
}


bool TypeDescriptor::IsInstantiable() const
{
	if (IsValueType())
	{
		return mTypeSpecifier->IsInstantiable();
	}
	else if (IsArrayType())
	{
		const TypeDescriptor parent = GetDereferencedType();
		return parent.IsInstantiable();
	}
	return true;
}


uint32_t TypeDescriptor::GetStackSize(PointerSize pointerSize) const
{
	if (IsPointerType())
	{
		return BOND_SLOT_SIZE;
	}
	else
	{
		return AlignUp(GetSize(pointerSize), uint32_t(BOND_SLOT_SIZE));
	}
}


uint32_t TypeDescriptor::GetSize(PointerSize pointerSize) const
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
		const uint32_t parentSize = parent.GetSize(pointerSize);
		const uint32_t arraySize = mLengthExpressionList->GetTypeAndValue().GetUIntValue();
		const uint32_t size = arraySize * parentSize;
		return size;
	}
	return 0;
}


uint32_t TypeDescriptor::GetAlignment(PointerSize pointerSize) const
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


uint32_t TypeDescriptor::GetSignatureType() const
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


bool TypeDescriptor::IsCharType() const
{
	if (IsValueType())
	{
		return mTypeSpecifier->IsCharType();
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


bool TypeDescriptor::IsStructType() const
{
	if (IsValueType())
	{
		return mTypeSpecifier->IsStructType();
	}
	return false;
}


bool TypeDescriptor::IsStringType() const
{
	if (IsPointerType())
	{
		TypeDescriptor typeDescriptor = GetDereferencedType();
		return typeDescriptor.IsCharType();
	}
	return false;
}


TypeDescriptor TypeDescriptor::GetVoidType()
{
	return TypeDescriptor(&VOID_TYPE_SPECIFIER, FLAG_VALUE);
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
	return GetConstPointerType(&CHAR_TYPE_SPECIFIER);
}


TypeDescriptor TypeDescriptor::GetVoidPointerType()
{
	return GetConstPointerType(&VOID_TYPE_SPECIFIER);
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
	return TypeDescriptor(nullptr, FLAG_NULL);
}


const Token *TypeSpecifier::GetContextToken() const
{
	if (mPrimitiveType != nullptr)
	{
		return mPrimitiveType;
	}
	if (mIdentifier != nullptr)
	{
		return mIdentifier->GetContextToken();
	}
	return nullptr;
}


bool TypeSpecifier::IsResolved() const
{
	return (mDefinition == nullptr) || (mDefinition->IsResolved());
}


bool TypeSpecifier::IsInstantiable() const
{
	const StructDeclaration *structDeclaration = CastNode<StructDeclaration>(mDefinition);
	if (structDeclaration != nullptr)
	{
		return structDeclaration->IsInstantiable();
	}
	return true;
}


uint32_t TypeSpecifier::GetSize(PointerSize pointerSize) const
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
			if (structDeclaration != nullptr)
			{
				return structDeclaration->GetSize();
			}
		}
	}
	return 0;
}


uint32_t TypeSpecifier::GetAlignment() const
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
			if (structDeclaration != nullptr)
			{
				return structDeclaration->GetAlignment();
			}
		}
	}
	return 0;
}


uint32_t TypeSpecifier::GetSignatureType() const
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
			if (structDeclaration != nullptr)
			{
				return SIG_STRUCT;
			}
		}
	}
	return SIG_VOID;
}


Token::TokenType TypeSpecifier::GetPrimitiveType() const
{
	if (mPrimitiveType != nullptr)
	{
		return mPrimitiveType->GetTokenType();
	}
	return Token::INVALID;
}


bool TypeSpecifier::IsBooleanType() const
{
	if (mPrimitiveType != nullptr)
	{
		return BOOLEAN_TYPE_SPECIFIERS_TYPESET.Contains(mPrimitiveType->GetTokenType());
	}
	return false;
}


bool TypeSpecifier::IsCharType() const
{
	if (mPrimitiveType != nullptr)
	{
		return CHAR_TYPE_SPECIFIERS_TYPESET.Contains(mPrimitiveType->GetTokenType());
	}
	return false;
}


bool TypeSpecifier::IsIntegerType() const
{
	if (mPrimitiveType != nullptr)
	{
		return INTEGER_TYPE_SPECIFIERS_TYPESET.Contains(mPrimitiveType->GetTokenType());
	}
	return false;
}


bool TypeSpecifier::IsLeast32IntegerType() const
{
	if (mPrimitiveType != nullptr)
	{
		return LEAST32_INTEGER_TYPE_SPECIFIERS_TYPESET.Contains(mPrimitiveType->GetTokenType());
	}
	return false;
}


bool TypeSpecifier::IsMost32IntegerType() const
{
	if (mPrimitiveType != nullptr)
	{
		return MOST32_INTEGER_TYPE_SPECIFIERS_TYPESET.Contains(mPrimitiveType->GetTokenType());
	}
	return false;
}


bool TypeSpecifier::IsNumericType() const
{
	if (mPrimitiveType != nullptr)
	{
		return NUMERIC_TYPE_SPECIFIERS_TYPESET.Contains(mPrimitiveType->GetTokenType());
	}
	return false;
}


bool TypeSpecifier::IsVoidType() const
{
	if (mPrimitiveType != nullptr)
	{
		return VOID_TYPE_SPECIFIERS_TYPESET.Contains(mPrimitiveType->GetTokenType());
	}
	return false;
}


bool TypeSpecifier::IsStructType() const
{
	return GetSignatureType() == SIG_STRUCT;
}


bool StructDeclaration::IsResolved() const
{
	return (mSize > 0) || !IsInstantiable();
}


const Token *Initializer::GetContextToken() const
{
	if (mExpression != nullptr)
	{
		return mExpression->GetContextToken();
	}
	else if (mInitializerList != nullptr)
	{
		return mInitializerList->GetContextToken();
	}
	return nullptr;
}


bool Initializer::IsResolved() const
{
	if (mExpression != nullptr)
	{
		return mExpression->GetTypeAndValue().IsResolved();
	}
	else
	{
		const Initializer *initializer = mInitializerList;
		while (initializer != nullptr)
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


const TypeSpecifier VOID_TYPE_SPECIFIER(&VOID_TOKEN);
const TypeSpecifier BOOL_TYPE_SPECIFIER(&BOOL_TOKEN);
const TypeSpecifier CHAR_TYPE_SPECIFIER(&CHAR_TOKEN);
const TypeSpecifier INT_TYPE_SPECIFIER(&INT_TOKEN);
const TypeSpecifier UINT_TYPE_SPECIFIER(&UINT_TOKEN);
const TypeSpecifier LONG_TYPE_SPECIFIER(&LONG_TOKEN);
const TypeSpecifier ULONG_TYPE_SPECIFIER(&ULONG_TOKEN);
const TypeSpecifier FLOAT_TYPE_SPECIFIER(&FLOAT_TOKEN);
const TypeSpecifier DOUBLE_TYPE_SPECIFIER(&DOUBLE_TOKEN);

}
