#ifndef PRIVATE_BOND_TYPESPECIFIERRESOLUTIONPASS_H
#define PRIVATE_BOND_TYPESPECIFIERRESOLUTIONPASS_H

#include "private/semanticanalysispass.h"

namespace Bond
{

class TypeSpecifierResolutionPass: public SemanticAnalysisPass
{
public:
	TypeSpecifierResolutionPass(ParseErrorBuffer &errorBuffer, SymbolTable &symbolTable):
		SemanticAnalysisPass(errorBuffer, symbolTable)
	{}

	virtual ~TypeSpecifierResolutionPass() {}

	virtual void Visit(TypeSpecifier *typeSpecifier);
	virtual void Visit(CompoundStatement *compoundStatement);
	virtual void Visit(SwitchSection *switchSection);
	virtual void Visit(ForStatement *forStatement);
};

}

#endif
