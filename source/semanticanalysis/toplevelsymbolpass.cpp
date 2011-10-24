namespace Bond
{

class TopLevelSymbolPass: public SemanticAnalysisPass
{
public:
	TopLevelSymbolPass(ParseErrorBuffer &errorBuffer, SymbolTable &symbolTable):
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
	Symbol *symbol = GetOrInsertSymbol(functionDefinition);
	FunctionDefinition *prev = CastNode<FunctionDefinition>(symbol);

	// TODO: Remove this stuff. Top-level function declarations don't exist anymore.
	// If we got a different symbol from GetOrInsertSymbol, then we have a previous occurence.
	if (prev != functionDefinition)
	{
		prev->AppendDefinition(functionDefinition);
		Symbol *parent = GetCurrentScope();
		functionDefinition->SetParentSymbol(parent);
	}
}


void TopLevelSymbolPass::Visit(NamedInitializer *namedInitializer)
{
	InsertSymbol(namedInitializer);
}

}
