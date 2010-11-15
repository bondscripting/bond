#ifndef BOND_SYMBOLTABLE_H
#define BOND_SYMBOLTABLE_H

#include "bond/conf.h"

namespace Bond
{

class ParseNode;
class Token;

class SymbolBase
{
public:
	enum Type
	{
		TYPE_NAMESPACE,
		TYPE_STRUCT,
		TYPE_ENUM,
		//TYPE_LOCALSCOPE,
		TYPE_VALUE,
		//TYPE_VARIABLE,
		//TYPE_FUNCTION,
	};

	Type GetType() const { return mType; }
	const Token *GetName() const { return mName; }
	const ParseNode *GetDefinition() const { return mDefinition; }

	bool Matches(const Token *name) const;

protected:
	SymbolBase(Type type, const Token *name, const ParseNode *definition):
		mType(type),
		mName(name),
		mDefinition(definition)
	{}

private:
	Type mType;
	const Token *mName;
	const ParseNode *mDefinition;
	bu32_t mQualifiedHash;
};


class Symbol: public SymbolBase
{
public:
	Symbol(Type type, const Token *name, const ParseNode *definition):
		SymbolBase(type, name, definition),
		mNext(0)
	{}

	Symbol *GetNext() { return mNext; }
	void SetNext(Symbol *next) { mNext = next; }

private:
	Symbol *mNext;
};


class Scope: public SymbolBase
{
public:
	Scope(Type type, const Token *name, const ParseNode *definition, Scope *parent):
		SymbolBase(type, name, definition),
		mNext(0),
		mParent(parent),
		mScopeList(0),
		mSymbolList(0)
	{}

	Scope *GetNext() { return mNext; }
	void SetNext(Scope *next) { mNext = next; }

	Scope *GetParent() { return mParent; }

	Scope *FindScope(const Token *name);
	Symbol *FindSymbol(const Token *name);

	void InsertScope(Scope *scope);
	void InsertSymbol(Symbol *symbol);

private:
	Scope *mNext;
	Scope *mParent;
	Scope *mScopeList;
	Symbol *mSymbolList;
};


class SymbolTable
{
public:
	SymbolTable(): mGlobalScope(SymbolBase::TYPE_NAMESPACE, 0, 0, 0) {}

	Scope *GetGlobalScope() { return &mGlobalScope; }

private:
	Scope mGlobalScope;
};

}

#endif
