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
	typedef AutoStack<const TypeDescriptor *> TypeStack;

	BoolStack mHasReturn;
	TypeStack mReturnType;
};


void ValidationPass::Analyze(TranslationUnit *translationUnitList)
{
	BoolStack::Element hasReturnElement(mHasReturn, false);
	TypeStack::Element returnTypeElement(mReturnType, 0);
	SemanticAnalysisPass::Analyze(translationUnitList);
}


void ValidationPass::Visit(FunctionDefinition *functionDefinition)
{
	const TypeDescriptor *returnType = functionDefinition->GetPrototype()->GetReturnType();
	BoolStack::Element hasReturnElement(mHasReturn, false);
	TypeStack::Element returnTypeElement(mReturnType, returnType);
	SemanticAnalysisPass::Visit(functionDefinition);

	if (!returnType->IsVoidType() && !hasReturnElement && (functionDefinition->GetBody() != 0))
	{
		mErrorBuffer.PushError(ParseError::NOT_ALL_PATHS_RETURN_A_VALUE, functionDefinition->GetName());
	}
}


void ValidationPass::Visit(CompoundStatement *compoundStatement)
{
	SemanticAnalysisPass::Visit(compoundStatement);
}


void ValidationPass::Visit(IfStatement *ifStatement)
{
	Traverse(ifStatement->GetCondition());
	bool hasReturn = false;
	{
		BoolStack::Element hasReturnElement(mHasReturn, false);
		Traverse(ifStatement->GetThenStatement());
		hasReturn = mHasReturn.GetTop();
	}

	if (ifStatement->GetElseStatement() != 0)
	{
		BoolStack::Element hasReturnElement(mHasReturn, false);
		Traverse(ifStatement->GetElseStatement());
		hasReturn = hasReturn && mHasReturn.GetTop();
	}
	mHasReturn.SetTop(hasReturn || mHasReturn.GetTop());
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
	if (jumpStatement->GetOperator()->GetTokenType() == Token::KEY_RETURN)
	{
		mHasReturn.SetTop(true);
	}
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
