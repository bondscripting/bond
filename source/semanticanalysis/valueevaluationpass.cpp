
namespace Bond
{

class ValueEvaluationPass: public SemanticAnalysisPass
{
public:
	ValueEvaluationPass(ParseErrorBuffer &errorBuffer, SymbolTable &symbolTable):
		SemanticAnalysisPass(errorBuffer, symbolTable),
		mPrevEnumerator(0),
		mItemResolved(false)
	{}

	virtual ~ValueEvaluationPass() {}

	virtual void Analyze(TranslationUnit *translationUnitList);

protected:
	virtual void Visit(EnumDeclaration *enumDeclaration);
	virtual void Visit(Enumerator *enumerator);
	virtual void Visit(NamedInitializer *namedInitializer);
	virtual void Visit(ConditionalExpression *conditionalExpression);
	virtual void Visit(BinaryExpression *binaryExpression);
	virtual void Visit(UnaryExpression *unaryExpression);
	virtual void Visit(PostfixExpression *postfixExpression);
	virtual void Visit(MemberExpression *memberExpression);
	virtual void Visit(ArraySubscriptExpression *arraySubscriptExpression);
	virtual void Visit(FunctionCallExpression *functionCallExpression);
	virtual void Visit(CastExpression *castExpression);
	virtual void Visit(SizeofExpression *sizeofExpression);
	virtual void Visit(ConstantExpression *constantExpression);
	virtual void Visit(IdentifierExpression *identifierExpression);

private:
	void Resolve(TypeAndValue &tav);

