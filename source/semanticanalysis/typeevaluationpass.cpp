namespace Bond
{

// Type descriptors aren't yet fully resolved, so we cannot yet fully validate types in assignments,
// initializers, function calls and so forth, however we can test if the types are convertible. That
// will ensure that the types are sane enough to continue the analysis.
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
	virtual void Visit(StructDeclaration *structDeclaration);
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
	virtual void Visit(ThisExpression *thisExpression);

private:
	typedef AutoStack<const StructDeclaration *> StructStack;

	class RecursiveStructAnalyzer: private ParseNodeTraverser
	{
	public:
		RecursiveStructAnalyzer(ParseErrorBuffer &errorBuffer):
			mErrorBuffer(errorBuffer),
			mTopLevelStruct(0)
		{}

		virtual ~RecursiveStructAnalyzer() {}

		void Analyze(const StructDeclaration *structDeclaration);

	private:
		virtual void Visit(const StructDeclaration *structDeclaration);
		virtual void Visit(const FunctionDefinition *functionDefinition) {}
		virtual void Visit(const DeclarativeStatement *declarativeStatement);
		virtual void Visit(const TypeDescriptor *typeDescriptor);
		virtual void Visit(const TypeSpecifier *typeSpecifier);

		StructStack mStructStack;
		ParseErrorBuffer &mErrorBuffer;
		const StructDeclaration *mTopLevelStruct;
	};

	bool AssertBooleanExpression(const Expression *expression, ParseError::Type errorType) const;
	bool AssertIntegerExpression(const Expression *expression, ParseError::Type errorType, const void *arg = 0) const;
	bool AssertNonConstExpression(const Token *op);
	bool AssertBooleanOperand(const TypeDescriptor *typeDescriptor, const Token *op);
	bool AssertIntegerOperand(const TypeDescriptor *typeDescriptor, const Token *op);
	bool AssertNumericOperand(const TypeDescriptor *typeDescriptor, const Token *op);
	bool AssertPointerOperand(const TypeDescriptor *typeDescriptor, const Token *op);
	bool AssertLValueType(const TypeDescriptor *typeDescriptor, const Token *op);
	bool AssertAssignableType(const TypeDescriptor *typeDescriptor, const Token *op);

	bool AssertConvertibleTypes(
		const TypeDescriptor *fromType,
		const TypeDescriptor *toType,
		const Token *context,
		ParseError::Type errorType);

	bool AssertComparableTypes(const TypeDescriptor *typeA, const TypeDescriptor *typeB, const Token *op);

	void ValidateInitializer(
		const Token *name,
		const Initializer *initializer,
		const TypeDescriptor *typeDescrioptor);

	BoolStack mAddNamedInitializers;
	BoolStack mEnforceConstExpressions;
	BoolStack mEnforceConstDeclarations;
	BoolStack mInConstFunction;
	StructStack mStructStack;
};


void TypeEvaluationPass::Analyze(TranslationUnit *translationUnitList)
{
	BoolStack::Element initalizerElement(mAddNamedInitializers, false);
	BoolStack::Element constExpressionElement(mEnforceConstExpressions, false);
	BoolStack::Element constTypeDescriptorElement(mEnforceConstDeclarations, true);
	BoolStack::Element inConstFunctionElement(mInConstFunction, false);
	StructStack::Element stackElement(mStructStack, 0);
	SemanticAnalysisPass::Analyze(translationUnitList);
}


void TypeEvaluationPass::Visit(Enumerator *enumerator)
{
	BoolStack::Element constExpressionElement(mEnforceConstExpressions, true);
	ParseNodeTraverser::Visit(enumerator);
	const Expression *value = enumerator->GetValue();
	if (value != 0)
	{
		AssertIntegerExpression(value, ParseError::ENUMERATOR_VALUE_IS_NOT_CONST_INTEGER, enumerator->GetName());
	}
}


void TypeEvaluationPass::Visit(StructDeclaration *structDeclaration)
{
	BoolStack::Element constTypeDescriptorElement(mEnforceConstDeclarations, false);
	StructStack::Element stackElement(mStructStack, structDeclaration);
	SemanticAnalysisPass::Visit(structDeclaration);
	RecursiveStructAnalyzer analyzer(mErrorBuffer);
	analyzer.Analyze(structDeclaration);
}


