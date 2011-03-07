namespace Bond
{

class TypeEvaluationPass: public SemanticAnalysisPass
{
public:
	virtual ~TypeEvaluationPass() {}

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
	virtual void Visit(ArraySubscriptExpression *arraySubscriptExpression);
	virtual void Visit(FunctionCallExpression *functionCallExpression);
	virtual void Visit(CastExpression *castExpression);
	virtual void Visit(SizeofExpression *sizeofExpression);
	virtual void Visit(ConstantExpression *constantExpression);
	virtual void Visit(IdentifierExpression *identifierExpression);

private:
	void AssertBooleanType(const TypeDescriptor *descriptor, const Token *op);
	void AssertIntegerType(const TypeDescriptor *descriptor, const Token *op);
	void AssertNumericType(const TypeDescriptor *descriptor, const Token *op);
	void AssertPointerType(const TypeDescriptor *descriptor, const Token *op);
	void AssertLValueType(const TypeDescriptor *descriptor, const Token *op);
	void AssertAssignableType(const TypeDescriptor *descriptor, const Token *op);
	void AssertComparableTypes(const TypeDescriptor *typeA, const TypeDescriptor *typeB, const Token *op);

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
	TypeAndValue &tav = conditionalExpression->GetTypeAndValue();

	if (!tav.IsTypeDefined())
	{
		ParseNodeTraverser::Visit(conditionalExpression);
		const TypeAndValue &trueTav = conditionalExpression->GetTrueExpression()->GetTypeAndValue();
		const TypeAndValue &falseTav = conditionalExpression->GetFalseExpression()->GetTypeAndValue();

		if (trueTav.IsTypeDefined() && falseTav.IsTypeDefined())
		{
			const TypeDescriptor *trueDescriptor = trueTav.GetTypeDescriptor();
			const TypeDescriptor *falseDescriptor = falseTav.GetTypeDescriptor();
			TypeDescriptor resultType = CombineOperandTypes(trueDescriptor, falseDescriptor);
			conditionalExpression->SetTypeDescriptor(resultType);
			tav.SetTypeDescriptor(conditionalExpression->GetTypeDescriptor());
			tav.Resolve();
			mMadeChanges = true;
		}
	}
}


void TypeEvaluationPass::Visit(BinaryExpression *binaryExpression)
{
	TypeAndValue &tav = binaryExpression->GetTypeAndValue();

	if (!tav.IsTypeDefined())
	{
		ParseNodeTraverser::Visit(binaryExpression);
		const TypeAndValue &lhTav = binaryExpression->GetLhs()->GetTypeAndValue();
		const TypeAndValue &rhTav = binaryExpression->GetRhs()->GetTypeAndValue();

		if (lhTav.IsTypeDefined() && rhTav.IsTypeDefined())
		{
			const TypeDescriptor *lhDescriptor = lhTav.GetTypeDescriptor();
			const TypeDescriptor *rhDescriptor = rhTav.GetTypeDescriptor();
			TypeDescriptor resultType = *lhDescriptor;
			const Token *op = binaryExpression->GetOperator();

			switch (op->GetTokenType())
			{
				case Token::COMMA:
					resultType = *rhDescriptor;
					break;

				case Token::ASSIGN:
					AssertAssignableType(lhDescriptor, op);
					// TODO: Assert that rhTav type can be assigned to lhTav.
					resultType = *lhDescriptor;
					break;

				case Token::ASSIGN_LEFT:
				case Token::ASSIGN_RIGHT:
				case Token::ASSIGN_MOD:
				case Token::OP_LEFT:
				case Token::OP_RIGHT:
					AssertIntegerType(lhDescriptor, op);
					AssertIntegerType(rhDescriptor, op);
					AssertAssignableType(lhDescriptor, op);
					resultType = *lhDescriptor;
					break;

				case Token::ASSIGN_PLUS:
				case Token::ASSIGN_MINUS:
					if (lhDescriptor->IsPointerType())
					{
						AssertIntegerType(rhDescriptor, op);
					}
					else if (rhDescriptor->IsPointerType())
					{
						AssertIntegerType(lhDescriptor, op);
					}
					else
					{
						AssertNumericType(lhDescriptor, op);
						AssertNumericType(rhDescriptor, op);
					}

					AssertAssignableType(lhDescriptor, op);
					resultType = *lhDescriptor;
					break;

				case Token::ASSIGN_MULT:
				case Token::ASSIGN_DIV:
					AssertNumericType(lhDescriptor, op);
					AssertNumericType(rhDescriptor, op);
					AssertAssignableType(lhDescriptor, op);
					resultType = *lhDescriptor;
					break;

				case Token::ASSIGN_AND:
				case Token::ASSIGN_OR:
				case Token::ASSIGN_XOR:
				case Token::OP_AND:
				case Token::OP_OR:
					AssertBooleanType(lhDescriptor, op);
					AssertBooleanType(rhDescriptor, op);
					AssertAssignableType(lhDescriptor, op);
					resultType = *lhDescriptor;
					break;

				case Token::OP_BIT_AND:
				case Token::OP_BIT_OR:
				case Token::OP_BIT_XOR:
				case Token::OP_MOD:
					AssertIntegerType(lhDescriptor, op);
					AssertIntegerType(rhDescriptor, op);
					resultType = CombineOperandTypes(lhDescriptor, rhDescriptor);
					break;

				case Token::OP_EQUAL:
				case Token::OP_NOT_EQUAL:
				case Token::OP_LT:
				case Token::OP_LTE:
				case Token::OP_GT:
				case Token::OP_GTE:
					AssertComparableTypes(lhDescriptor, rhDescriptor, op);
					resultType = CONST_BOOL_TYPE_DESCRIPTOR;
					break;

				case Token::OP_PLUS:
				case Token::OP_MINUS:
					if (lhDescriptor->IsPointerType())
					{
						AssertIntegerType(rhDescriptor, op);
					}
					else if (rhDescriptor->IsPointerType())
					{
						AssertIntegerType(lhDescriptor, op);
					}
					else
					{
						AssertNumericType(lhDescriptor, op);
						AssertNumericType(rhDescriptor, op);
					}
					resultType = CombineOperandTypes(lhDescriptor, rhDescriptor);
					break;

				case Token::OP_MULT:
				case Token::OP_DIV:
					AssertNumericType(lhDescriptor, op);
					AssertNumericType(rhDescriptor, op);
					resultType = CombineOperandTypes(lhDescriptor, rhDescriptor);
					break;

				default:
					break;
			}

			resultType.SetRValue();
			binaryExpression->SetTypeDescriptor(resultType);
			tav.SetTypeDescriptor(binaryExpression->GetTypeDescriptor());
			tav.Resolve();
			mMadeChanges = true;
		}
	}
}


void TypeEvaluationPass::Visit(UnaryExpression *unaryExpression)
{
	TypeAndValue &tav = unaryExpression->GetTypeAndValue();

	if (!tav.IsTypeDefined())
	{
		ParseNodeTraverser::Visit(unaryExpression);
		const TypeAndValue &rhTav = unaryExpression->GetRhs()->GetTypeAndValue();

		if (rhTav.IsTypeDefined())
		{
			bool isRValue = true;
			const TypeDescriptor *rhDescriptor = rhTav.GetTypeDescriptor();
			TypeDescriptor resultType = *rhDescriptor;
			resultType.SetRValue();
			const Token *op = unaryExpression->GetOperator();

			switch (op->GetTokenType())
			{
				case Token::OP_PLUS:
				case Token::OP_MINUS:
				case Token::OP_INC:
				case Token::OP_DEC:
					AssertNumericType(rhDescriptor, op);
					break;

				case Token::OP_NOT:
					AssertBooleanType(rhDescriptor, op);
					break;

				case Token::OP_BIT_AND:
					AssertLValueType(rhDescriptor, op);
					resultType = TypeDescriptor(rhDescriptor, true);
					break;

				case Token::OP_BIT_NOT:
					AssertIntegerType(rhDescriptor, op);
					break;

				case Token::OP_MULT:
					AssertPointerType(rhDescriptor, op);
					resultType = *rhDescriptor->GetParent();
					isRValue = false;
					break;

				default:
					break;
			}

			if (isRValue)
			{
				resultType.SetRValue();
			}

			unaryExpression->SetTypeDescriptor(resultType);
			tav.SetTypeDescriptor(unaryExpression->GetTypeDescriptor());
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
		const TypeAndValue &lhTav = postfixExpression->GetLhs()->GetTypeAndValue();

		if (lhTav.IsTypeDefined())
		{
			const TypeDescriptor *lhDescriptor = lhTav.GetTypeDescriptor();
			const Token *op = postfixExpression->GetOperator();
			AssertNumericType(lhDescriptor, op);
			tav.SetTypeDescriptor(lhDescriptor);
			tav.Resolve();
			mMadeChanges = true;
		}
	}
}


void TypeEvaluationPass::Visit(ArraySubscriptExpression *arraySubscriptExpression)
{
	TypeAndValue &tav = arraySubscriptExpression->GetTypeAndValue();

	if (!tav.IsTypeDefined())
	{
		ParseNodeTraverser::Visit(arraySubscriptExpression);
		const TypeAndValue &lhTav = arraySubscriptExpression->GetLhs()->GetTypeAndValue();
		const TypeAndValue &indexTav = arraySubscriptExpression->GetIndex()->GetTypeAndValue();

		if (lhTav.IsTypeDefined() && indexTav.IsTypeDefined())
		{
			const TypeDescriptor *lhDescriptor = lhTav.GetTypeDescriptor();
			const TypeDescriptor *indexDescriptor = indexTav.GetTypeDescriptor();
			if (!indexDescriptor->IsIntegerType())
			{
				const Token *op = arraySubscriptExpression->GetOperator();
				mErrorBuffer.PushError(ParseError::INVALID_TYPE_FOR_INDEX_OPERATOR, op, indexDescriptor);
			}
			tav.SetTypeDescriptor(lhDescriptor->GetParent());
			tav.Resolve();
			mMadeChanges = true;
		}
	}
}


void TypeEvaluationPass::Visit(FunctionCallExpression *functionCallExpression)
{
	ParseNodeTraverser::Visit(functionCallExpression);
	TypeAndValue &tav = functionCallExpression->GetTypeAndValue();

	if (!tav.IsTypeDefined())
	{
		// TODO: Holy crap. How does an expression evaluate to a symbol?
		//tav.SetTypeDescriptor(lhDescriptor);
		mMadeChanges = true;
	}
}


void TypeEvaluationPass::Visit(CastExpression *castExpression)
{
	ParseNodeTraverser::Visit(castExpression);
	TypeAndValue &tav = castExpression->GetTypeAndValue();

	if (!tav.IsTypeDefined())
	{
		const TypeAndValue &rhTav = castExpression->GetRhs()->GetTypeAndValue();

		if (rhTav.IsTypeDefined())
		{
			const TypeDescriptor *rhDescriptor = rhTav.GetTypeDescriptor();
			TypeDescriptor *lhDescriptor = castExpression->GetTypeDescriptor();
			bool convertible = false;

			if (rhDescriptor->GetVariant() == TypeDescriptor::VARIANT_VALUE)
			{
				convertible = rhDescriptor->IsNumericType() && lhDescriptor->IsNumericType();
			}
			else
			{
				convertible = lhDescriptor->GetVariant() == TypeDescriptor::VARIANT_POINTER;
			}

			if (!convertible)
			{
				mErrorBuffer.PushError(
					ParseError::INVALID_TYPE_CONVERSION,
					lhDescriptor->GetContextToken(),
					rhDescriptor,
					lhDescriptor);
			}

			if (rhDescriptor->IsLValue())
			{
				lhDescriptor->SetLValue();
			}

			tav.SetTypeDescriptor(lhDescriptor);
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
		const Symbol *symbol = GetSymbol(identifierExpression->GetIdentifier());
		if (symbol != 0)
		{
			const TypeAndValue &symbolTav = symbol->GetTypeAndValue();
			if (symbolTav.IsTypeDefined())
			{
				tav.SetTypeDescriptor(symbolTav.GetTypeDescriptor());
				mMadeChanges = true;
			}
		}
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


void TypeEvaluationPass::AssertPointerType(const TypeDescriptor *descriptor, const Token *op)
{
	if (!descriptor->IsPointerType())
	{
		mErrorBuffer.PushError(ParseError::INVALID_TYPE_FOR_POINTER_OPERATOR, op, descriptor);
	}
}


void TypeEvaluationPass::AssertNumericType(const TypeDescriptor *descriptor, const Token *op)
{
	if (!descriptor->IsNumericType())
	{
		mErrorBuffer.PushError(ParseError::INVALID_TYPE_FOR_OPERATOR, op, descriptor);
	}
}


void TypeEvaluationPass::AssertLValueType(const TypeDescriptor *descriptor, const Token *op)
{
	if (!descriptor->IsLValue())
	{
		mErrorBuffer.PushError(ParseError::NON_LVALUE_TYPE, op, descriptor);
	}
}


void TypeEvaluationPass::AssertAssignableType(const TypeDescriptor *descriptor, const Token *op)
{
	if (!descriptor->IsAssignable())
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

}
