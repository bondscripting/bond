namespace Bond
{

class TypeEvaluationPass: public SemanticAnalysisPass
{
public:
	TypeEvaluationPass(ParseErrorBuffer &errorBuffer, SymbolTable &symbolTable):
		SemanticAnalysisPass(errorBuffer, symbolTable)
	{}

	virtual ~TypeEvaluationPass() {}

	virtual void Analyze(TranslationUnit *translationUnitList);

protected:
	virtual void Visit(Enumerator *enumerator);
	virtual void Visit(FunctionDefinition *functionDefinition);
	virtual void Visit(Parameter *parameter);
	virtual void Visit(TypeDescriptor *typeDescriptor);
	virtual void Visit(NamedInitializer *namedInitializer);
	virtual void Visit(IfStatement *ifStatement);
	virtual void Visit(SwitchStatement *switchStatement);
	virtual void Visit(SwitchLabel *switchLabel);
	virtual void Visit(WhileStatement *whileStatement);
	virtual void Visit(ForStatement *forStatement);
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
	bool IsBooleanExpression(const Expression *expression) const;
	bool IsIntegerExpression(const Expression *expression) const;;
	void AssertNonConstExpression(const Token *op);
	void AssertBooleanOperand(const TypeDescriptor *descriptor, const Token *op);
	void AssertIntegerOperand(const TypeDescriptor *descriptor, const Token *op);
	void AssertNumericOperand(const TypeDescriptor *descriptor, const Token *op);
	void AssertPointerOperand(const TypeDescriptor *descriptor, const Token *op);
	void AssertLValueType(const TypeDescriptor *descriptor, const Token *op);
	void AssertAssignableType(const TypeDescriptor *descriptor, const Token *op);
	void AssertComparableTypes(const TypeDescriptor *typeA, const TypeDescriptor *typeB, const Token *op);

	BoolStack mAddNamedInitializers;
	BoolStack mEnforceConstExpressions;
};


void TypeEvaluationPass::Analyze(TranslationUnit *translationUnitList)
{
	BoolStack::Element initalizerElement(mAddNamedInitializers, false);
	BoolStack::Element constExpressionElement(mEnforceConstExpressions, false);
	SemanticAnalysisPass::Analyze(translationUnitList);
}


void TypeEvaluationPass::Visit(Enumerator *enumerator)
{
	BoolStack::Element constExpressionElement(mEnforceConstExpressions, true);
	ParseNodeTraverser::Visit(enumerator);
	const Expression *value = enumerator->GetValue();
	if ((value != 0) && !IsIntegerExpression(value))
	{
		mErrorBuffer.PushError(
			ParseError::ENUMERATOR_VALUE_IS_NOT_CONST_INTEGER,
			value->GetContextToken(),
			enumerator->GetName());
	}
}


void TypeEvaluationPass::Visit(FunctionDefinition *functionDefinition)
{
	// Top-level named initializers have already been added to the symbol table, but not ones in local scopes.
	// Top-level identifiers can be referenced out of order from their declarations, but local ones must be
	// declared before being referenced, so they must be added as the expressions are evaluated.
	BoolStack::Element initalizerElement(mAddNamedInitializers, true);
	SemanticAnalysisPass::Visit(functionDefinition);
}


void TypeEvaluationPass::Visit(Parameter *parameter)
{
	ParseNodeTraverser::Visit(parameter);
	InsertSymbol(parameter);
}


void TypeEvaluationPass::Visit(TypeDescriptor *typeDescriptor)
{
	BoolStack::Element constExpressionElement(mEnforceConstExpressions, true);
	ParseNodeTraverser::Visit(typeDescriptor);
	const Expression *expression = typeDescriptor->GetLengthExpression();
	if ((expression != 0) && !IsIntegerExpression(expression))
	{
		mErrorBuffer.PushError(
			ParseError::ARRAY_SIZE_IS_NOT_CONST_INTEGER,
			expression->GetContextToken());
	}
}


void TypeEvaluationPass::Visit(NamedInitializer *namedInitializer)
{
	ParseNodeTraverser::Visit(namedInitializer);

	if (mAddNamedInitializers.GetTop())
	{
		InsertSymbol(namedInitializer);
	}

	TypeAndValue &tav = *namedInitializer->GetTypeAndValue();
	if (tav.IsTypeDefined())
	{
		const TypeDescriptor *typeDescriptor = tav.GetTypeDescriptor();
		if (typeDescriptor->IsSingleAssignable() && (namedInitializer->GetInitializer() == 0))
		{
			mErrorBuffer.PushError(
				ParseError::UNINITIALIZED_CONST,
				namedInitializer->GetName());
		}
	}
}


void TypeEvaluationPass::Visit(IfStatement *ifStatement)
{
	ParseNodeTraverser::Visit(ifStatement);
	const Expression *condition = ifStatement->GetCondition();
	if ((condition != 0) && !IsBooleanExpression(condition))
	{
		mErrorBuffer.PushError(
			ParseError::IF_CONDITION_IS_NOT_BOOLEAN,
			condition->GetContextToken());
	}
}


void TypeEvaluationPass::Visit(SwitchStatement *switchStatement)
{
	ParseNodeTraverser::Visit(switchStatement);
	const Expression *control = switchStatement->GetControl();
	if ((control != 0) && !IsIntegerExpression(control))
	{
		mErrorBuffer.PushError(
			ParseError::SWITCH_CONTROL_IS_NOT_INTEGER,
			control->GetContextToken());
	}
}


void TypeEvaluationPass::Visit(SwitchLabel *switchLabel)
{
	BoolStack::Element constExpressionElement(mEnforceConstExpressions, true);
	ParseNodeTraverser::Visit(switchLabel);
	const Expression *expression = switchLabel->GetExpression();
	if ((expression != 0) && !IsIntegerExpression(expression))
	{
		mErrorBuffer.PushError(
			ParseError::SWITCH_LABEL_IS_NOT_CONST_INTEGER,
			expression->GetContextToken());
	}
}


void TypeEvaluationPass::Visit(WhileStatement *whileStatement)
{
	ParseNodeTraverser::Visit(whileStatement);
	const Expression *condition = whileStatement->GetCondition();
	if ((condition != 0) && !IsBooleanExpression(condition))
	{
		mErrorBuffer.PushError(
			ParseError::WHILE_CONDITION_IS_NOT_BOOLEAN,
			condition->GetContextToken());
	}
}


void TypeEvaluationPass::Visit(ForStatement *forStatement)
{
	SemanticAnalysisPass::Visit(forStatement);
	const Expression *condition = forStatement->GetCondition();
	if ((condition != 0) && !IsBooleanExpression(condition))
	{
		mErrorBuffer.PushError(
			ParseError::FOR_CONDITION_IS_NOT_BOOLEAN,
			condition->GetContextToken());
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
				AssertNonConstExpression(op);
				resultType = *rhDescriptor;
				resultType.SetRValue();
				break;

			case Token::ASSIGN:
				AssertAssignableType(lhDescriptor, op);
				AssertNonConstExpression(op);
				// TODO: Assert that rhTav type can be assigned to lhTav.
				resultType = *lhDescriptor;
				break;

			case Token::ASSIGN_LEFT:
			case Token::ASSIGN_RIGHT:
			case Token::ASSIGN_MOD:
			case Token::ASSIGN_AND:
			case Token::ASSIGN_OR:
			case Token::ASSIGN_XOR:
				AssertIntegerOperand(lhDescriptor, op);
				AssertIntegerOperand(rhDescriptor, op);
				AssertAssignableType(lhDescriptor, op);
				AssertNonConstExpression(op);
				resultType = *lhDescriptor;
				break;

			case Token::ASSIGN_PLUS:
			case Token::ASSIGN_MINUS:
				if (lhDescriptor->IsPointerType())
				{
					AssertIntegerOperand(rhDescriptor, op);
				}
				else if (rhDescriptor->IsPointerType())
				{
					AssertIntegerOperand(lhDescriptor, op);
				}
				else
				{
					AssertNumericOperand(lhDescriptor, op);
					AssertNumericOperand(rhDescriptor, op);
				}

				AssertAssignableType(lhDescriptor, op);
				AssertNonConstExpression(op);
				resultType = *lhDescriptor;
				break;

			case Token::ASSIGN_MULT:
			case Token::ASSIGN_DIV:
				AssertNumericOperand(lhDescriptor, op);
				AssertNumericOperand(rhDescriptor, op);
				AssertAssignableType(lhDescriptor, op);
				AssertNonConstExpression(op);
				resultType = *lhDescriptor;
				break;

			case Token::OP_AND:
			case Token::OP_OR:
				AssertBooleanOperand(lhDescriptor, op);
				AssertBooleanOperand(rhDescriptor, op);
				resultType = *lhDescriptor;
				break;

			case Token::OP_BIT_AND:
			case Token::OP_BIT_OR:
			case Token::OP_BIT_XOR:
			case Token::OP_MOD:
				AssertIntegerOperand(lhDescriptor, op);
				AssertIntegerOperand(rhDescriptor, op);
				resultType = CombineOperandTypes(lhDescriptor, rhDescriptor);
				break;

			case Token::OP_LEFT:
			case Token::OP_RIGHT:
				AssertIntegerOperand(lhDescriptor, op);
				AssertIntegerOperand(rhDescriptor, op);
				resultType = *lhDescriptor;
				break;

			case Token::OP_LT:
			case Token::OP_LTE:
			case Token::OP_GT:
			case Token::OP_GTE:
			case Token::OP_EQUAL:
			case Token::OP_NOT_EQUAL:
				AssertComparableTypes(lhDescriptor, rhDescriptor, op);
				resultType = BOOL_TYPE_DESCRIPTOR;
				break;

			case Token::OP_PLUS:
			case Token::OP_MINUS:
				if (lhDescriptor->IsPointerType())
				{
					AssertIntegerOperand(rhDescriptor, op);
				}
				else if (rhDescriptor->IsPointerType())
				{
					AssertIntegerOperand(lhDescriptor, op);
				}
				else
				{
					AssertNumericOperand(lhDescriptor, op);
					AssertNumericOperand(rhDescriptor, op);
				}
				resultType = CombineOperandTypes(lhDescriptor, rhDescriptor);
				break;

			case Token::OP_MULT:
			case Token::OP_DIV:
				AssertNumericOperand(lhDescriptor, op);
				AssertNumericOperand(rhDescriptor, op);
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
				AssertNumericOperand(rhDescriptor, op);
				break;

			case Token::OP_INC:
			case Token::OP_DEC:
				AssertAssignableType(rhDescriptor, op);
				AssertNonConstExpression(op);
				if (!rhDescriptor->IsPointerType())
				{
					AssertNumericOperand(rhDescriptor, op);
				}
				break;

			case Token::OP_NOT:
				AssertBooleanOperand(rhDescriptor, op);
				break;

			case Token::OP_BIT_AND:
				AssertLValueType(rhDescriptor, op);
				resultType = TypeDescriptor(rhDescriptor, true);
				break;

			case Token::OP_BIT_NOT:
				AssertIntegerOperand(rhDescriptor, op);
				break;

			case Token::OP_MULT:
				AssertPointerOperand(rhDescriptor, op);
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
		AssertNonConstExpression(op);
		if (!lhDescriptor->IsPointerType())
		{
			AssertNumericOperand(lhDescriptor, op);
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
			AssertPointerOperand(lhDescriptor, op);
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

		AssertPointerOperand(lhDescriptor, op);
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
			if (mEnforceConstExpressions.GetTop())
			{
				mErrorBuffer.PushError(
					ParseError::FUNCTION_CALL_IN_CONST_EXPRESSION,
					functionCallExpression->GetContextToken());
			}
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
	tav.SetTypeDescriptor(&UINT_TYPE_DESCRIPTOR);
}


void TypeEvaluationPass::Visit(ConstantExpression *constantExpression)
{
	const Token *token = constantExpression->GetValueToken();
	const TypeDescriptor *descriptor = &INT_TYPE_DESCRIPTOR;

	switch (token->GetTokenType())
	{
		case Token::CONST_BOOL:
			descriptor = &BOOL_TYPE_DESCRIPTOR;
			break;
		case Token::CONST_CHAR:
			descriptor = &CHAR_TYPE_DESCRIPTOR;
			break;
		case Token::CONST_INT:
			descriptor = &INT_TYPE_DESCRIPTOR;
			break;
		case Token::CONST_UINT:
			descriptor = &UINT_TYPE_DESCRIPTOR;
			break;
		case Token::CONST_FLOAT:
			descriptor = &FLOAT_TYPE_DESCRIPTOR;
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
		identifierExpression->SetDefinition(symbol);
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


bool TypeEvaluationPass::IsBooleanExpression(const Expression *expression) const
{
	const TypeAndValue &tav = expression->GetTypeAndValue();
	if (tav.IsTypeDefined())
	{
		const TypeDescriptor *descriptor = tav.GetTypeDescriptor();
		return descriptor->IsBooleanType();
	}
	return false;
}


bool TypeEvaluationPass::IsIntegerExpression(const Expression *expression) const
{
	const TypeAndValue &tav = expression->GetTypeAndValue();
	if (tav.IsTypeDefined())
	{
		const TypeDescriptor *descriptor = tav.GetTypeDescriptor();
		return descriptor->IsIntegerType();
	}
	return false;
}


void TypeEvaluationPass::AssertNonConstExpression(const Token *op)
{
	if (mEnforceConstExpressions.GetTop())
	{
		mErrorBuffer.PushError(ParseError::INVALID_OPERATOR_IN_CONST_EXPRESSION, op);
	}
}


void TypeEvaluationPass::AssertBooleanOperand(const TypeDescriptor *descriptor, const Token *op)
{
	if (!descriptor->IsBooleanType())
	{
		mErrorBuffer.PushError(ParseError::INVALID_TYPE_FOR_OPERATOR, op, descriptor);
	}
}


void TypeEvaluationPass::AssertIntegerOperand(const TypeDescriptor *descriptor, const Token *op)
{
	if (!descriptor->IsIntegerType())
	{
		mErrorBuffer.PushError(ParseError::INVALID_TYPE_FOR_OPERATOR, op, descriptor);
	}
}


void TypeEvaluationPass::AssertNumericOperand(const TypeDescriptor *descriptor, const Token *op)
{
	if (!descriptor->IsNumericType())
	{
		mErrorBuffer.PushError(ParseError::INVALID_TYPE_FOR_OPERATOR, op, descriptor);
	}
}


void TypeEvaluationPass::AssertPointerOperand(const TypeDescriptor *descriptor, const Token *op)
{
	if (!descriptor->IsPointerType())
	{
		mErrorBuffer.PushError(ParseError::INVALID_TYPE_FOR_POINTER_OPERATOR, op, descriptor);
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
	if (descriptor->IsRValue())
	{
		mErrorBuffer.PushError(ParseError::RVALUE_ASSIGNMENT, op);
	}
	else if (!descriptor->IsAssignable())
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
