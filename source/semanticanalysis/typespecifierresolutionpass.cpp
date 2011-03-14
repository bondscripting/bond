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


void TypeSpecifierResolutionPass::Visit(TypeSpecifier *typeSpecifier)
{
	const QualifiedIdentifier *identifier = typeSpecifier->GetIdentifier();
	if (identifier != 0)
	{
		const Symbol *symbol = GetSymbol(identifier);
		if (symbol == 0)
		{
			mErrorBuffer.PushError(ParseError::SYMBOL_IS_NOT_DEFINED, identifier->GetContextToken(), identifier);
		}
		else if (!symbol->IsTypeDefinition())
		{
			mErrorBuffer.PushError(ParseError::SYMBOL_IS_NOT_A_TYPE, identifier->GetContextToken(), identifier);
		}
		else
		{
			typeSpecifier->SetDefinition(symbol);
		}
	}
}


void TypeSpecifierResolutionPass::Visit(CompoundStatement *compoundStatement)
{
	Symbol *parent = GetCurrentScope();
	compoundStatement->SetParentSymbol(parent);
	SemanticAnalysisPass::Visit(compoundStatement);
}


void TypeSpecifierResolutionPass::Visit(SwitchSection *switchSection)
{
	Symbol *parent = GetCurrentScope();
	switchSection->SetParentSymbol(parent);
	SemanticAnalysisPass::Visit(switchSection);
}


void TypeSpecifierResolutionPass::Visit(ForStatement *forStatement)
{
	Symbol *parent = GetCurrentScope();
	forStatement->SetParentSymbol(parent);
	SemanticAnalysisPass::Visit(forStatement);
}

}
