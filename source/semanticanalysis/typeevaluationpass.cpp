namespace Bond
{

class TypeEvaluationPass: public SemanticAnalysisPass
{
public:
	~TypeEvaluationPass() {}

	virtual void Analyze(TranslationUnit *translationUnitList);

protected:
	TypeEvaluationPass(ParseErrorBuffer &errorBuffer, Allocator &allocator, SymbolTable &symbolTable):
		SemanticAnalysisPass(errorBuffer, allocator, symbolTable)
	{}

	virtual void Visit(ConditionalExpression *conditionalExpression);
	virtual void Visit(BinaryExpression *binaryExpression);
	virtual void Visit(UnaryExpression *unaryExpression);
	virtual void Visit(PostfixExpression *postfixExpression);
	//virtual void Visit(MemberExpression *memberExpression);
	//virtual void Visit(ArraySubscriptExpression *arraySubscriptExpression);
	//virtual void Visit(FunctionCallExpression *functionCallExpression);
	virtual void Visit(CastExpression *castExpression);
	virtual void Visit(SizeofExpression *sizeofExpression);
	virtual void Visit(ConstantExpression *constantExpression);
	virtual void Visit(IdentifierExpression *identifierExpression);

private:
	void AssertBooleanType(const TypeDescriptor *descriptor, const Token *op);
	void AssertIntegerType(const TypeDescriptor *descriptor, const Token *op);
	void AssertNumericType(const TypeDescriptor *descriptor, const Token *op);
	void AssertAssignableType(const TypeDescriptor *descriptor, const Token *op);
	void AssertComparableTypes(const TypeDescriptor *typeA, const TypeDescriptor *typeB, const Token *op);
	const TypeDescriptor *CombineOperandTypes(const TypeDescriptor *typeA, const TypeDescriptor *typeB) const;

	bool mMadeChanges;
};


class TopLevelTypeEvaluationPass: public TypeEvaluationPass
{
public:
	TopLevelTypeEvaluationPass(ParseErrorBuffer &errorBuffer, Allocator &allocator, SymbolTable &symbolTable):
		TypeEvaluationPass(errorBuffer, allocator, symbolTable)
	{}

	~TopLevelTypeEvaluationPass() {}

protected:
	// This pass doesn't descend into structs and functions.
	virtual void Visit(StructDeclaration *structDeclaration) {}
	virtual void Visit(FunctionDefinition *functionDefinition) {}
};


void TypeEvaluationPass::Analyze(TranslationUnit *translationUnitList)
{
	do
	{
		mMadeChanges = false;
		SemanticAnalysisPass::Analyze(translationUnitList);
	}
	while (mMadeChanges);
}


void TypeEvaluationPass::Visit(ConditionalExpression *conditionalExpression)
{
	ParseNodeTraverser::Visit(conditionalExpression);
	TypeAndValue &tav = conditionalExpression->GetTypeAndValue();

	if (!tav.IsTypeDefined())
	{
		// TODO: WRONG
		tav.SetTypeDescriptor(&CONST_BOOL_TYPE_DESCRIPTOR);
		mMadeChanges = true;
	}
}


void TypeEvaluationPass::Visit(BinaryExpression *binaryExpression)
{
	TypeAndValue &tav = binaryExpression->GetTypeAndValue();

	if (!tav.IsTypeDefined())
	{
		ParseNodeTraverser::Visit(binaryExpression);
		const TypeAndValue &lhs = binaryExpression->GetLhs()->GetTypeAndValue();
		const TypeAndValue &rhs = binaryExpression->GetRhs()->GetTypeAndValue();

		if (lhs.IsTypeDefined() && rhs.IsTypeDefined())
		{
			const TypeDescriptor *lhd = lhs.GetTypeDescriptor();
			const TypeDescriptor *rhd = rhs.GetTypeDescriptor();
			const TypeDescriptor *result = lhd;
			const Token *op = binaryExpression->GetOperator();

			switch (op->GetTokenType())
			{
				case Token::COMMA:
					result = lhd;
					break;

				case Token::ASSIGN:
					AssertAssignableType(lhd, op);
					// TODO: Assert that rhs can be assigned to lhs.
					result = lhd;
					break;

				case Token::ASSIGN_LEFT:
				case Token::ASSIGN_RIGHT:
				case Token::ASSIGN_MOD:
				case Token::OP_LEFT:
				case Token::OP_RIGHT:
					AssertIntegerType(lhd, op);
					AssertIntegerType(rhd, op);
					AssertAssignableType(lhd, op);
					result = lhd;
					break;

				case Token::ASSIGN_PLUS:
				case Token::ASSIGN_MINUS:
					AssertAssignableType(lhd, op);
					if (lhd->IsPointerType())
					{
						AssertIntegerType(rhd, op);
						// TODO
					}
					else
					{
						AssertNumericType(lhd, op);
						AssertNumericType(rhd, op);
						result = lhd;
					}
					break;

				case Token::ASSIGN_MULT:
				case Token::ASSIGN_DIV:
					AssertNumericType(lhd, op);
					AssertNumericType(rhd, op);
					AssertAssignableType(lhd, op);
					result = lhd;
					break;

				case Token::ASSIGN_AND:
				case Token::ASSIGN_OR:
				case Token::ASSIGN_XOR:
				case Token::OP_AND:
				case Token::OP_OR:
					AssertBooleanType(lhd, op);
					AssertBooleanType(rhd, op);
					AssertAssignableType(lhd, op);
					result = lhd;
					break;

				case Token::OP_BIT_AND:
				case Token::OP_BIT_OR:
				case Token::OP_BIT_XOR:
				case Token::OP_MOD:
					AssertIntegerType(lhd, op);
					AssertIntegerType(rhd, op);
					result = CombineOperandTypes(lhd, rhd);
					break;

				case Token::OP_EQUAL:
				case Token::OP_NOT_EQUAL:
				case Token::OP_LT:
				case Token::OP_LTE:
				case Token::OP_GT:
				case Token::OP_GTE:
					AssertComparableTypes(lhd, rhd, op);
					result = &CONST_BOOL_TYPE_DESCRIPTOR;
					break;

				case Token::OP_PLUS:
				case Token::OP_MINUS:
					if (lhd->IsPointerType())
					{
						AssertIntegerType(rhd, op);
						// TODO
					}
					else if (rhd->IsPointerType())
					{
						AssertIntegerType(lhd, op);
						// TODO
					}
					else
					{
						AssertNumericType(lhd, op);
						AssertNumericType(rhd, op);
						result = CombineOperandTypes(lhd, rhd);
					}
					break;

				case Token::OP_MULT:
				case Token::OP_DIV:
					AssertNumericType(lhd, op);
					AssertNumericType(rhd, op);
					result = CombineOperandTypes(lhd, rhd);
					break;

				default:
					break;
			}
		}
	}
}


void TypeEvaluationPass::Visit(UnaryExpression *unaryExpression)
{
	TypeAndValue &tav = unaryExpression->GetTypeAndValue();

	if (!tav.IsTypeDefined())
	{
		ParseNodeTraverser::Visit(unaryExpression);
		const TypeAndValue &rhs = unaryExpression->GetRhs()->GetTypeAndValue();

		if (rhs.IsTypeDefined())
		{
			const TypeDescriptor *rhd = rhs.GetTypeDescriptor();
			const Token *op = unaryExpression->GetOperator();

			switch (op->GetTokenType())
			{
				case Token::OP_PLUS:
				case Token::OP_MINUS:
				case Token::OP_MULT:
				case Token::OP_INC:
				case Token::OP_DEC:
					AssertNumericType(rhd, op);
					break;

				case Token::OP_NOT:
					AssertBooleanType(rhd, op);
					break;

				case Token::OP_BIT_AND:
				case Token::OP_BIT_NOT:
					AssertIntegerType(rhd, op);
					break;

				default:
					break;
			}

			tav.SetTypeDescriptor(rhd);
			tav.Resolve();
			mMadeChanges = true;
		}
	}
}


void TypeEvaluationPass::Visit(PostfixExpression *postfixExpression)
{
	TypeAndValue &tav = postfixExpression->GetTypeAndValue();

	if (!tav.IsTypeDefined())
	{
		ParseNodeTraverser::Visit(postfixExpression);
		const TypeAndValue &lhs = postfixExpression->GetLhs()->GetTypeAndValue();

		if (lhs.IsTypeDefined())
		{
			const TypeDescriptor *lhd = lhs.GetTypeDescriptor();
			const Token *op = postfixExpression->GetOperator();
			AssertNumericType(lhd, op);
			tav.SetTypeDescriptor(lhd);
			tav.Resolve();
			mMadeChanges = true;
		}
	}
}


void TypeEvaluationPass::Visit(CastExpression *castExpression)
{
	ParseNodeTraverser::Visit(castExpression);
	TypeAndValue &tav = castExpression->GetTypeAndValue();

	if (!tav.IsTypeDefined())
	{
		const TypeAndValue &rhs = castExpression->GetRhs()->GetTypeAndValue();

		if (rhs.IsTypeDefined())
		{
			const TypeDescriptor *rhd = rhs.GetTypeDescriptor();
			const TypeDescriptor *lhd = castExpression->GetTypeDescriptor();
			bool convertible = false;

			if (rhd->GetVariant() == TypeDescriptor::VARIANT_VALUE)
			{
				convertible = rhd->IsNumericType() && lhd->IsNumericType();
			}
			else
			{
				convertible = lhd->GetVariant() == TypeDescriptor::VARIANT_POINTER;
			}

			if (!convertible)
			{
				mErrorBuffer.PushError(ParseError::INVALID_TYPE_CONVERSION, lhd->GetContextToken(), rhd, lhd);
			}

			tav.SetTypeDescriptor(lhd);
			mMadeChanges = true;
		}
	}
}


void TypeEvaluationPass::Visit(SizeofExpression *sizeofExpression)
{
	ParseNodeTraverser::Visit(sizeofExpression);
	TypeAndValue &tav = sizeofExpression->GetTypeAndValue();

	if (!tav.IsTypeDefined())
	{
		tav.SetTypeDescriptor(&CONST_UINT_TYPE_DESCRIPTOR);
		mMadeChanges = true;
	}
}


void TypeEvaluationPass::Visit(ConstantExpression *constantExpression)
{
	TypeAndValue &tav = constantExpression->GetTypeAndValue();

	if (!tav.IsTypeDefined())
	{
		const Token *token = constantExpression->GetValueToken();
		const TypeDescriptor *descriptor = &CONST_INT_TYPE_DESCRIPTOR;

		switch (token->GetTokenType())
		{
			case Token::CONST_BOOL:
				descriptor = &CONST_BOOL_TYPE_DESCRIPTOR;
				break;
			case Token::CONST_CHAR:
				descriptor = &CONST_CHAR_TYPE_DESCRIPTOR;
				break;
			case Token::CONST_INT:
				descriptor = &CONST_INT_TYPE_DESCRIPTOR;
				break;
			case Token::CONST_UINT:
				descriptor = &CONST_UINT_TYPE_DESCRIPTOR;
				break;
			case Token::CONST_FLOAT:
				descriptor = &CONST_FLOAT_TYPE_DESCRIPTOR;
				break;
			case Token::CONST_STRING:
				descriptor = &CONST_STRING_TYPE_DESCRIPTOR;
				break;
			default:
				// Ignore the default case because the parser isn't supposed to allow it to happen.
				break;
		}

		tav.SetTypeDescriptor(descriptor);
		mMadeChanges = true;
	}
}


void TypeEvaluationPass::Visit(IdentifierExpression *identifierExpression)
{
	TypeAndValue &tav = identifierExpression->GetTypeAndValue();

	if (!tav.IsTypeDefined())
	{
	}
}


void TypeEvaluationPass::AssertBooleanType(const TypeDescriptor *descriptor, const Token *op)
{
	if (!descriptor->IsBooleanType())
	{
		mErrorBuffer.PushError(ParseError::INVALID_TYPE_FOR_OPERATOR, op, descriptor);
	}
}


void TypeEvaluationPass::AssertIntegerType(const TypeDescriptor *descriptor, const Token *op)
{
	if (!descriptor->IsIntegerType())
	{
		mErrorBuffer.PushError(ParseError::INVALID_TYPE_FOR_OPERATOR, op, descriptor);
	}
}


void TypeEvaluationPass::AssertNumericType(const TypeDescriptor *descriptor, const Token *op)
{
	if (!descriptor->IsNumericType())
	{
		mErrorBuffer.PushError(ParseError::INVALID_TYPE_FOR_OPERATOR, op, descriptor);
	}
}


void TypeEvaluationPass::AssertAssignableType(const TypeDescriptor *descriptor, const Token *op)
{
	if (!descriptor->IsAssignableType())
	{
		mErrorBuffer.PushError(ParseError::UNASSIGNABLE_TYPE, op, descriptor);
	}
}


void TypeEvaluationPass::AssertComparableTypes(const TypeDescriptor *typeA, const TypeDescriptor *typeB, const Token *op)
{
	if (!AreComparableTypes(typeA, typeB))
	{
		mErrorBuffer.PushError(ParseError::INVALID_COMPARISON, op, typeA, typeB);
	}
}


const TypeDescriptor *TypeEvaluationPass::CombineOperandTypes(const TypeDescriptor *typeA, const TypeDescriptor *typeB) const
{
	const Token::TokenType a = typeA->GetPrimitiveType();
	const Token::TokenType b = typeB->GetPrimitiveType();
	const TypeDescriptor *result = &CONST_INT_TYPE_DESCRIPTOR;

	if (typeA->IsPointerType())
	{
		// TODO
	}

	else if (typeB->IsPointerType())
	{
		// TODO
	}

	else if ((a == Token::KEY_FLOAT) || (b == Token::KEY_FLOAT))
	{
		result = &CONST_FLOAT_TYPE_DESCRIPTOR;
	}
	else if ((a == Token::KEY_INT) || (b == Token::KEY_INT))
	{
		result = &CONST_INT_TYPE_DESCRIPTOR;
	}
	else if ((a == Token::KEY_UINT) || (b == Token::KEY_UINT))
	{
		result = &CONST_UINT_TYPE_DESCRIPTOR;
	}
	else if ((a == Token::KEY_CHAR) || (b == Token::KEY_CHAR))
	{
		result = &CONST_CHAR_TYPE_DESCRIPTOR;
	}
	else if ((a == Token::KEY_BOOL) || (b == Token::KEY_BOOL))
	{
		result = &CONST_BOOL_TYPE_DESCRIPTOR;
	}

	return result;
}

}
