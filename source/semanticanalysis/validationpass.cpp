namespace Bond
{

class ValidationPass: public SemanticAnalysisPass
{
public:
	ValidationPass(ParseErrorBuffer &errorBuffer, SymbolTable &symbolTable):
		SemanticAnalysisPass(errorBuffer, symbolTable)
	{}

	virtual ~ValidationPass() {}

	virtual void Analyze(TranslationUnit *translationUnitList);

protected:
	virtual void Visit(FunctionDefinition *functionDefinition);
	virtual void Visit(CompoundStatement *compoundStatement);
	virtual void Visit(IfStatement *ifStatement);
	virtual void Visit(SwitchStatement *switchStatement);
	virtual void Visit(SwitchSection *switchSection);
	virtual void Visit(SwitchLabel *switchLabel);
	virtual void Visit(WhileStatement *whileStatement);
	virtual void Visit(ForStatement *forStatement);
	virtual void Visit(JumpStatement *jumpStatement);
	virtual void Visit(DeclarativeStatement *declarativeStatement);
	virtual void Visit(ExpressionStatement *expressionStatement);

private:
};

void ValidationPass::Analyze(TranslationUnit *translationUnitList)
{
	SemanticAnalysisPass::Analyze(translationUnitList);
}


void ValidationPass::Visit(FunctionDefinition *functionDefinition)
{
	SemanticAnalysisPass::Visit(functionDefinition);
}


void ValidationPass::Visit(CompoundStatement *compoundStatement)
{
	SemanticAnalysisPass::Visit(compoundStatement);
}


void ValidationPass::Visit(IfStatement *ifStatement)
{
	ParseNodeTraverser::Visit(ifStatement);
}


void ValidationPass::Visit(SwitchStatement *switchStatement)
{
	ParseNodeTraverser::Visit(switchStatement);
}


void ValidationPass::Visit(SwitchSection *switchSection)
{
	SemanticAnalysisPass::Visit(switchSection);
}


void ValidationPass::Visit(SwitchLabel *switchLabel)
{
	ParseNodeTraverser::Visit(switchLabel);
}


void ValidationPass::Visit(WhileStatement *whileStatement)
{
	ParseNodeTraverser::Visit(whileStatement);
}


void ValidationPass::Visit(ForStatement *forStatement)
{
	SemanticAnalysisPass::Visit(forStatement);
}


void ValidationPass::Visit(JumpStatement *jumpStatement)
{
	ParseNodeTraverser::Visit(jumpStatement);
}


void ValidationPass::Visit(DeclarativeStatement *declarativeStatement)
{
	ParseNodeTraverser::Visit(declarativeStatement);
}


void ValidationPass::Visit(ExpressionStatement *expressionStatement)
{
	ParseNodeTraverser::Visit(expressionStatement);
}

}
