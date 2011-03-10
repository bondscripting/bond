#ifndef BOND_SYMBOLTABLE_H
#define BOND_SYMBOLTABLE_H

#include "bond/parsenodes.h"

namespace Bond
{

class SymbolTable
{
public:
	SymbolTable() {}

	Symbol *GetGlobalScope() { return &mGlobalScope; }
	const Symbol *GetGlobalScope() const { return &mGlobalScope; }

private:
	class GlobalScope: public Symbol
	{
	public:
		virtual void Accept(ParseNodeVisitor &visitor) {}
		virtual void Accept(ParseNodeVisitor &visitor) const {}
		virtual SymbolType GetSymbolType() const { return TYPE_NAMESPACE; }
		virtual const Token *GetName() const { return 0; }
	};

	GlobalScope mGlobalScope;
};

}

#endif
