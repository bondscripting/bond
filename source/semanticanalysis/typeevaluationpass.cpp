#include <stdio.h>

namespace Bond
{

class TypeEvaluationPass: public SemanticAnalysisPass
{
public:
	TypeEvaluationPass(ParseErrorBuffer &errorBuffer, SymbolTable &symbolTable):
		SemanticAnalysisPass(errorBuffer, symbolTable),
		mAddNamedInitializers(false)
	{}

	virtual ~TypeEvaluationPass() {}

protected:
	virtual void Visit(FunctionDefinition *functionDefinition);
	virtual void Visit(Parameter *parameter);
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
	void AssertBooleanType(const TypeDescriptor *descriptor, const Token *op);
	void AssertIntegerType(const TypeDescriptor *descriptor, const Token *op);
	void AssertNumericType(const TypeDescriptor *descriptor, const Token *op);
	void AssertPointerType(const TypeDescriptor *descriptor, const Token *op);
	void AssertLValueType(const TypeDescriptor *descriptor, const Token *op);
	void AssertAssignableType(const TypeDescriptor *descriptor, const Token *op);
	void AssertComparableTypes(const TypeDescriptor *typeA, const TypeDescriptor *typeB, const Token *op);

	bool mAddNamedInitializers;
};


void TypeEvaluationPass::Visit(FunctionDefinition *functionDefinition)
{
	// Top-level named initializers have already been added to the symbol table, but not ones in local scopes.
	// Top-level identifiers can be referenced out of order from their declarations, but local ones must be
	// declared before being referenced, so they must be added as the expressions are evaluated.
	mAddNamedInitializers = true;
	SemanticAnalysisPass::Visit(functionDefinition);
	mAddNamedInitializers = false;
}


void TypeEvaluationPass::Visit(Parameter *parameter)
{
	ParseNodeTraverser::Visit(parameter);
	InsertSymbol(parameter);
}


void TypeEvaluationPass::Visit(NamedInitializer *namedInitializer)
{
	ParseNodeTraverser::Visit(namedInitializer);

	if (mAddNamedInitializers)
	{
		InsertSymbol(namedInitializer);
	}
}


void TypeEvaluationPass::Visit(ConditionalExpression *conditionalExpression)
{
	ParseNodeTraverser::Visit(conditionalExpression);

	const TypeAndValue &trueTav = conditionalExpression->GetTrueExpression()->GetTypeAndValue();
	const TypeAndValue &falseTav = conditionalExpression->GetFalseExpression()->GetTypeAndValue();

	if (trueTav.IsTypeDefined() && falseTav.IsTypeDefined())
	{
		const TypeDescriptor *trueDescriptor = trueTav.GetTypeDescriptor();
		const TypeDescriptor *falseDescriptor = falseTav.GetTypeDescriptor();
		// TODO: Need to test if types are combinable.
		TypeDescriptor resultType = CombineOperandTypes(trueDescriptor, falseDescriptor);
		conditionalExpression->SetTypeDescriptor(resultType);
		TypeAndValue &tav = conditionalExpression->GetTypeAndValue();
		tav.SetTypeDescriptor(conditionalExpression->GetTypeDescriptor());
	}
}


void TypeEvaluationPass::Visit(BinaryExpression *binaryExpression)
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
			case Token::ASSIGN_AND:
			case Token::ASSIGN_OR:
			case Token::ASSIGN_XOR:
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

			case Token::OP_AND:
			case Token::OP_OR:
				AssertBooleanType(lhDescriptor, op);
				AssertBooleanType(rhDescriptor, op);
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

			case Token::OP_LEFT:
			case Token::OP_RIGHT:
				AssertIntegerType(lhDescriptor, op);
				AssertIntegerType(rhDescriptor, op);
				resultType = *lhDescriptor;
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
		TypeAndValue &tav = binaryExpression->GetTypeAndValue();
		tav.SetTypeDescriptor(binaryExpression->GetTypeDescriptor());
	}
}


