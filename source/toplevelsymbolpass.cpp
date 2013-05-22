#include "private/toplevelsymbolpass.h"
#include "bond/compiler/parsenodes.h"

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
	if (structDeclaration->GetVariant() != StructDeclaration::VARIANT_NATIVE_STUB)
	{
		InsertSymbol(structDeclaration);
	}
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
