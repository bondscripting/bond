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

	void AssertReachableCode(const ParseNode *node);

	BoolStack mHasDefaultLabel;
	BoolStack mEndsWithJump;
	BoolStack mHasReturn;
	BoolStack mIsInLoop;
	BoolStack mIsInSwitch;
	TypeStack mReturnType;
};


void ValidationPass::Analyze(TranslationUnit *translationUnitList)
{
	BoolStack::Element hasDefaultLabelElement(mHasDefaultLabel, false);
	BoolStack::Element endsWithJumpElement(mEndsWithJump, false);
	BoolStack::Element hasReturnElement(mHasReturn, false);
	BoolStack::Element isInLoopElement(mIsInLoop, false);
	BoolStack::Element isInSwitchElement(mIsInSwitch, false);
	TypeStack::Element returnTypeElement(mReturnType, 0);
	SemanticAnalysisPass::Analyze(translationUnitList);
}


void ValidationPass::Visit(FunctionDefinition *functionDefinition)
{
	const TypeDescriptor *returnType = functionDefinition->GetPrototype()->GetReturnType();
	BoolStack::Element endsWithJumpElement(mEndsWithJump, false);
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
	AssertReachableCode(ifStatement);
	mEndsWithJump.SetTop(false);
	Traverse(ifStatement->GetCondition());

	bool hasReturn = ifStatement->GetElseStatement() != 0;
	{
		BoolStack::Element hasReturnElement(mHasReturn, false);
		BoolStack::Element endsWithJumpElement(mEndsWithJump, false);
		Traverse(ifStatement->GetThenStatement());
		hasReturn = hasReturn && hasReturnElement;
	}

	{
		BoolStack::Element hasReturnElement(mHasReturn, false);
		BoolStack::Element endsWithJumpElement(mEndsWithJump, false);
		Traverse(ifStatement->GetElseStatement());
		hasReturn = hasReturn && hasReturnElement;
	}
	mHasReturn.SetTop(mHasReturn.GetTop() || hasReturn);
	mEndsWithJump.SetTop(hasReturn);
}


void ValidationPass::Visit(SwitchStatement *switchStatement)
{
	AssertReachableCode(switchStatement);
	BoolStack::Element isInSwitchElement(mIsInSwitch, true);
	mEndsWithJump.SetTop(false);
	Traverse(switchStatement->GetControl());

	bool hasReturn = true;
	SwitchSection *sectionList = switchStatement->GetSectionList();
	if (sectionList != 0)
	{
		BoolStack::Element hasDefaultLabelElement(mHasDefaultLabel, false);
		while (sectionList != 0)
		{
			BoolStack::Element hasReturnElement(mHasReturn, false);
			Traverse(sectionList);
			hasReturn = hasReturn && hasReturnElement;
			sectionList = NextNode(sectionList);
		}

		hasReturn = (hasReturn && hasDefaultLabelElement);
		mHasReturn.SetTop(mHasReturn.GetTop() || hasReturn);
	}
	mEndsWithJump.SetTop(hasReturn);
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
	if (switchLabel->GetLabel()->GetTokenType() == Token::KEY_DEFAULT)
	{
		mHasDefaultLabel.SetTop(true);
	}
}


void ValidationPass::Visit(WhileStatement *whileStatement)
{
	AssertReachableCode(whileStatement);
	BoolStack::Element isInLoopElement(mIsInLoop, true);
	mEndsWithJump.SetTop(false);
	BoolStack::Element endsWithJumpElement(mEndsWithJump, false);
	ParseNodeTraverser::Visit(whileStatement);
}


void ValidationPass::Visit(ForStatement *forStatement)
{
	AssertReachableCode(forStatement);
	BoolStack::Element isInLoopElement(mIsInLoop, true);
	mEndsWithJump.SetTop(false);
	BoolStack::Element endsWithJumpElement(mEndsWithJump, false);
	SemanticAnalysisPass::Visit(forStatement);
}


void ValidationPass::Visit(JumpStatement *jumpStatement)
{
	AssertReachableCode(jumpStatement);
	ParseNodeTraverser::Visit(jumpStatement);

	bool endsWithJump = true;
	if (jumpStatement->IsBreak())
	{
		if (!mIsInLoop.GetTop() && !mIsInSwitch.GetTop())
		{
			mErrorBuffer.PushError(ParseError::INVALID_BREAK, jumpStatement->GetContextToken());
			endsWithJump = false;
		}
	}
	else if (jumpStatement->IsContinue())
	{
		if (!mIsInLoop.GetTop())
		{
			mErrorBuffer.PushError(ParseError::INVALID_CONTINUE, jumpStatement->GetContextToken());
			endsWithJump = false;
		}
	}
	else if (jumpStatement->IsReturn())
	{
		mHasReturn.SetTop(true);
	}
	mEndsWithJump.SetTop(mEndsWithJump.GetTop() || endsWithJump);
}


void ValidationPass::Visit(DeclarativeStatement *declarativeStatement)
{
	AssertReachableCode(declarativeStatement);
	mEndsWithJump.SetTop(false);
	ParseNodeTraverser::Visit(declarativeStatement);
}


void ValidationPass::Visit(ExpressionStatement *expressionStatement)
{
	if (expressionStatement->GetExpression() != 0)
	{
		AssertReachableCode(expressionStatement);
	}
	mEndsWithJump.SetTop(false);
	ParseNodeTraverser::Visit(expressionStatement);
}


void ValidationPass::AssertReachableCode(const ParseNode *node)
{
	if (mEndsWithJump.GetTop())
	{
		mErrorBuffer.PushError(ParseError::UNREACHABLE_CODE, node->GetContextToken());
	}
}

}
