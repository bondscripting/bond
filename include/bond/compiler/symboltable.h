#ifndef BOND_COMPILER_SYMBOLTABLE_H
#define BOND_COMPILER_SYMBOLTABLE_H

#include "bond/compiler/parsenodes.h"

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
		virtual void Accept(ParseNodeVisitor &visitor) override {}
		virtual void Accept(ParseNodeVisitor &visitor) const override {}
		virtual SymbolType GetSymbolType() const override { return TYPE_NAMESPACE; }
	};

	GlobalScope mGlobalScope;
};

}

#endif
