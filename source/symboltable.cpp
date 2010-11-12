#include "bond/symboltable.h"
#include "bond/token.h"

namespace Bond
{

bool SymbolBase::Matches(const Token *name) const
{
	return name->GetHashCode() == mName->GetHashCode();
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


Symbol *Scope::FindSymbol(const Token *name)
{
	Symbol *symbol = mSymbolList;

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