void TypeEvaluationPass::Visit(FunctionDefinition *functionDefinition)
{
	// Top-level named initializers have already been added to the symbol table, but not ones in local scopes.
	// Top-level identifiers can be referenced out of order from their declarations, but local ones must be
	// declared before being referenced, so they must be added as the expressions are evaluated.
	BoolStack::Element initalizerElement(mAddNamedInitializers, true);
	BoolStack::Element constTypeDescriptorElement(mEnforceConstDeclarations, false);
	BoolStack::Element inConstFunctionElement(mInConstFunction, functionDefinition->GetPrototype()->IsConst());
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
	Expression *expressionList = typeDescriptor->GetLengthExpressionList();
	while (expressionList != 0)
	{
		if (CastNode<EmptyExpression>(expressionList) != 0)
		{
			expressionList->SetTypeDescriptor(TypeDescriptor::GetUIntType());
		}
		else
		{
			AssertIntegerExpression(expressionList, ParseError::ARRAY_SIZE_IS_NOT_CONST_INTEGER);
		}
		expressionList = NextNode(expressionList);
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
		const Initializer *initializer = namedInitializer->GetInitializer();

		if (initializer != 0)
		{
			ValidateInitializer(namedInitializer->GetName(), initializer, typeDescriptor);
		}
		else if (typeDescriptor->IsConst())
		{
			mErrorBuffer.PushError(ParseError::UNINITIALIZED_CONST, namedInitializer->GetName());
		}

		if (mEnforceConstDeclarations.GetTop() && !typeDescriptor->IsConst())
		{
			mErrorBuffer.PushError(ParseError::NON_CONST_DECLARATION, namedInitializer->GetName());
		}
	}
}


void TypeEvaluationPass::Visit(IfStatement *ifStatement)
{
	ParseNodeTraverser::Visit(ifStatement);
	const Expression *condition = ifStatement->GetCondition();
	AssertBooleanExpression(condition, ParseError::IF_CONDITION_IS_NOT_BOOLEAN);
}


void TypeEvaluationPass::Visit(SwitchStatement *switchStatement)
{
	ParseNodeTraverser::Visit(switchStatement);
	const Expression *control = switchStatement->GetControl();
	AssertIntegerExpression(control, ParseError::SWITCH_CONTROL_IS_NOT_INTEGER);
}


void TypeEvaluationPass::Visit(SwitchLabel *switchLabel)
{
	BoolStack::Element constExpressionElement(mEnforceConstExpressions, true);
	ParseNodeTraverser::Visit(switchLabel);
	const Expression *expression = switchLabel->GetExpression();
	if (expression != 0)
	{
		AssertIntegerExpression(expression, ParseError::SWITCH_LABEL_IS_NOT_CONST_INTEGER);
	}
}


void TypeEvaluationPass::Visit(WhileStatement *whileStatement)
{
	ParseNodeTraverser::Visit(whileStatement);
	const Expression *condition = whileStatement->GetCondition();
	AssertBooleanExpression(condition, ParseError::WHILE_CONDITION_IS_NOT_BOOLEAN);
}


