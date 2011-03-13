#include "bond/parsenodes.h"
#include "bond/stringutil.h"
#include <string.h>

namespace Bond
{

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
	else
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


Token::TokenType TypeDescriptor::GetPrimitiveType() const
{
	if (mTypeSpecifier != 0)
	{
		return mTypeSpecifier->GetPrimitiveType();
	}
	return Token::INVALID;
}


bool TypeDescriptor::IsBooleanType() const
{
	if (mTypeSpecifier != 0)
	{
		return mTypeSpecifier->IsBooleanType();
	}
	return false;
}


bool TypeDescriptor::IsIntegerType() const
{
	if (mTypeSpecifier != 0)
	{
		return mTypeSpecifier->IsIntegerType();
	}
	return false;
}


bool TypeDescriptor::IsNumericType() const
{
	if (mTypeSpecifier != 0)
	{
		return mTypeSpecifier->IsNumericType();
	}
	return false;
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
	return Token::INVALID;
}


bool TypeSpecifier::IsIntegerType() const
{
	if (mPrimitiveType != 0)
	{
		return INTEGER_TYPE_SPECIFIERS_TYPESET.Contains(mPrimitiveType->GetTokenType());
	}
	return Token::INVALID;
}


bool TypeSpecifier::IsNumericType() const
{
	if (mPrimitiveType != 0)
	{
		return NUMERIC_TYPE_SPECIFIERS_TYPESET.Contains(mPrimitiveType->GetTokenType());
	}
	return Token::INVALID;
}


const Token *IdentifierExpression::GetContextToken() const
{
	return (mIdentifier != 0) ? mIdentifier->GetContextToken() : 0;
}


const TypeSpecifier BOOL_TYPE_SPECIFIER(&BOOL_TOKEN);
const TypeSpecifier CHAR_TYPE_SPECIFIER(&CHAR_TOKEN);
const TypeSpecifier INT_TYPE_SPECIFIER(&INT_TOKEN);
const TypeSpecifier UINT_TYPE_SPECIFIER(&UINT_TOKEN);
const TypeSpecifier FLOAT_TYPE_SPECIFIER(&FLOAT_TOKEN);

const TypeDescriptor CONST_BOOL_TYPE_DESCRIPTOR(&BOOL_TYPE_SPECIFIER, true);
const TypeDescriptor CONST_CHAR_TYPE_DESCRIPTOR(&CHAR_TYPE_SPECIFIER, true);
const TypeDescriptor CONST_INT_TYPE_DESCRIPTOR(&INT_TYPE_SPECIFIER, true);
const TypeDescriptor CONST_UINT_TYPE_DESCRIPTOR(&UINT_TYPE_SPECIFIER, true);
const TypeDescriptor CONST_FLOAT_TYPE_DESCRIPTOR(&FLOAT_TYPE_SPECIFIER, true);
const TypeDescriptor CONST_STRING_TYPE_DESCRIPTOR(&CONST_CHAR_TYPE_DESCRIPTOR, true);

}
