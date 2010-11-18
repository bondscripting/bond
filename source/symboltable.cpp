#include "bond/symboltable.h"
#include "bond/token.h"
#include "bond/util.h"
#include <string.h>

namespace Bond
{

bool SymbolBase::Matches(bu32_t hashCode, const char *name) const
{
	return (hashCode == mName->GetHashCode()) &&
		(strcmp(name, mName->GetText()) == 0);
}


bool SymbolBase::Matches(const Token *name) const
{
	return (name->GetHashCode() == mName->GetHashCode()) &&
		(strcmp(name->GetText(), mName->GetText()) == 0);
}


const Scope *Scope::FindScope(const char *name) const
{
	const bu32_t hashCode = Util::StringHash(name);
	const Scope *scope = mScopeList;

	while ((scope != 0) && !scope->Matches(hashCode, name))
	{
		scope = scope->GetNext();
	}

	return scope;
}


Scope *Scope::FindScope(const Token *name)
{
	Scope *scope = mScopeList;

	while ((scope != 0) && !scope->Matches(name))
	{
		scope = scope->GetNext();
	}

	return scope;
}


const Scope *Scope::FindScope(const Token *name) const
{
	const Scope *scope = mScopeList;

	while ((scope != 0) && !scope->Matches(name))
	{
		scope = scope->GetNext();
	}

	return scope;
}


const Symbol *Scope::FindSymbol(const char *name) const
{
	const bu32_t hashCode = Util::StringHash(name);
	const Symbol *symbol = mSymbolList;

	while ((symbol != 0) && !symbol->Matches(hashCode, name))
	{
		symbol = symbol->GetNext();
	}

	return symbol;
}


Symbol *Scope::FindSymbol(const Token *name)
{
	Symbol *symbol = mSymbolList;

	while ((symbol != 0) && !symbol->Matches(name))
	{
		symbol = symbol->GetNext();
	}

	return symbol;
}


const Symbol *Scope::FindSymbol(const Token *name) const
{
	const Symbol *symbol = mSymbolList;

	while ((symbol != 0) && !symbol->Matches(name))
	{
		symbol = symbol->GetNext();
	}

	return symbol;
}


void Scope::InsertScope(Scope *scope)
{
	scope->SetNext(mScopeList);
	mScopeList = scope;
}


void Scope::InsertSymbol(Symbol *symbol)
{
	symbol->SetNext(mSymbolList);
	mSymbolList = symbol;
}

}