void TypeEvaluationPass::Visit(ForStatement *forStatement)
{
	SemanticAnalysisPass::Visit(forStatement);
	const Expression *condition = forStatement->GetCondition();
	if (condition != 0)
	{
		AssertBooleanExpression(condition, ParseError::FOR_CONDITION_IS_NOT_BOOLEAN);
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

		if (!AreConvertibleTypes(trueDescriptor, falseDescriptor))
		{
			mErrorBuffer.PushError(
				ParseError::TERNARY_OPERAND_TYPE_MISMATCH,
				conditionalExpression->GetContextToken(),
				trueDescriptor,
				falseDescriptor);
		}

		TypeDescriptor resultType = CombineOperandTypes(trueDescriptor, falseDescriptor);
		conditionalExpression->SetTypeDescriptor(resultType);
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
		bool isResolvable = true;

		switch (op->GetTokenType())
		{
			case Token::COMMA:
				AssertNonConstExpression(op);
				resultType = *rhDescriptor;
				resultType.SetRValue();
				break;

			case Token::ASSIGN:
				AssertAssignableType(lhDescriptor, op);
				AssertConvertibleTypes(rhDescriptor, lhDescriptor, op, ParseError::INVALID_TYPE_ASSIGNMENT) &&
				AssertNonConstExpression(op);
				resultType = *lhDescriptor;
				break;

			case Token::ASSIGN_LEFT:
			case Token::ASSIGN_RIGHT:
			case Token::ASSIGN_MOD:
			case Token::ASSIGN_AND:
			case Token::ASSIGN_OR:
			case Token::ASSIGN_XOR:
				isResolvable = AssertIntegerOperand(lhDescriptor, op) && AssertIntegerOperand(rhDescriptor, op);
				AssertAssignableType(lhDescriptor, op);
				AssertNonConstExpression(op);
				resultType = *lhDescriptor;
				break;

			case Token::ASSIGN_PLUS:
			case Token::ASSIGN_MINUS:
				if (lhDescriptor->IsPointerType())
				{
					isResolvable = isResolvable && AssertIntegerOperand(rhDescriptor, op);
				}
				else if (rhDescriptor->IsPointerType())
				{
					isResolvable = isResolvable && AssertIntegerOperand(lhDescriptor, op);
				}
				else
				{
					isResolvable = isResolvable &&
						AssertNumericOperand(lhDescriptor, op) &&
						AssertNumericOperand(rhDescriptor, op);
				}

				AssertAssignableType(lhDescriptor, op);
				AssertNonConstExpression(op);
				resultType = *lhDescriptor;
				break;

			case Token::ASSIGN_MULT:
			case Token::ASSIGN_DIV:
				isResolvable = AssertNumericOperand(lhDescriptor, op) && AssertNumericOperand(rhDescriptor, op);
				AssertAssignableType(lhDescriptor, op);
				AssertNonConstExpression(op);
				resultType = *lhDescriptor;
				break;

			case Token::OP_AND:
			case Token::OP_OR:
				isResolvable =
					AssertBooleanOperand(lhDescriptor, op) &&
					AssertBooleanOperand(rhDescriptor, op);
				resultType = *lhDescriptor;
				break;

			case Token::OP_AMP:
			case Token::OP_BIT_OR:
			case Token::OP_BIT_XOR:
			case Token::OP_MOD:
				isResolvable =
					AssertIntegerOperand(lhDescriptor, op) &&
					AssertIntegerOperand(rhDescriptor, op);
				resultType = CombineOperandTypes(lhDescriptor, rhDescriptor);
				break;

			case Token::OP_LEFT:
			case Token::OP_RIGHT:
				isResolvable =
					AssertIntegerOperand(lhDescriptor, op) &&
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
				resultType = TypeDescriptor::GetBoolType();
				break;

			case Token::OP_PLUS:
			case Token::OP_MINUS:
				if (lhDescriptor->IsPointerType())
				{
					isResolvable = isResolvable && AssertIntegerOperand(rhDescriptor, op);
				}
				else if (rhDescriptor->IsPointerType())
				{
					isResolvable = isResolvable && AssertIntegerOperand(lhDescriptor, op);
				}
				else
				{
					isResolvable = isResolvable &&
						AssertNumericOperand(lhDescriptor, op) &&
						AssertNumericOperand(rhDescriptor, op);
				}
				resultType = CombineOperandTypes(lhDescriptor, rhDescriptor);
				break;

			case Token::OP_STAR:
			case Token::OP_DIV:
				isResolvable = isResolvable &&
					AssertNumericOperand(lhDescriptor, op) &&
					AssertNumericOperand(rhDescriptor, op);
				resultType = CombineOperandTypes(lhDescriptor, rhDescriptor);
				break;

			default:
				break;
		}

		if (isResolvable)
		{
			resultType.SetRValue();
			binaryExpression->SetTypeDescriptor(resultType);
		}
	}
}


