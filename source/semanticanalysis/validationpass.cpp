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

	BoolStack mHasDefaultLabel;
	BoolStack mEndsWithJump;
	BoolStack mHasReturn;
	TypeStack mReturnType;
};


void ValidationPass::Analyze(TranslationUnit *translationUnitList)
{
	BoolStack::Element hasDefaultLabelElement(mHasDefaultLabel, false);
	BoolStack::Element endsWithJumpElement(mEndsWithJump, false);
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
	mEndsWithJump.SetTop(false);

	if (!mHasReturn.GetTop() && (ifStatement->GetElseStatement() != 0))
	{
		Traverse(ifStatement->GetCondition());
		bool hasReturn = false;
		{
			BoolStack::Element hasReturnElement(mHasReturn, false);
			Traverse(ifStatement->GetThenStatement());
			hasReturn = hasReturnElement;
		}

		{
			BoolStack::Element hasReturnElement(mHasReturn, false);
			Traverse(ifStatement->GetElseStatement());
			hasReturn = hasReturn && hasReturnElement;
		}
		mHasReturn.SetTop(hasReturn);
	}
	else
	{
		ParseNodeTraverser::Visit(ifStatement);
	}
}


void ValidationPass::Visit(SwitchStatement *switchStatement)
{
	mEndsWithJump.SetTop(false);
	Traverse(switchStatement->GetControl());

	SwitchSection *sectionList = switchStatement->GetSectionList();
	if (sectionList != 0)
	{
		BoolStack::Element hasDefaultLabelElement(mHasDefaultLabel, false);
		bool hasReturn = true;
		while (sectionList != 0)
		{
			BoolStack::Element hasReturnElement(mHasReturn, false);
			Traverse(sectionList);
			hasReturn = hasReturn && hasReturnElement;
			sectionList = NextNode(sectionList);
		}

		hasReturn = (hasReturn && hasDefaultLabelElement) || mHasReturn.GetTop();
		mHasReturn.SetTop(hasReturn);
	}
}


void ValidationPass::Visit(SwitchSection *switchSection)
{
	BoolStack::Element endsWithJumpElement(mEndsWithJump, false);
	SemanticAnalysisPass::Visit(switchSection);

	if (!endsWithJumpElement)
	{
		mErrorBuffer.PushError(ParseError::UNTERMINATED_SWITCH_SECTION, switchSection->GetLabelList()->GetContextToken());
	}
}


void ValidationPass::Visit(SwitchLabel *switchLabel)
{
	ParseNodeTraverser::Visit(switchLabel);
	if (switchLabel->GetVariant() == SwitchLabel::VARIANT_DEFAULT)
	{
		mHasDefaultLabel.SetTop(true);
	}
}


void ValidationPass::Visit(WhileStatement *whileStatement)
{
	mEndsWithJump.SetTop(false);
	ParseNodeTraverser::Visit(whileStatement);
}


void ValidationPass::Visit(ForStatement *forStatement)
{
	mEndsWithJump.SetTop(false);
	SemanticAnalysisPass::Visit(forStatement);
}


void ValidationPass::Visit(JumpStatement *jumpStatement)
{
	mEndsWithJump.SetTop(true);
	ParseNodeTraverser::Visit(jumpStatement);
	if (jumpStatement->GetOperator()->GetTokenType() == Token::KEY_RETURN)
	{
		mHasReturn.SetTop(true);
	}
}


void ValidationPass::Visit(DeclarativeStatement *declarativeStatement)
{
	mEndsWithJump.SetTop(false);
	ParseNodeTraverser::Visit(declarativeStatement);
}


void ValidationPass::Visit(ExpressionStatement *expressionStatement)
{
	mEndsWithJump.SetTop(false);
	ParseNodeTraverser::Visit(expressionStatement);
}

}
