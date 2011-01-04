namespace Bond
{

class TypeAndConstantDeclarationPass: public SemanticAnalysisPass
{
public:
	TypeAndConstantDeclarationPass(ParseErrorBuffer &errorBuffer, Allocator &allocator, SymbolTable &symbolTable):
		SemanticAnalysisPass(errorBuffer, allocator, symbolTable)
	{}

	virtual ~TypeAndConstantDeclarationPass() {}

	virtual void Visit(NamespaceDefinition *namespaceDefinition);
	virtual void Visit(EnumDeclaration *enumDeclaration);
	virtual void Visit(Enumerator *enumerator);
	virtual void Visit(StructDeclaration *structDeclaration);
	virtual void Visit(FunctionDefinition *functionDefinition) {}
	virtual void Visit(NamedInitializer *namedInitializer);
};


void TypeAndConstantDeclarationPass::Visit(NamespaceDefinition *namespaceDefinition)
{
	const Token *name = namespaceDefinition->GetName();
	Symbol *symbol = GetOrInsertSymbol(Symbol::TYPE_NAMESPACE, name, namespaceDefinition);
	namespaceDefinition->SetSymbol(symbol);
	SemanticAnalysisPass ::Visit(namespaceDefinition);
}


void TypeAndConstantDeclarationPass::Visit(EnumDeclaration *enumDeclaration)
{
	const Token *name = enumDeclaration->GetName();
	Symbol *symbol = InsertSymbol(Symbol::TYPE_ENUM, name, enumDeclaration);
	enumDeclaration->SetSymbol(symbol);
	ParseNodeTraverser::Visit(enumDeclaration);
}


void TypeAndConstantDeclarationPass::Visit(Enumerator *enumerator)
{
	const Token *name = enumerator->GetName();
	Symbol *symbol = InsertSymbol(Symbol::TYPE_CONSTANT, name, enumerator);
	enumerator->SetSymbol(symbol);
	TypeAndValue &tav = symbol->GetTypeAndValue();
	const TypeDescriptor *descriptor = enumerator->GetParent()->GetTypeDescriptor();
	tav.SetTypeDescriptor(descriptor);
}


void TypeAndConstantDeclarationPass::Visit(StructDeclaration *structDeclaration)
{
	const Token *name = structDeclaration->GetName();
	Symbol *symbol = InsertSymbol(Symbol::TYPE_STRUCT, name, structDeclaration);
	structDeclaration->SetSymbol(symbol);
}

/*
void TypeAndConstantDeclarationPass::Visit(FunctionDefinition *functionDefinition)
{
	const FunctionPrototype *currentPrototype = functionDefinition->GetPrototype();
	const Token *name = currentPrototype->GetName();
	Symbol *parent = GetCurrentScope();

	Symbol *symbol = parent->FindSymbol(name);
	if (symbol != 0)
	{
		const FunctionDefinition *previousFunction = CastNode<FunctionDefinition>(symbol->GetDefinition());
		const FunctionPrototype *previousPrototype = previousFunction->GetPrototype();

		if (!AreMatchingFunctionPrototypes(parent, currentPrototype, previousPrototype))
		{
			mErrorBuffer.PushError(ParseError::FUNCTION_PROTOTYPE_MISMATCH, name, symbol->GetName());
		}
		else if (!functionDefinition->IsDeclaration())
		{
			// We've found a definition for a previously declared function. Replace the declaration.
			if (previousFunction->IsDeclaration())
			{
				symbol->SetDefinition(functionDefinition);
			}
			// We have two definitions for the same function.
			else
			{
				mErrorBuffer.PushError(ParseError::DUPLICATE_FUNCTION_DEFINITION, name, symbol->GetName());
			}
		}
	}
	else
	{
		InsertSymbol(Symbol::TYPE_FUNCTION, name, functionDefinition, parent);
	}
}
*/

void TypeAndConstantDeclarationPass::Visit(NamedInitializer *namedInitializer)
{
	// TODO: Assert that type is a valid top-level constant type.
	// Assert that the expression is a constant expression.
	const Token *name = namedInitializer->GetName();
	Symbol *symbol = InsertSymbol(Symbol::TYPE_CONSTANT, name, namedInitializer);
	//namedInitializer->SetSymbol(symbol);
	TypeAndValue &tav = symbol->GetTypeAndValue();
	const TypeDescriptor *descriptor = namedInitializer->GetTypeDescriptor();
	tav.SetTypeDescriptor(descriptor);
}

}
