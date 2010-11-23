#ifndef BOND_SYMBOLTABLE_H
#define BOND_SYMBOLTABLE_H

#include "bond/conf.h"

namespace Bond
{

class ParseNode;
class QualifiedIdentifier;
class Token;

class Symbol
{
public:
	enum Type
	{
		TYPE_NAMESPACE,
		TYPE_STRUCT,
		TYPE_ENUM,
		//TYPE_LOCALSCOPE,
		TYPE_CONSTANT,
		//TYPE_VARIABLE,
		TYPE_FUNCTION,
	};

	Symbol(Type type, const Token *name, const ParseNode *definition, Symbol *parent):
		mType(type),
		mName(name),
		mDefinition(definition),
		mNext(0),
		mParent(parent),
		mSymbolList(0)
	{}

	Type GetType() const { return mType; }
	const Token *GetName() const { return mName; }

	const ParseNode *GetDefinition() const { return mDefinition; }
	void SetDefinition(const ParseNode *definition) { mDefinition = definition; }

	Symbol *GetNext() { return mNext; }
	const Symbol *GetNext() const { return mNext; }
	void SetNext(Symbol *next) { mNext = next; }

	Symbol *GetParent() { return mParent; }

	const Symbol *FindSymbol(const char *name) const;

	Symbol *FindSymbol(const Token *name);
	const Symbol *FindSymbol(const Token *name) const;

	Symbol *FindSymbol(const QualifiedIdentifier *identifier);
	const Symbol *FindSymbol(const QualifiedIdentifier *identifier) const;

	void InsertSymbol(Symbol *symbol);

	bool Matches(bu32_t hashCode, const char *name) const;
	bool Matches(const Token *name) const;

private:
	Symbol *FindQualifiedSymbol(const QualifiedIdentifier *identifier);
	const Symbol *FindQualifiedSymbol(const QualifiedIdentifier *identifier) const;

	Type mType;
	const Token *mName;
	const ParseNode *mDefinition;
	//bu32_t mQualifiedHash;
	Symbol *mNext;
	Symbol *mParent;
	Symbol *mSymbolList;
};


class SymbolTable
{
public:
	SymbolTable(): mGlobalScope(Symbol::TYPE_NAMESPACE, 0, 0, 0) {}

	Symbol *GetGlobalScope() { return &mGlobalScope; }
	const Symbol *GetGlobalScope() const { return &mGlobalScope; }

private:
	Symbol mGlobalScope;
};

}

#endif