	Enumerator *mPrevEnumerator;
	bool mItemResolved;
};


void ValueEvaluationPass::Analyze(TranslationUnit *translationUnitList)
{
	do
	{
		mItemResolved = false;
		SemanticAnalysisPass::Analyze(translationUnitList);
	}
	while (mItemResolved);
}


void ValueEvaluationPass::Visit(EnumDeclaration *enumDeclaration)
{
	mPrevEnumerator = 0;
	ParseNodeTraverser::Visit(enumDeclaration);
	mPrevEnumerator = 0;
}


void ValueEvaluationPass::Visit(Enumerator *enumerator)
{
	TypeAndValue &tav = *enumerator->GetTypeAndValue();
	if (!tav.IsResolved())
	{
		ParseNodeTraverser::Visit(enumerator);

		if (enumerator->GetValue() != 0)
		{
			const TypeAndValue &valueTav = enumerator->GetValue()->GetTypeAndValue();
			if (valueTav.IsValueDefined())
			{
				Resolve(tav);
				const TypeDescriptor *resultType = tav.GetTypeDescriptor();
				tav.SetValue(CastValue(valueTav, resultType));
			}
		}
		else
		{
			if (mPrevEnumerator != 0)
			{
				const TypeAndValue &prevTav = *mPrevEnumerator->GetTypeAndValue();
				if (prevTav.IsValueDefined())
				{
					Resolve(tav);
					tav.SetIntValue(prevTav.GetIntValue() + 1);
				}
			}
			else
			{
				Resolve(tav);
				tav.SetIntValue(0);
			}
		}
	}

	mPrevEnumerator = enumerator;
}


void ValueEvaluationPass::Visit(NamedInitializer *namedInitializer)
{
	TypeAndValue &tav = *namedInitializer->GetTypeAndValue();
	if (!tav.IsResolved())
	{
		ParseNodeTraverser::Visit(namedInitializer);
		const TypeDescriptor *typeDescriptor = tav.GetTypeDescriptor();
		const Initializer *initializer = namedInitializer->GetInitializer();

		if (initializer == 0)
		{
			Resolve(tav);
		}
		else if (initializer->IsResolved() && typeDescriptor->IsResolved())
		{
			Resolve(tav);

			// TODO: Handle non-primitive types (e.g. arrays of primitive types or string literals).
			if ((typeDescriptor->GetPrimitiveType() != Token::INVALID) && typeDescriptor->IsConst())
			{
				// TODO: TypeEvaluationPass needs to assert that there is a single initializer.
				const TypeAndValue initializerTav = initializer->GetExpression()->GetTypeAndValue();
				if (initializerTav.IsValueDefined())
				{
					tav.SetValue(CastValue(initializerTav, typeDescriptor));
				}
			}
		}
	}
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
			Resolve(tav);

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
		const TypeAndValue &lhs = binaryExpression->GetLhs()->GetTypeAndValue();
		const TypeAndValue &rhs = binaryExpression->GetRhs()->GetTypeAndValue();

		if (lhs.IsResolved() && rhs.IsResolved())
		{
			Resolve(tav);

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
			Resolve(tav);

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


void ValueEvaluationPass::Visit(PostfixExpression *postfixExpression)
{
	// Value can't be evaluated at compile time. Mark as resolved when operand is resolved.
	TypeAndValue &tav = postfixExpression->GetTypeAndValue();
	if (!tav.IsResolved())
	{
		ParseNodeTraverser::Visit(postfixExpression);
		const TypeAndValue &lhs = postfixExpression->GetLhs()->GetTypeAndValue();
		if (lhs.IsResolved())
		{
			Resolve(tav);
		}
	}
}


void ValueEvaluationPass::Visit(MemberExpression *memberExpression)
{
	// Value can't be evaluated at compile time. Mark as resolved when operand is resolved.
	TypeAndValue &tav = memberExpression->GetTypeAndValue();
	if (!tav.IsResolved())
	{
		ParseNodeTraverser::Visit(memberExpression);
		const TypeAndValue &lhs = memberExpression->GetLhs()->GetTypeAndValue();
		if (lhs.IsResolved())
		{
			Resolve(tav);
		}
	}
}


void ValueEvaluationPass::Visit(ArraySubscriptExpression *arraySubscriptExpression)
{
	// TODO: A constant index into a constant array could be be evaluated at compile time.
	// Mark as resolved when array and index are resolved.
	TypeAndValue &tav = arraySubscriptExpression->GetTypeAndValue();
	if (!tav.IsResolved())
	{
		ParseNodeTraverser::Visit(arraySubscriptExpression);
		const TypeAndValue &lhs = arraySubscriptExpression->GetLhs()->GetTypeAndValue();
		const TypeAndValue &indexTav = arraySubscriptExpression->GetIndex()->GetTypeAndValue();
		if (lhs.IsResolved() && indexTav.IsResolved())
		{
			Resolve(tav);
		}
	}
}


void ValueEvaluationPass::Visit(FunctionCallExpression *functionCallExpression)
{
	// Value can't be evaluated at compile time. Mark as resolved when lhs and all arguments are resolved.
	TypeAndValue &tav = functionCallExpression->GetTypeAndValue();
	if (!tav.IsResolved())
	{
		ParseNodeTraverser::Visit(functionCallExpression);
		const TypeAndValue &lhs = functionCallExpression->GetLhs()->GetTypeAndValue();
		if (!lhs.IsResolved())
		{
			return;
		}

		const Expression *argument = functionCallExpression->GetArgumentList();
		while (argument != 0)
		{
			const TypeAndValue &argTav = argument->GetTypeAndValue();
			if (!argTav.IsResolved())
			{
				return;
			}
			argument = static_cast<const Expression *>(argument->GetNextNode());
		}
		Resolve(tav);
	}
}


void ValueEvaluationPass::Visit(CastExpression *castExpression)
{
	TypeAndValue &tav = castExpression->GetTypeAndValue();
	if (!tav.IsResolved())
	{
		ParseNodeTraverser::Visit(castExpression);
		const TypeAndValue &rhs = castExpression->GetRhs()->GetTypeAndValue();
		if (rhs.IsResolved())
		{
			Resolve(tav);
			const TypeDescriptor *resultType = tav.GetTypeDescriptor();
			tav.SetValue(CastValue(rhs, resultType));
		}
	}
}


void ValueEvaluationPass::Visit(SizeofExpression *sizeofExpression)
{
	TypeAndValue &tav = sizeofExpression->GetTypeAndValue();
	if (!tav.IsResolved())
	{
		ParseNodeTraverser::Visit(sizeofExpression);
		const TypeDescriptor *typeDescriptor = 0;

		if (sizeofExpression->GetRhs() != 0)
		{
			const TypeAndValue &rhs = sizeofExpression->GetRhs()->GetTypeAndValue();
			if (rhs.IsResolved())
			{
				typeDescriptor = rhs.GetTypeDescriptor();
			}
		}
		else
		{
			typeDescriptor = sizeofExpression->GetTypeDescriptor();
		}

		if (typeDescriptor != 0)
		{
			Resolve(tav);

			switch (typeDescriptor->GetPrimitiveType())
			{
				// TODO: Define constants somewhere.
				case Token::KEY_BOOL:
					tav.SetUIntValue(BOND_BOOL_SIZE);
					break;
				case Token::KEY_CHAR:
					tav.SetUIntValue(BOND_CHAR_SIZE);
					break;
				case Token::KEY_FLOAT:
					tav.SetUIntValue(BOND_FLOAT_SIZE);
					break;
				case Token::KEY_INT:
					tav.SetUIntValue(BOND_INT_SIZE);
					break;
				case Token::KEY_UINT:
					tav.SetUIntValue(BOND_UINT_SIZE);
					break;
				default:
					// TODO: Handle structs, arrays and pointers.
					break;
			}
		}
	}
}


void ValueEvaluationPass::Visit(ConstantExpression *constantExpression)
{
	TypeAndValue &tav = constantExpression->GetTypeAndValue();
	if (!tav.IsResolved())
	{
		const Token *token = constantExpression->GetValueToken();
		Resolve(tav);
		tav.SetValue(token->GetValue());
	}
}


void ValueEvaluationPass::Visit(IdentifierExpression *identifierExpression)
{
	TypeAndValue &tav = identifierExpression->GetTypeAndValue();
	if (!tav.IsResolved())
	{
		const TypeAndValue &definitionTav = *identifierExpression->GetDefinition()->GetTypeAndValue();
		if (definitionTav.IsResolved())
		{
			Resolve(tav);
			if (definitionTav.IsValueDefined())
			{
				tav.SetValue(definitionTav.GetValue());
			}
		}
	}
}


void ValueEvaluationPass::Resolve(TypeAndValue &tav)
{
	tav.Resolve();
	mItemResolved = true;
}

}