void TypeEvaluationPass::Visit(UnaryExpression *unaryExpression)
{
	ParseNodeTraverser::Visit(unaryExpression);

	TypeAndValue &rhTav = unaryExpression->GetRhs()->GetTypeAndValue();

	if (rhTav.IsTypeDefined())
	{
		TypeDescriptor *rhDescriptor = rhTav.GetTypeDescriptor();
		TypeDescriptor resultType = *rhDescriptor;
		resultType.SetRValue();
		const Token *op = unaryExpression->GetOperator();
		bool isResolvable = true;
		bool isRValue = true;

		switch (op->GetTokenType())
		{
			case Token::OP_PLUS:
			case Token::OP_MINUS:
				isResolvable = AssertNumericOperand(rhDescriptor, op);
				resultType = PromoteType(rhDescriptor);
				break;

			case Token::OP_INC:
			case Token::OP_DEC:
				isResolvable =
					(rhDescriptor->IsPointerType() || AssertNumericOperand(rhDescriptor, op)) &&
					AssertAssignableType(rhDescriptor, op);
				AssertNonConstExpression(op);
				resultType = PromoteType(rhDescriptor);
				break;

			case Token::OP_NOT:
				isResolvable = AssertBooleanOperand(rhDescriptor, op);
				resultType = PromoteType(rhDescriptor);
				break;

			case Token::OP_AMP:
				AssertLValueType(rhDescriptor, op);
				resultType = TypeDescriptor(rhDescriptor, false);
				break;

			case Token::OP_BIT_NOT:
				isResolvable = AssertIntegerOperand(rhDescriptor, op);
				resultType = PromoteType(rhDescriptor);
				break;

			case Token::OP_STAR:
				isResolvable = AssertPointerOperand(rhDescriptor, op);
				if (rhDescriptor->IsPointerType())
				{
					resultType = rhDescriptor->GetDereferencedType();
					if (resultType.IsVoidType())
					{
						mErrorBuffer.PushError(ParseError::VOID_POINTER_DEREFERENCE, op, rhDescriptor);
					}
				}
				isRValue = false;
				break;

			default:
				break;
		}

		if (isResolvable)
		{
			if (isRValue)
			{
				resultType.SetRValue();
			}

			unaryExpression->SetTypeDescriptor(resultType);
		}
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
		if (lhDescriptor->IsPointerType() || AssertNumericOperand(lhDescriptor, op))
		{
			AssertAssignableType(lhDescriptor, op);
			AssertNonConstExpression(op);
			postfixExpression->SetTypeDescriptor(*lhDescriptor);
		}
	}
}


