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
