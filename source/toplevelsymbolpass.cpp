#include "bond/parsenodes.h"
#include "private/toplevelsymbolpass.h"

namespace Bond
{

void TopLevelSymbolPass::Visit(NamespaceDefinition *namespaceDefinition)
{
	GetOrInsertSymbol(namespaceDefinition);
	SemanticAnalysisPass::Visit(namespaceDefinition);
}


void TopLevelSymbolPass::Visit(EnumDeclaration *enumDeclaration)
{
	InsertSymbol(enumDeclaration);
	ParseNodeTraverser::Visit(enumDeclaration);
}


void TopLevelSymbolPass::Visit(Enumerator *enumerator)
{
	InsertSymbol(enumerator);
}


void TopLevelSymbolPass::Visit(StructDeclaration *structDeclaration)
{
	InsertSymbol(structDeclaration);
	SemanticAnalysisPass::Visit(structDeclaration);
}


void TopLevelSymbolPass::Visit(FunctionDefinition *functionDefinition)
{
	InsertSymbol(functionDefinition);
}


void TopLevelSymbolPass::Visit(NamedInitializer *namedInitializer)
{
	InsertSymbol(namedInitializer);
}

}
