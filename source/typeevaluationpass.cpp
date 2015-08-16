#include "private/typeevaluationpass.h"
#include "bond/compiler/parsenodes.h"
#include "bond/compiler/parsenodeutil.h"

namespace Bond
{

void TypeEvaluationPass::Analyze(TranslationUnit *translationUnitList)
{
	BoolStack::Element constExpressionElement(mEnforceConstExpressions, false);
	StructStack::Element structElement(mStruct, nullptr);
	FunctionStack::Element functionElement(mFunction, nullptr);
	SemanticAnalysisPass::Analyze(translationUnitList);
}


void TypeEvaluationPass::Visit(Enumerator *enumerator)
{
	BoolStack::Element constExpressionElement(mEnforceConstExpressions, true);
	ParseNodeTraverser::Visit(enumerator);
	const Expression *value = enumerator->GetValue();
	if (value != nullptr)
	{
		AssertMost32IntegerExpression(value, CompilerError::ENUMERATOR_VALUE_IS_NOT_CONST_INTEGER, enumerator->GetName());
	}
}


void TypeEvaluationPass::Visit(StructDeclaration *structDeclaration)
{
	StructStack::Element structElement(mStruct, structDeclaration);
	SemanticAnalysisPass::Visit(structDeclaration);
	RecursiveStructAnalyzer analyzer(mErrorBuffer);
	analyzer.Analyze(structDeclaration);
}


void TypeEvaluationPass::Visit(FunctionDefinition *functionDefinition)
{
	FunctionStack::Element functionElement(mFunction, functionDefinition);
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
	while (expressionList != nullptr)
	{
		if (CastNode<EmptyExpression>(expressionList) != nullptr)
		{
			expressionList->SetTypeDescriptor(TypeDescriptor::GetUIntType());
		}
		else
		{
			AssertMost32IntegerExpression(expressionList, CompilerError::ARRAY_SIZE_IS_NOT_CONST_INTEGER);
		}
		expressionList = NextNode(expressionList);
	}
}


void TypeEvaluationPass::Visit(NamedInitializer *namedInitializer)
{
	ParseNodeTraverser::Visit(namedInitializer);

	// Top-level named initializers have already been added to the symbol table, but not ones in local scopes.
	// Top-level identifiers can be referenced out of order from their declarations, but local ones must be
	// declared before being referenced, so they must be added as the expressions are evaluated.
	if (namedInitializer->GetScope() == SCOPE_LOCAL)
	{
		InsertSymbol(namedInitializer);
	}

	TypeAndValue &tav = *namedInitializer->GetTypeAndValue();
	if (tav.IsTypeDefined())
	{
		const TypeDescriptor *typeDescriptor = tav.GetTypeDescriptor();
		Initializer *initializer = namedInitializer->GetInitializer();

		if (initializer != nullptr)
		{
			ValidateInitializer(initializer, typeDescriptor);
		}
		else if (typeDescriptor->IsConst())
		{
			mErrorBuffer.PushError(CompilerError::UNINITIALIZED_CONST, namedInitializer->GetName());
		}

		if ((namedInitializer->GetScope() == SCOPE_GLOBAL) && !typeDescriptor->IsConst())
		{
			mErrorBuffer.PushError(CompilerError::NON_CONST_DECLARATION, namedInitializer->GetName());
		}
	}
}


void TypeEvaluationPass::Visit(IfStatement *ifStatement)
{
	ParseNodeTraverser::Visit(ifStatement);
	const Expression *condition = ifStatement->GetCondition();
	AssertBooleanExpression(condition, CompilerError::IF_CONDITION_IS_NOT_BOOLEAN);
}


void TypeEvaluationPass::Visit(SwitchStatement *switchStatement)
{
	ParseNodeTraverser::Visit(switchStatement);
	const Expression *control = switchStatement->GetControl();
	AssertMost32IntegerExpression(control, CompilerError::SWITCH_CONTROL_IS_NOT_INTEGER);
}


void TypeEvaluationPass::Visit(SwitchLabel *switchLabel)
{
	BoolStack::Element constExpressionElement(mEnforceConstExpressions, true);
	ParseNodeTraverser::Visit(switchLabel);
	const Expression *expression = switchLabel->GetExpression();
	if (expression != nullptr)
	{
		AssertMost32IntegerExpression(expression, CompilerError::SWITCH_LABEL_IS_NOT_CONST_INTEGER);
	}
}


void TypeEvaluationPass::Visit(WhileStatement *whileStatement)
{
	ParseNodeTraverser::Visit(whileStatement);
	const Expression *condition = whileStatement->GetCondition();
	AssertBooleanExpression(condition, CompilerError::WHILE_CONDITION_IS_NOT_BOOLEAN);
}


void TypeEvaluationPass::Visit(ForStatement *forStatement)
{
	SemanticAnalysisPass::Visit(forStatement);
	const Expression *condition = forStatement->GetCondition();
	if (condition != nullptr)
	{
		AssertBooleanExpression(condition, CompilerError::FOR_CONDITION_IS_NOT_BOOLEAN);
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
				CompilerError::TERNARY_OPERAND_TYPE_MISMATCH,
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
				break;

			case Token::ASSIGN:
				AssertAssignableType(lhDescriptor, op);
				AssertConvertibleTypes(rhDescriptor, lhDescriptor, op, CompilerError::INVALID_TYPE_ASSIGNMENT) &&
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
					isResolvable = AssertIntegerOperand(rhDescriptor, op);
				}
				else
				{
					isResolvable =
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
			case Token::OP_LEFT:
			case Token::OP_RIGHT:
				isResolvable =
					AssertIntegerOperand(lhDescriptor, op) &&
					AssertIntegerOperand(rhDescriptor, op);
				resultType = CombineOperandTypes(lhDescriptor, rhDescriptor);
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
				if (lhDescriptor->IsPointerType())
				{
					isResolvable = AssertIntegerOperand(rhDescriptor, op);
				}
				else if (rhDescriptor->IsPointerType())
				{
					isResolvable = AssertIntegerOperand(lhDescriptor, op);
				}
				else
				{
					isResolvable =
						AssertNumericOperand(lhDescriptor, op) &&
						AssertNumericOperand(rhDescriptor, op);
				}
				resultType = CombineOperandTypes(lhDescriptor, rhDescriptor);
				break;

			case Token::OP_MINUS:
				if (lhDescriptor->IsPointerType() && rhDescriptor->IsPointerType())
				{
					if (mPointerSize == POINTER_64BIT)
					{
						resultType = TypeDescriptor::GetLongType();
					}
					else
					{
						resultType = TypeDescriptor::GetIntType();
					}
					isResolvable = true;
				}
				else
				{
					if (lhDescriptor->IsPointerType())
					{
						isResolvable = AssertIntegerOperand(rhDescriptor, op);
					}
					else if (rhDescriptor->IsPointerType())
					{
						isResolvable = AssertIntegerOperand(lhDescriptor, op);
					}
					else
					{
						isResolvable =
							AssertNumericOperand(lhDescriptor, op) &&
							AssertNumericOperand(rhDescriptor, op);
					}
					resultType = CombineOperandTypes(lhDescriptor, rhDescriptor);
				}
				break;

			case Token::OP_STAR:
			case Token::OP_DIV:
				isResolvable = AssertNumericOperand(lhDescriptor, op) &&
					AssertNumericOperand(rhDescriptor, op);
				resultType = CombineOperandTypes(lhDescriptor, rhDescriptor);
				break;

			default:
				break;
		}

		if (isResolvable)
		{
			resultType.ClearLValue();
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
		resultType.ClearLValue();
		const Token *op = unaryExpression->GetOperator();
		bool isResolvable = true;
		bool isRValue = true;

		switch (op->GetTokenType())
		{
			case Token::OP_PLUS:
			case Token::OP_MINUS:
				isResolvable = AssertNumericOperand(rhDescriptor, op);
				break;

			case Token::OP_INC:
			case Token::OP_DEC:
				isResolvable =
					(rhDescriptor->IsPointerType() || AssertNumericOperand(rhDescriptor, op)) &&
					AssertAssignableType(rhDescriptor, op);
				AssertNonConstExpression(op);
				break;

			case Token::OP_NOT:
				isResolvable = AssertBooleanOperand(rhDescriptor, op);
				resultType = PromoteType(rhDescriptor);
				break;

			case Token::OP_AMP:
				AssertAddressableType(rhDescriptor, op);
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
						mErrorBuffer.PushError(CompilerError::VOID_POINTER_DEREFERENCE, op, rhDescriptor);
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
				resultType.ClearLValue();
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
			TypeDescriptor resultType = *lhDescriptor;
			resultType.ClearLValue();
			postfixExpression->SetTypeDescriptor(resultType);
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

		if (!structDescriptor.IsStructType())
		{
			mErrorBuffer.PushError(CompilerError::NON_STRUCT_MEMBER_REQUEST, memberName, lhDescriptor);
		}
		else
		{
			const TypeSpecifier *structSpecifier = structDescriptor.GetTypeSpecifier();
			const Symbol *structDeclaration = CastNode<StructDeclaration>(structSpecifier->GetDefinition());
			const Symbol *member = structDeclaration->FindSymbol(memberName);
			if (member == nullptr)
			{
				mErrorBuffer.PushError(CompilerError::INVALID_MEMBER_REQUEST, memberName, lhDescriptor);
			}
			else
			{
				memberExpression->SetDefinition(member);
				TypeDescriptor memberDescriptor = *member->GetTypeAndValue()->GetTypeDescriptor();
				if (structDescriptor.IsConst())
				{
					const NamedInitializer *namedInitializer = CastNode<NamedInitializer>(member);
					if (namedInitializer != nullptr)
					{
						memberDescriptor.SetConst();
					}
					else
					{
						const FunctionDefinition *functionDefinition = CastNode<FunctionDefinition>(member);
						if ((functionDefinition != nullptr) && !functionDefinition->GetPrototype()->IsConst())
						{
							mErrorBuffer.PushError(
								CompilerError::NON_CONST_MEMBER_FUNCTION_REQUEST,
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
			mErrorBuffer.PushError(CompilerError::INVALID_TYPE_FOR_INDEX_OPERATOR, op, indexDescriptor);
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

		if ((lhSpecifier == nullptr) ||
		    (lhSpecifier->GetDefinition() == nullptr) ||
		    (lhSpecifier->GetDefinition()->GetSymbolType() != Symbol::TYPE_FUNCTION))
		{
			mErrorBuffer.PushError(CompilerError::EXPRESSION_IS_NOT_CALLABLE, context);
		}
		else
		{
			if (mEnforceConstExpressions.GetTop())
			{
				mErrorBuffer.PushError(CompilerError::FUNCTION_CALL_IN_CONST_EXPRESSION, context);
			}

			const FunctionDefinition *function = CastNode<FunctionDefinition>(lhSpecifier->GetDefinition());
			const FunctionPrototype *prototype = function->GetPrototype();
			const Parameter *paramList = prototype->GetParameterList();
			const Expression *argList = functionCallExpression->GetArgumentList();
			const uint32_t numParams = GetLength(paramList);
			const uint32_t numArgs = GetLength(argList);

			if (numParams == numArgs)
			{
				while ((paramList != nullptr) && (argList != nullptr))
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
							CompilerError::INVALID_TYPE_CONVERSION);
					}
					paramList = NextNode(paramList);
					argList = NextNode(argList);
				}
			}
			else
			{
				mErrorBuffer.PushError(CompilerError::INCORRECT_NUMBER_OF_ARGS, context, prototype);
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
		TypeDescriptor *lhDescriptor = castExpression->GetTargetTypeDescriptor();

		AssertConvertibleTypes(
			rhDescriptor,
			lhDescriptor,
			lhDescriptor->GetContextToken(),
			CompilerError::INVALID_TYPE_CONVERSION);

		castExpression->SetTypeDescriptor(*lhDescriptor);
	}
}


void TypeEvaluationPass::Visit(SizeofExpression *sizeofExpression)
{
	ParseNodeTraverser::Visit(sizeofExpression);
	sizeofExpression->SetTypeDescriptor(TypeDescriptor::GetUIntType());
}


void TypeEvaluationPass::Visit(ConstantLiteralExpression *constantExpression)
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
		case Token::CONST_LONG:
			typeDescriptor = TypeDescriptor::GetLongType();
			break;
		case Token::CONST_ULONG:
			typeDescriptor = TypeDescriptor::GetULongType();
			break;
		case Token::CONST_FLOAT:
			typeDescriptor = TypeDescriptor::GetFloatType();
			break;
		case Token::CONST_DOUBLE:
			typeDescriptor = TypeDescriptor::GetDoubleType();
			break;
		case Token::CONST_STRING:
			typeDescriptor = TypeDescriptor::GetStringType();
			break;
		case Token::CONST_NULL:
			typeDescriptor = TypeDescriptor::GetNullType();
			break;
		default:
			// The parser is not supposed to allow this to happen.
			mErrorBuffer.PushError(CompilerError::INTERNAL_ERROR);
			break;
	}

	constantExpression->SetTypeDescriptor(typeDescriptor);
}


void TypeEvaluationPass::Visit(IdentifierExpression *identifierExpression)
{
	const QualifiedIdentifier *identifier = identifierExpression->GetIdentifier();
	Symbol *symbol = GetSymbol(identifier);

	if (symbol == nullptr)
	{
		mErrorBuffer.PushError(CompilerError::SYMBOL_IS_NOT_DEFINED, identifier->GetContextToken(), identifier);
	}
	else
	{
		identifierExpression->SetDefinition(symbol);
		const TypeAndValue *symbolTav = symbol->GetTypeAndValue();

		if (symbolTav == nullptr)
		{
			mErrorBuffer.PushError(CompilerError::INVALID_SYMBOL_IN_EXPRESSION, identifier->GetContextToken(), identifier);
		}
		else if (symbolTav->IsTypeDefined())
		{
			TypeDescriptor typeDescriptor = *symbolTav->GetTypeDescriptor();

			// Verify if the symbol was reached by implicitly dereferencing a const 'this' pointer.
			if ((symbol->GetParentSymbol() == mStruct.GetTop()) &&
			    (mFunction.GetTop() != nullptr) &&
			    (mFunction.GetTop()->GetPrototype()->IsConst()))
			{
				const NamedInitializer *namedInitializer = CastNode<NamedInitializer>(symbol);
				if (namedInitializer != nullptr)
				{
					typeDescriptor.SetConst();
				}
				else
				{
					const FunctionDefinition *functionDefinition = CastNode<FunctionDefinition>(symbol);
					if ((functionDefinition != nullptr) && !functionDefinition->GetPrototype()->IsConst())
					{
						mErrorBuffer.PushError(
							CompilerError::NON_CONST_MEMBER_FUNCTION_REQUEST,
							identifier->GetContextToken(),
							functionDefinition->GetPrototype(),
							mStruct.GetTop()->GetConstThisTypeDescriptor());
					}
				}
			}
			identifierExpression->SetTypeDescriptor(typeDescriptor);
		}
	}
}


void TypeEvaluationPass::Visit(ThisExpression *thisExpression)
{
	const StructDeclaration *structDeclaration = mStruct.GetTop();
	if (structDeclaration != nullptr)
	{
		thisExpression->SetTypeDescriptor(*mFunction.GetTop()->GetThisTypeDescriptor());
	}
	else
	{
		mErrorBuffer.PushError(CompilerError::THIS_IN_NON_MEMBER_FUNCTION, thisExpression->GetContextToken());
	}
}


bool TypeEvaluationPass::AssertBooleanExpression(const Expression *expression, CompilerError::Type errorType) const
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


bool TypeEvaluationPass::AssertMost32IntegerExpression(
	const Expression *expression,
	CompilerError::Type errorType,
	const void *arg) const
{
	const TypeAndValue &tav = expression->GetTypeAndValue();
	if (tav.IsTypeDefined())
	{
		const TypeDescriptor *typeDescriptor = tav.GetTypeDescriptor();
		if (!typeDescriptor->IsMost32IntegerType())
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
		mErrorBuffer.PushError(CompilerError::INVALID_OPERATOR_IN_CONST_EXPRESSION, op);
		return false;
	}
	return true;
}


bool TypeEvaluationPass::AssertBooleanOperand(const TypeDescriptor *typeDescriptor, const Token *op)
{
	if (!typeDescriptor->IsBooleanType())
	{
		mErrorBuffer.PushError(CompilerError::INVALID_TYPE_FOR_OPERATOR, op, typeDescriptor);
		return false;		
	}
	return true;
}


bool TypeEvaluationPass::AssertIntegerOperand(const TypeDescriptor *typeDescriptor, const Token *op)
{
	if (!typeDescriptor->IsIntegerType())
	{
		mErrorBuffer.PushError(CompilerError::INVALID_TYPE_FOR_OPERATOR, op, typeDescriptor);
		return false;
	}
	return true;
}


bool TypeEvaluationPass::AssertNumericOperand(const TypeDescriptor *typeDescriptor, const Token *op)
{
	if (!typeDescriptor->IsNumericType())
	{
		mErrorBuffer.PushError(CompilerError::INVALID_TYPE_FOR_OPERATOR, op, typeDescriptor);
		return false;
	}
	return true;
}


bool TypeEvaluationPass::AssertPointerOperand(const TypeDescriptor *typeDescriptor, const Token *op)
{
	if (!typeDescriptor->IsPointerType())
	{
		mErrorBuffer.PushError(CompilerError::INVALID_TYPE_FOR_POINTER_OPERATOR, op, typeDescriptor);
		return false;
	}
	return true;
}


bool TypeEvaluationPass::AssertAddressableType(const TypeDescriptor *typeDescriptor, const Token *op)
{
	if (!typeDescriptor->IsAddressable())
	{
		mErrorBuffer.PushError(CompilerError::NON_LVALUE_TYPE, op, typeDescriptor);
		return false;
	}
	return true;
}


bool TypeEvaluationPass::AssertAssignableType(const TypeDescriptor *typeDescriptor, const Token *op)
{
	if (!typeDescriptor->IsLValue())
	{
		mErrorBuffer.PushError(CompilerError::NON_LVALUE_ASSIGNMENT, op);
		return false;
	}
	else if (!typeDescriptor->IsAssignable())
	{
		mErrorBuffer.PushError(CompilerError::UNASSIGNABLE_TYPE, op, typeDescriptor);
		return false;
	}
	return true;
}


bool TypeEvaluationPass::AssertConvertibleTypes(
	const TypeDescriptor *fromType,
	const TypeDescriptor *toType,
	const Token *context,
	CompilerError::Type errorType)
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
		mErrorBuffer.PushError(CompilerError::INVALID_COMPARISON, op, typeA, typeB);
		return false;
	}
	return true;
}


void TypeEvaluationPass::ValidateInitializer(Initializer *initializer, const TypeDescriptor *typeDescriptor)
{
	initializer->SetTypeDescriptor(*typeDescriptor);
	const TypeDescriptor *descriptor = initializer->GetTypeDescriptor();
	const Expression *expression = initializer->GetExpression();
	Initializer *initializerList = initializer->GetInitializerList();

	if (descriptor->IsArrayType())
	{
		const TypeDescriptor elementDescriptor = descriptor->GetDereferencedType();
		if (initializerList != nullptr)
		{
			// TODO: Assert if too many initializers. Should be done after ValueEvaluationPass.
			while (initializerList != nullptr)
			{
				ValidateInitializer(initializerList, &elementDescriptor);
				initializerList = NextNode(initializerList);
			}
		}
		else if (expression != nullptr)
		{
			const ConstantLiteralExpression *constantExpression = CastNode<ConstantLiteralExpression>(expression);
			const bool isStringInitializer =
				elementDescriptor.IsCharType() &&
				(constantExpression != nullptr) &&
				constantExpression->GetTypeDescriptor()->IsStringType();

			if (!isStringInitializer)
			{
				mErrorBuffer.PushError(
					CompilerError::MISSING_BRACES_IN_INITIALIZER,
					initializer->GetContextToken(),
					descriptor);
			}
		}
	}
	else if (expression != nullptr)
	{
		const TypeAndValue &tav = expression->GetTypeAndValue();
		if (tav.IsTypeDefined())
		{
			AssertConvertibleTypes(
				expression->GetTypeAndValue().GetTypeDescriptor(),
				descriptor,
				expression->GetContextToken(),
				CompilerError::INVALID_TYPE_CONVERSION);
		}
	}
	else if (descriptor->IsStructType())
	{
		const TypeSpecifier *structSpecifier = descriptor->GetTypeSpecifier();
		const StructDeclaration *structDeclaration = CastNode<StructDeclaration>(structSpecifier->GetDefinition());
		const DeclarativeStatement *memberDeclarationList = structDeclaration->GetMemberVariableList();

		if (structDeclaration->IsNative())
		{
			mErrorBuffer.PushError(
				CompilerError::CANNOT_INITIALIZE_NATIVE_TYPE_WITH_INITIALIZER_LIST,
				initializer->GetContextToken(),
				descriptor);
		}
		else
		{
			while ((memberDeclarationList != nullptr) && (initializerList != nullptr))
			{
				const TypeDescriptor *memberDescriptor = memberDeclarationList->GetTypeDescriptor();
				const NamedInitializer *nameList = memberDeclarationList->GetNamedInitializerList();
				while ((nameList != nullptr) && (initializerList != nullptr))
				{
					ValidateInitializer(initializerList, memberDescriptor);
					nameList = NextNode(nameList);
					initializerList = NextNode(initializerList);
				}
				memberDeclarationList = NextNode(memberDeclarationList);
			}
			if (initializerList != nullptr)
			{
				mErrorBuffer.PushError(
					CompilerError::TOO_MANY_INITIALIZERS,
					initializerList->GetContextToken(),
					descriptor);
			}
		}
	}
	else
	{
		mErrorBuffer.PushError(
			CompilerError::BRACES_AROUND_SCALAR_INITIALIZER,
			initializer->GetContextToken(),
			descriptor);
	}
}


void TypeEvaluationPass::RecursiveStructAnalyzer::Analyze(const StructDeclaration *structDeclaration)
{
	mTopLevelStruct = structDeclaration;
	StructStack::Element structElement(mStruct, structDeclaration);
	ParseNodeTraverser::Visit(structDeclaration);
	mTopLevelStruct = nullptr;
}


void TypeEvaluationPass::RecursiveStructAnalyzer::Visit(const StructDeclaration *structDeclaration)
{
	if (structDeclaration == mTopLevelStruct)
	{
		mErrorBuffer.PushError(CompilerError::RECURSIVE_STRUCT, structDeclaration->GetName());
	}

	if (!mStruct.Contains(structDeclaration))
	{
		StructStack::Element structElement(mStruct, structDeclaration);
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
	if (typeSpecifier->GetDefinition() != nullptr)
	{
		Traverse(CastNode<StructDeclaration>(typeSpecifier->GetDefinition()));
	}
}

}
