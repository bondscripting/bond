#ifndef PRIVATE_BOND_TYPESPECIFIERRESOLUTIONPASS_H
#define PRIVATE_BOND_TYPESPECIFIERRESOLUTIONPASS_H

#include "private/semanticanalysispass.h"

namespace Bond
{

class TypeSpecifierResolutionPass: public SemanticAnalysisPass
{
public:
	TypeSpecifierResolutionPass(CompilerErrorBuffer &errorBuffer, SymbolTable &symbolTable):
		SemanticAnalysisPass(errorBuffer, symbolTable)
	{}

	virtual ~TypeSpecifierResolutionPass() {}

protected:
	using SemanticAnalysisPass::Visit;
	virtual void Visit(StructDeclaration *structDeclaration) override;
	virtual void Visit(TypeSpecifier *typeSpecifier) override;
	virtual void Visit(CompoundStatement *compoundStatement) override;
	virtual void Visit(SwitchSection *switchSection) override;
	virtual void Visit(ForStatement *forStatement) override;
};

}

#endif
