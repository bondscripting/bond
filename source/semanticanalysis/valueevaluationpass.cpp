namespace Bond
{

class ValueEvaluationPass: public SemanticAnalysisPass
{
public:
	virtual ~ValueEvaluationPass() {}

	virtual void Analyze(TranslationUnit *translationUnitList);

protected:
	ValueEvaluationPass(ParseErrorBuffer &errorBuffer, SymbolTable &symbolTable):
		SemanticAnalysisPass(errorBuffer, symbolTable)
	{}

	virtual void Visit(ConditionalExpression *conditionalExpression);
	virtual void Visit(BinaryExpression *binaryExpression);
	virtual void Visit(UnaryExpression *unaryExpression);
	//virtual void Visit(PostfixExpression *postfixExpression);
	//virtual void Visit(MemberExpression *memberExpression);
	//virtual void Visit(ArraySubscriptExpression *arraySubscriptExpression);
	//virtual void Visit(FunctionCallExpression *functionCallExpression);
	//virtual void Visit(CastExpression *castExpression);
	//virtual void Visit(SizeofExpression *sizeofExpression);
	virtual void Visit(ConstantExpression *constantExpression);
	//virtual void Visit(IdentifierExpression *identifierExpression);

private:
	bool mMadeChanges;
};


class TopLevelValueEvaluationPass: public ValueEvaluationPass
{
public:
	TopLevelValueEvaluationPass(ParseErrorBuffer &errorBuffer, SymbolTable &symbolTable):
		ValueEvaluationPass(errorBuffer, symbolTable)
	{}

	~TopLevelValueEvaluationPass() {}

protected:
	// This pass doesn't descend into structs and functions.
	virtual void Visit(StructDeclaration *structDeclaration) {}
	virtual void Visit(FunctionDefinition *functionDefinition) {}
};


void ValueEvaluationPass::Analyze(TranslationUnit *translationUnitList)
{
	/*
	do
	{
		mMadeChanges = false;
		SemanticAnalysisPass::Analyze(translationUnitList);
	}
	while (mMadeChanges);
	*/
}


void ValueEvaluationPass::Visit(ConditionalExpression *conditionalExpression)
{
	TypeAndValue &tav = conditionalExpression->GetTypeAndValue();

	if (!tav.IsResolved())
	{
		ParseNodeTraverser::Visit(conditionalExpression);
		const TypeAndValue &condTav = conditionalExpression->GetCondition()->GetTypeAndValue();
		const TypeAndValue &trueTav = conditionalExpression->GetTrueExpression()->GetTypeAndValue();
		const TypeAndValue &falseTav = conditionalExpression->GetFalseExpression()->GetTypeAndValue();

		if (condTav.IsResolved() && trueTav.IsResolved() && falseTav.IsResolved())
		{
			tav.Resolve();

			if (condTav.IsValueDefined() && trueTav.IsValueDefined() && falseTav.IsValueDefined())
			{
				const bool cond = condTav.GetBoolValue();
				const TypeDescriptor *resultType = tav.GetTypeDescriptor();
				tav.SetValue(CastValue(cond ? trueTav : falseTav, resultType));
			}
		}
	}
}


void ValueEvaluationPass::Visit(BinaryExpression *binaryExpression)
{
	TypeAndValue &tav = binaryExpression->GetTypeAndValue();

	if (!tav.IsResolved())
	{
		ParseNodeTraverser::Visit(binaryExpression);
		const TypeAndValue &lhs = binaryExpression->GetRhs()->GetTypeAndValue();
		const TypeAndValue &rhs = binaryExpression->GetRhs()->GetTypeAndValue();

		if (lhs.IsResolved() && rhs.IsResolved())
		{
			tav.Resolve();

			if (lhs.IsValueDefined() && rhs.IsValueDefined())
			{
				const Token *op = binaryExpression->GetOperator();
				const TypeDescriptor *resultType = tav.GetTypeDescriptor();

				switch (op->GetTokenType())
				{
					case Token::COMMA:
						tav.SetValue(rhs.GetValue());
						break;
					case Token::OP_PLUS:
						tav.SetValue(BinaryAdd(lhs, rhs, resultType));
						break;
					case Token::OP_MINUS:
						tav.SetValue(BinarySub(lhs, rhs, resultType));
						break;
					case Token::OP_MULT:
						tav.SetValue(BinaryMult(lhs, rhs, resultType));
						break;
					case Token::OP_DIV:
						tav.SetValue(BinaryDiv(lhs, rhs, resultType));
						break;
					case Token::OP_MOD:
						tav.SetValue(BinaryMod(lhs, rhs, resultType));
						break;
					case Token::OP_LEFT:
						tav.SetValue(BinaryLeft(lhs, rhs, resultType));
						break;
					case Token::OP_RIGHT:
						tav.SetValue(BinaryRight(lhs, rhs, resultType));
						break;
					case Token::OP_BIT_AND:
						tav.SetValue(BinaryBitAnd(lhs, rhs, resultType));
						break;
					case Token::OP_BIT_OR:
						tav.SetValue(BinaryBitOr(lhs, rhs, resultType));
						break;
					case Token::OP_BIT_XOR:
						tav.SetValue(BinaryBitXOr(lhs, rhs, resultType));
						break;
					case Token::OP_LT:
						tav.SetValue(BinaryLT(lhs, rhs));
						break;
					case Token::OP_LTE:
						tav.SetValue(BinaryLTE(lhs, rhs));
						break;
					case Token::OP_GT:
						tav.SetValue(BinaryGT(lhs, rhs));
						break;
					case Token::OP_GTE:
						tav.SetValue(BinaryGTE(lhs, rhs));
						break;
					case Token::OP_EQUAL:
						tav.SetValue(BinaryEqual(lhs, rhs));
						break;
					case Token::OP_NOT_EQUAL:
						tav.SetValue(BinaryNotEqual(lhs, rhs));
						break;
					case Token::OP_AND:
						tav.SetBoolValue(lhs.GetBoolValue() && rhs.GetBoolValue());
						break;
					case Token::OP_OR:
						tav.SetBoolValue(lhs.GetBoolValue() || rhs.GetBoolValue());
						break;
					default:
						break;
				}
			}
		}
	}
}


void ValueEvaluationPass::Visit(UnaryExpression *unaryExpression)
{
	TypeAndValue &tav = unaryExpression->GetTypeAndValue();

	if (!tav.IsResolved())
	{
		ParseNodeTraverser::Visit(unaryExpression);
		const TypeAndValue &rhs = unaryExpression->GetRhs()->GetTypeAndValue();

		if (rhs.IsResolved())
		{
			tav.Resolve();

			if (rhs.IsValueDefined())
			{
				const Token *op = unaryExpression->GetOperator();

				switch (op->GetTokenType())
				{
					case Token::OP_PLUS:
						tav.SetValue(rhs.GetValue());
						break;
					case Token::OP_MINUS:
						tav.SetValue(UnaryMinus(rhs));
						break;
					case Token::OP_NOT:
						tav.SetBoolValue(!rhs.GetBoolValue());
						break;
					case Token::OP_BIT_AND:
						// TODO
						break;
					case Token::OP_BIT_NOT:
						break;
					case Token::OP_MULT:
						// TODO
						break;
					default:
						break;
				}
			}
		}
	}
}


void ValueEvaluationPass::Visit(ConstantExpression *constantExpression)
{
	const Token *token = constantExpression->GetValueToken();
	TypeAndValue &tav = constantExpression->GetTypeAndValue();
	tav.Resolve();
	tav.SetValue(token->GetValue());
}

}
