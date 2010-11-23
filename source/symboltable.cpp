#include "bond/parsenodes.h"
#include "bond/symboltable.h"
#include "bond/token.h"
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
		symbol = symbol->GetNext();
	}

	return symbol;
}


Symbol *Symbol::FindSymbol(const Token *name)
{
	Symbol *symbol = mSymbolList;

	while ((symbol != 0) && !symbol->Matches(name))
	{
		symbol = symbol->GetNext();
	}

	return symbol;
}


const Symbol *Symbol::FindSymbol(const Token *name) const
{
	const Symbol *symbol = mSymbolList;

	while ((symbol != 0) && !symbol->Matches(name))
	{
		symbol = symbol->GetNext();
	}

	return symbol;
}


Symbol *Symbol::FindSymbol(const QualifiedIdentifier *identifier)
{
	Symbol *symbol = FindQualifiedSymbol(identifier);

	if ((symbol == 0) && (mParent != 0))
	{
		symbol = mParent->FindSymbol(identifier);
	}

	return symbol;
}


const Symbol *Symbol::FindSymbol(const QualifiedIdentifier *identifier) const
{
	const Symbol *symbol = FindQualifiedSymbol(identifier);

	if ((symbol == 0) && (mParent != 0))
	{
		symbol = mParent->FindSymbol(identifier);
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
	else if (mType == TYPE_NAMESPACE)
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
	else if (mType == TYPE_NAMESPACE)
	{
		const Symbol *nextScope = FindSymbol(identifier->GetName());
		const QualifiedIdentifier *nextIdentifier = identifier->GetNextIdentifier();
		symbol = nextScope->FindQualifiedSymbol(nextIdentifier);
	}

	return symbol;
}


void Symbol::InsertSymbol(Symbol *symbol)
{
	symbol->SetNext(mSymbolList);
	mSymbolList = symbol;
}


bool Symbol::Matches(bu32_t hashCode, const char *name) const
{
	return (hashCode == mName->GetHashCode()) &&
		(strcmp(name, mName->GetText()) == 0);
}


bool Symbol::Matches(const Token *name) const
{
	return (name->GetHashCode() == mName->GetHashCode()) &&
		(strcmp(name->GetText(), mName->GetText()) == 0);
}

}
