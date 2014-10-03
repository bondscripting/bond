#ifndef PRIVATE_BOND_TOPLEVELSYMBOLPASS_H
#define PRIVATE_BOND_TOPLEVELSYMBOLPASS_H

#include "private/semanticanalysispass.h"

namespace Bond
{

class TopLevelSymbolPass: public SemanticAnalysisPass
{
public:
	TopLevelSymbolPass(CompilerErrorBuffer &errorBuffer, SymbolTable &symbolTable):
		SemanticAnalysisPass(errorBuffer, symbolTable)
	{}

	virtual ~TopLevelSymbolPass() {}

protected:
	using SemanticAnalysisPass::Visit;
	virtual void Visit(EnumDeclaration *enumDeclaration) override;
	virtual void Visit(Enumerator *enumerator) override;
	virtual void Visit(StructDeclaration *structDeclaration) override;
	virtual void Visit(FunctionDefinition *functionDefinition) override;
	virtual void Visit(NamedInitializer *namedInitializer) override;
};

}

#endif