void TypeEvaluationPass::Visit(UnaryExpression *unaryExpression)
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
				AssertNumericType(rhDescriptor, op);
				break;

			case Token::OP_INC:
			case Token::OP_DEC:
				AssertAssignableType(rhDescriptor, op);
				if (!rhDescriptor->IsPointerType())
				{
					AssertNumericType(rhDescriptor, op);
				}
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
				if (rhDescriptor->IsPointerType())
				{
					resultType = *rhDescriptor->GetParent();
				}
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
		TypeAndValue &tav = unaryExpression->GetTypeAndValue();
		tav.SetTypeDescriptor(unaryExpression->GetTypeDescriptor());
	}
}


void TypeEvaluationPass::Visit(PostfixExpression *postfixExpression)
{
	ParseNodeTraverser::Visit(postfixExpression);

	const TypeAndValue &lhTav = postfixExpression->GetLhs()->GetTypeAndValue();

	if (lhTav.IsTypeDefined())
	{
		const TypeDescriptor *lhDescriptor = lhTav.GetTypeDescriptor();
		const Token *op = postfixExpression->GetOperator();
		AssertAssignableType(lhDescriptor, op);
		if (!lhDescriptor->IsPointerType())
		{
			AssertNumericType(lhDescriptor, op);
		}
		TypeAndValue &tav = postfixExpression->GetTypeAndValue();
		tav.SetTypeDescriptor(lhDescriptor);
	}
}


void TypeEvaluationPass::Visit(MemberExpression *memberExpression)
{
	ParseNodeTraverser::Visit(memberExpression);

	const TypeAndValue &lhTav = memberExpression->GetLhs()->GetTypeAndValue();

	if (lhTav.IsTypeDefined())
	{
		const TypeDescriptor *lhDescriptor = lhTav.GetTypeDescriptor();
		const TypeDescriptor *structDescriptor = lhDescriptor;
		const Token *op = memberExpression->GetOperator();
		const Token *memberName = memberExpression->GetMemberName();

		if (op->GetTokenType() == Token::OP_ARROW)
		{
			AssertPointerType(lhDescriptor, op);
			if (lhDescriptor->IsPointerType())
			{
				structDescriptor = lhDescriptor->GetParent();
			}
		}

		const TypeSpecifier *structSpecifier = structDescriptor->GetTypeSpecifier();
		if ((structSpecifier == 0) ||
		    (structSpecifier->GetDefinition() == 0) ||
		    (structSpecifier->GetDefinition()->GetSymbolType() != Symbol::TYPE_STRUCT))
		{
			mErrorBuffer.PushError(ParseError::NON_STRUCT_MEMBER_REQUEST, memberName, structDescriptor);
		}
		else
		{
			const Symbol *structDeclaration = CastNode<StructDeclaration>(structSpecifier->GetDefinition());
			const Symbol *member = structDeclaration->FindSymbol(memberName);
			if (member == 0)
			{
				mErrorBuffer.PushError(ParseError::INVALID_MEMBER_REQUEST, memberName, structDescriptor);
			}
			else
			{
				TypeAndValue &tav = memberExpression->GetTypeAndValue();
				tav.SetTypeDescriptor(member->GetTypeAndValue()->GetTypeDescriptor());
			}
		}
	}
}