void TypeEvaluationPass::Visit(MemberExpression *memberExpression)
{
	ParseNodeTraverser::Visit(memberExpression);

	const TypeAndValue &lhTav = memberExpression->GetLhs()->GetTypeAndValue();

	if (lhTav.IsTypeDefined())
	{
		const TypeDescriptor *lhDescriptor = lhTav.GetTypeDescriptor();
		TypeDescriptor structDescriptor = *lhDescriptor;
		const Token *op = memberExpression->GetOperator();
		const Token *memberName = memberExpression->GetMemberName();

		if (op->GetTokenType() == Token::OP_ARROW)
		{
			AssertPointerOperand(lhDescriptor, op);
			if (lhDescriptor->IsPointerType())
			{
				structDescriptor = lhDescriptor->GetDereferencedType();
			}
		}

		const TypeSpecifier *structSpecifier = structDescriptor.GetTypeSpecifier();
		if ((structSpecifier == 0) ||
		    (structSpecifier->GetDefinition() == 0) ||
		    (structSpecifier->GetDefinition()->GetSymbolType() != Symbol::TYPE_STRUCT))
		{
			mErrorBuffer.PushError(ParseError::NON_STRUCT_MEMBER_REQUEST, memberName, lhDescriptor);
		}
		else
		{
			const Symbol *structDeclaration = CastNode<StructDeclaration>(structSpecifier->GetDefinition());
			const Symbol *member = structDeclaration->FindSymbol(memberName);
			if (member == 0)
			{
				mErrorBuffer.PushError(ParseError::INVALID_MEMBER_REQUEST, memberName, lhDescriptor);
			}
			else
			{
				TypeDescriptor memberDescriptor = *member->GetTypeAndValue()->GetTypeDescriptor();
				if (structDescriptor.IsConst())
				{
					const NamedInitializer *namedInitializer = CastNode<NamedInitializer>(member);
					if (namedInitializer != 0)
					{
						memberDescriptor.SetConst();
					}
					else
					{
						const FunctionDefinition *functionDefinition = CastNode<FunctionDefinition>(member);
						if ((functionDefinition != 0) && !functionDefinition->GetPrototype()->IsConst())
						{
							mErrorBuffer.PushError(
								ParseError::NON_CONST_MEMBER_FUNCTION_REQUEST,
								memberName,
								functionDefinition->GetPrototype(),
								lhDescriptor);
						}
					}
				}
				memberExpression->SetTypeDescriptor(memberDescriptor);
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
			arraySubscriptExpression->SetTypeDescriptor(lhDescriptor->GetDereferencedType());
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
		const Token *context = functionCallExpression->GetContextToken();

		if ((lhSpecifier == 0) ||
		    (lhSpecifier->GetDefinition() == 0) ||
		    (lhSpecifier->GetDefinition()->GetSymbolType() != Symbol::TYPE_FUNCTION))
		{
			mErrorBuffer.PushError(ParseError::EXPRESSION_IS_NOT_CALLABLE, context);
		}
		else
		{
			if (mEnforceConstExpressions.GetTop())
			{
				mErrorBuffer.PushError(ParseError::FUNCTION_CALL_IN_CONST_EXPRESSION, context);
			}

			const FunctionDefinition *function = CastNode<FunctionDefinition>(lhSpecifier->GetDefinition());
			const FunctionPrototype *prototype = function->GetPrototype();
			const Parameter *paramList = prototype->GetParameterList();
			const Expression *argList = functionCallExpression->GetArgumentList();
			const bu32_t numParams = GetLength(paramList);
			const bu32_t numArgs = GetLength(argList);

			if (numParams == numArgs)
			{
				while ((paramList != 0) && (argList != 0))
				{
					const TypeAndValue &argTav = argList->GetTypeAndValue();
					if (argTav.IsTypeDefined())
					{
						const TypeDescriptor *paramDescriptor = paramList->GetTypeDescriptor();
						const TypeDescriptor *argDescriptor = argTav.GetTypeDescriptor();
						AssertConvertibleTypes(
							argDescriptor,
							paramDescriptor,
							argList->GetContextToken(),
							ParseError::INVALID_TYPE_CONVERSION);
						paramList = NextNode(paramList);
						argList = NextNode(argList);
					}
				}
			}
			else
			{
				mErrorBuffer.PushError(ParseError::INCORRECT_NUMBER_OF_ARGS, context, prototype);
			}

			const TypeDescriptor *returnType = prototype->GetReturnType();
			functionCallExpression->SetTypeDescriptor(*returnType);
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

		AssertConvertibleTypes(
			rhDescriptor,
			lhDescriptor,
			lhDescriptor->GetContextToken(),
			ParseError::INVALID_TYPE_CONVERSION);

		if (rhDescriptor->IsLValue())
		{
			lhDescriptor->SetLValue();
		}

		castExpression->SetTypeDescriptor(*lhDescriptor);
	}
}


void TypeEvaluationPass::Visit(SizeofExpression *sizeofExpression)
{
	ParseNodeTraverser::Visit(sizeofExpression);
	sizeofExpression->SetTypeDescriptor(TypeDescriptor::GetUIntType());
}


void TypeEvaluationPass::Visit(ConstantExpression *constantExpression)
{
	const Token *token = constantExpression->GetValueToken();
	TypeDescriptor typeDescriptor = TypeDescriptor::GetIntType();

	switch (token->GetTokenType())
	{
		case Token::CONST_BOOL:
			typeDescriptor = TypeDescriptor::GetBoolType();
			break;
		case Token::CONST_CHAR:
			typeDescriptor = TypeDescriptor::GetCharType();
			break;
		case Token::CONST_INT:
			typeDescriptor = TypeDescriptor::GetIntType();
			break;
		case Token::CONST_UINT:
			typeDescriptor = TypeDescriptor::GetUIntType();
			break;
		case Token::CONST_FLOAT:
			typeDescriptor = TypeDescriptor::GetFloatType();
			break;
		case Token::CONST_STRING:
			typeDescriptor = TypeDescriptor::GetStringType();
			break;
		case Token::CONST_NULL:
			typeDescriptor = TypeDescriptor::GetNullType();
			break;
		default:
			// Ignore the default case because the parser is not supposed to allow it to happen.
			break;
	}

	constantExpression->SetTypeDescriptor(typeDescriptor);
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
			TypeDescriptor typeDescriptor = *symbolTav->GetTypeDescriptor();

			// Verify if the symbol was reached by implicitly dereferencing a const 'this' pointer.
			if ((symbol->GetParentSymbol() == mStructStack.GetTop()) && mInConstFunction.GetTop())
			{
				const NamedInitializer *namedInitializer = CastNode<NamedInitializer>(symbol);
				if (namedInitializer != 0)
				{
					typeDescriptor.SetConst();
				}
				else
				{
					const FunctionDefinition *functionDefinition = CastNode<FunctionDefinition>(symbol);
					if ((functionDefinition != 0) && !functionDefinition->GetPrototype()->IsConst())
					{
						mErrorBuffer.PushError(
							ParseError::NON_CONST_MEMBER_FUNCTION_REQUEST,
							identifier->GetContextToken(),
							functionDefinition->GetPrototype(),
							mStructStack.GetTop().GetValue()->GetConstThisTypeDescriptor());
					}
				}
			}
			identifierExpression->SetTypeDescriptor(typeDescriptor);
		}
	}
}


void TypeEvaluationPass::Visit(ThisExpression *thisExpression)
{
	const StructDeclaration *structDeclaration = mStructStack.GetTop();
	if (structDeclaration != 0)
	{
		if (mInConstFunction.GetTop())
		{
			thisExpression->SetTypeDescriptor(*structDeclaration->GetConstThisTypeDescriptor());
		}
		else
		{
			thisExpression->SetTypeDescriptor(*structDeclaration->GetThisTypeDescriptor());
		}
	}
	else
	{
		mErrorBuffer.PushError(ParseError::THIS_IN_NON_MEMBER_FUNCTION, thisExpression->GetContextToken());
	}
}


bool TypeEvaluationPass::AssertBooleanExpression(const Expression *expression, ParseError::Type errorType) const
{
	const TypeAndValue &tav = expression->GetTypeAndValue();
	if (tav.IsTypeDefined())
	{
		const TypeDescriptor *typeDescriptor = tav.GetTypeDescriptor();
		if (!typeDescriptor->IsBooleanType())
		{
			mErrorBuffer.PushError(errorType, expression->GetContextToken());
			return false;
		}
	}
	return true;
}


bool TypeEvaluationPass::AssertIntegerExpression(
	const Expression *expression,
	ParseError::Type errorType,
	const void *arg) const
{
	const TypeAndValue &tav = expression->GetTypeAndValue();
	if (tav.IsTypeDefined())
	{
		const TypeDescriptor *typeDescriptor = tav.GetTypeDescriptor();
		if (!typeDescriptor->IsIntegerType())
		{
			mErrorBuffer.PushError(errorType, expression->GetContextToken(), arg);
			return false;
		}
	}
	return true;
}


bool TypeEvaluationPass::AssertNonConstExpression(const Token *op)
{
	if (mEnforceConstExpressions.GetTop())
	{
		mErrorBuffer.PushError(ParseError::INVALID_OPERATOR_IN_CONST_EXPRESSION, op);
		return false;
	}
	return true;
}


bool TypeEvaluationPass::AssertBooleanOperand(const TypeDescriptor *typeDescriptor, const Token *op)
{
	if (!typeDescriptor->IsBooleanType())
	{
		mErrorBuffer.PushError(ParseError::INVALID_TYPE_FOR_OPERATOR, op, typeDescriptor);
		return false;		
	}
	return true;
}


bool TypeEvaluationPass::AssertIntegerOperand(const TypeDescriptor *typeDescriptor, const Token *op)
{
	if (!typeDescriptor->IsIntegerType())
	{
		mErrorBuffer.PushError(ParseError::INVALID_TYPE_FOR_OPERATOR, op, typeDescriptor);
		return false;
	}
	return true;
}


bool TypeEvaluationPass::AssertNumericOperand(const TypeDescriptor *typeDescriptor, const Token *op)
{
	if (!typeDescriptor->IsNumericType())
	{
		mErrorBuffer.PushError(ParseError::INVALID_TYPE_FOR_OPERATOR, op, typeDescriptor);
		return false;
	}
	return true;
}


bool TypeEvaluationPass::AssertPointerOperand(const TypeDescriptor *typeDescriptor, const Token *op)
{
	if (!typeDescriptor->IsPointerType())
	{
		mErrorBuffer.PushError(ParseError::INVALID_TYPE_FOR_POINTER_OPERATOR, op, typeDescriptor);
		return false;
	}
	return true;
}


bool TypeEvaluationPass::AssertLValueType(const TypeDescriptor *typeDescriptor, const Token *op)
{
	if (!typeDescriptor->IsLValue())
	{
		mErrorBuffer.PushError(ParseError::NON_LVALUE_TYPE, op, typeDescriptor);
		return false;
	}
	return true;
}


bool TypeEvaluationPass::AssertAssignableType(const TypeDescriptor *typeDescriptor, const Token *op)
{
	if (typeDescriptor->IsRValue())
	{
		mErrorBuffer.PushError(ParseError::NON_LVALUE_ASSIGNMENT, op);
		return false;
	}
	else if (!typeDescriptor->IsAssignable())
	{
		mErrorBuffer.PushError(ParseError::UNASSIGNABLE_TYPE, op, typeDescriptor);
		return false;
	}
	return true;
}


bool TypeEvaluationPass::AssertConvertibleTypes(
	const TypeDescriptor *fromType,
	const TypeDescriptor *toType,
	const Token *context,
	ParseError::Type errorType)
{
	if (!AreConvertibleTypes(fromType, toType))
	{
		mErrorBuffer.PushError(errorType, context, fromType, toType);
		return false;
	}
	return true;
}


bool TypeEvaluationPass::AssertComparableTypes(const TypeDescriptor *typeA, const TypeDescriptor *typeB, const Token *op)
{
	if (!AreComparableTypes(typeA, typeB))
	{
		mErrorBuffer.PushError(ParseError::INVALID_COMPARISON, op, typeA, typeB);
		return false;
	}
	return true;
}


void TypeEvaluationPass::ValidateInitializer(
	const Token *name,
	const Initializer *initializer,
	const TypeDescriptor *typeDescriptor)
{
	const Expression *expression = initializer->GetExpression();
	const Initializer *initializerList = initializer->GetInitializerList();

	if (typeDescriptor->IsArrayType())
	{
		if (initializerList != 0)
		{
			const TypeDescriptor parent = typeDescriptor->GetDereferencedType();
			while (initializerList != 0)
			{
				ValidateInitializer(name, initializerList, &parent);
				initializerList = NextNode(initializerList);
			}
		}
		else if (expression != 0)
		{
			mErrorBuffer.PushError(
				ParseError::MISSING_BRACES_IN_INITIALIZER,
				initializer->GetContextToken(),
				typeDescriptor);
		}
	}
	else
	{
		if (expression != 0)
		{
			const TypeAndValue &tav = expression->GetTypeAndValue();
			if (tav.IsTypeDefined())
			{
				AssertConvertibleTypes(
					expression->GetTypeAndValue().GetTypeDescriptor(),
					typeDescriptor,
					expression->GetContextToken(),
					ParseError::INVALID_TYPE_CONVERSION);
			}
		}
		else if (initializerList != 0)
		{
			mErrorBuffer.PushError(
				ParseError::BRACES_AROUND_SCALAR_INITIALIZER,
				initializerList->GetContextToken(),
				typeDescriptor);
		}
	}
}


void TypeEvaluationPass::RecursiveStructAnalyzer::Analyze(const StructDeclaration *structDeclaration)
{
	mTopLevelStruct = structDeclaration;
	StructStack::Element stackElement(mStructStack, structDeclaration);
	ParseNodeTraverser::Visit(structDeclaration);
	mTopLevelStruct = 0;
}


void TypeEvaluationPass::RecursiveStructAnalyzer::Visit(const StructDeclaration *structDeclaration)
{
	if (structDeclaration == mTopLevelStruct)
	{
		mErrorBuffer.PushError(ParseError::RECURSIVE_STRUCT, structDeclaration->GetName());
	}

	if (!mStructStack.Contains(structDeclaration))
	{
		StructStack::Element stackElement(mStructStack, structDeclaration);
		ParseNodeTraverser::Visit(structDeclaration);
	}
}


void TypeEvaluationPass::RecursiveStructAnalyzer::Visit(const DeclarativeStatement *declarativeStatement)
{
	Traverse(declarativeStatement->GetTypeDescriptor());
}


void TypeEvaluationPass::RecursiveStructAnalyzer::Visit(const TypeDescriptor *typeDescriptor)
{
	if (!typeDescriptor->IsPointerIntrinsicType())
	{
		ParseNodeTraverser::Visit(typeDescriptor);
	}
}


void TypeEvaluationPass::RecursiveStructAnalyzer::Visit(const TypeSpecifier *typeSpecifier)
{
	if (typeSpecifier->GetDefinition() != 0)
	{
		Traverse(CastNode<StructDeclaration>(typeSpecifier->GetDefinition()));
	}
}

}
