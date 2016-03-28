#ifndef BOND_COMPILER_SYMBOLTABLE_H
#define BOND_COMPILER_SYMBOLTABLE_H

#include "bond/compiler/parsenodes.h"

namespace Bond
{

/// \brief A heirarchical symbol table used by the Bond compiler.
///
/// A SymbolTable is a data structure used by the SemanticAnalyzer to index and look up
/// information about all identifiers it encounters during its analysis of Bond source code.
/// The symbols are indexed heirarchically according to the nesting of scopes in which they
/// appear (e.g. namespaces, struct declarations, local scopes and so forth). Since symbols
/// representing scopes contain the index for the symbols declared within their scope, the
/// SymbolTable is little more than a container for a single anonymous namespace called the
/// global scope.
///
/// \sa SemanticAnalyzer, Symbol
/// \ingroup compiler
class SymbolTable
{
public:
	SymbolTable() {}

	/// \brief Returns the Symbol for the global scope namespace.
	Symbol *GetGlobalScope() { return &mGlobalScope; }

	/// \brief Returns the Symbol for the global scope namespace.
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