void TypeEvaluationPass::Visit(ArraySubscriptExpression *arraySubscriptExpression)
{
	ParseNodeTraverser::Visit(arraySubscriptExpression);

	const Token *op = arraySubscriptExpression->GetOperator();
	const TypeAndValue &indexTav = arraySubscriptExpression->GetIndex()->GetTypeAndValue();

	if (indexTav.IsTypeDefined())
	{
		const TypeDescriptor *indexDescriptor = indexTav.GetTypeDescriptor();
		if (!indexDescriptor->IsIntegerType())
		{
			mErrorBuffer.PushError(ParseError::INVALID_TYPE_FOR_INDEX_OPERATOR, op, indexDescriptor);
		}
	}

	const TypeAndValue &lhTav = arraySubscriptExpression->GetLhs()->GetTypeAndValue();

	if (lhTav.IsTypeDefined())
	{
		const TypeDescriptor *lhDescriptor = lhTav.GetTypeDescriptor();

		AssertPointerType(lhDescriptor, op);
		if (lhDescriptor->IsPointerType())
		{
			TypeAndValue &tav = arraySubscriptExpression->GetTypeAndValue();
			tav.SetTypeDescriptor(lhDescriptor->GetParent());
		}
	}
}


void TypeEvaluationPass::Visit(FunctionCallExpression *functionCallExpression)
{
	ParseNodeTraverser::Visit(functionCallExpression);

	const TypeAndValue &lhTav = functionCallExpression->GetLhs()->GetTypeAndValue();

	if (lhTav.IsTypeDefined())
	{
		const TypeDescriptor *lhDescriptor = lhTav.GetTypeDescriptor();
		const TypeSpecifier *lhSpecifier = lhDescriptor->GetTypeSpecifier();

		if ((lhSpecifier == 0) ||
		    (lhSpecifier->GetDefinition() == 0) ||
		    (lhSpecifier->GetDefinition()->GetSymbolType() != Symbol::TYPE_FUNCTION))
		{
			mErrorBuffer.PushError(ParseError::EXPRESSION_IS_NOT_CALLABLE, functionCallExpression->GetContextToken());
		}
		else
		{
			// TODO: Validate the number of arguments.
			const FunctionDefinition *function = CastNode<FunctionDefinition>(lhSpecifier->GetDefinition());
			const FunctionPrototype *prototype = function->GetPrototype();
			const TypeDescriptor *returnType = prototype->GetReturnType();
			TypeAndValue &tav = functionCallExpression->GetTypeAndValue();
			tav.SetTypeDescriptor(returnType);
		}
	}
}


void TypeEvaluationPass::Visit(CastExpression *castExpression)
{
	ParseNodeTraverser::Visit(castExpression);

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
		else if (rhDescriptor->IsPointerType())
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

		TypeAndValue &tav = castExpression->GetTypeAndValue();
		tav.SetTypeDescriptor(lhDescriptor);
	}
}


void TypeEvaluationPass::Visit(SizeofExpression *sizeofExpression)
{
	ParseNodeTraverser::Visit(sizeofExpression);

	TypeAndValue &tav = sizeofExpression->GetTypeAndValue();
	tav.SetTypeDescriptor(&CONST_UINT_TYPE_DESCRIPTOR);
}


void TypeEvaluationPass::Visit(ConstantExpression *constantExpression)
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

	TypeAndValue &tav = constantExpression->GetTypeAndValue();
	tav.SetTypeDescriptor(descriptor);
}


void TypeEvaluationPass::Visit(IdentifierExpression *identifierExpression)
{
	const QualifiedIdentifier *identifier = identifierExpression->GetIdentifier();
	const Symbol *symbol = GetSymbol(identifier);

	if (symbol == 0)
	{
		mErrorBuffer.PushError(ParseError::SYMBOL_IS_NOT_DEFINED, identifier->GetContextToken(), identifier);
	}
	else
	{
		const TypeAndValue *symbolTav = symbol->GetTypeAndValue();

		if (symbolTav == 0)
		{
			mErrorBuffer.PushError(ParseError::INVALID_SYMBOL_IN_EXPRESSION, identifier->GetContextToken(), identifier);
		}
		else if (symbolTav->IsTypeDefined())
		{
			TypeAndValue &tav = identifierExpression->GetTypeAndValue();
			tav.SetTypeDescriptor(symbolTav->GetTypeDescriptor());
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
