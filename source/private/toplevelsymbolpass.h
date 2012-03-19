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

	virtual void Visit(NamespaceDefinition *namespaceDefinition);
	virtual void Visit(EnumDeclaration *enumDeclaration);
	virtual void Visit(Enumerator *enumerator);
	virtual void Visit(StructDeclaration *structDeclaration);
	virtual void Visit(FunctionDefinition *functionDefinition);
	virtual void Visit(NamedInitializer *namedInitializer);
};

}

#endif
