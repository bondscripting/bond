#include "bond/compiler/compilererror.h"
#include "bond/compiler/parsenodes.h"
#include "bond/compiler/parsenodeutil.h"
#include "bond/systems/math.h"
#include "private/validationpass.h"

namespace Bond
{

void ValidationPass::Analyze(TranslationUnit *translationUnitList)
{
	BoolStack::Element hasDefaultLabelElement(mHasDefaultLabel, false);
	BoolStack::Element endsWithJumpElement(mEndsWithJump, false);
	BoolStack::Element hasReturnElement(mHasReturn, false);
	BoolStack::Element isInLoopElement(mIsInLoop, false);
	BoolStack::Element isInSwitchElement(mIsInSwitch, false);
	SizeStack::Element nextJumpTargetIdElement(mNextJumpTargetId, 0);
	SizeStack::Element switchJumpTargetIdElement(mSwitchJumpTargetId, 0);
	TypeStack::Element returnTypeElement(mReturnType, nullptr);
	FunctionStack::Element functionElement(mFunction, nullptr);
	SwitchLabelStack::Element switchLabelListElement(mSwitchLabelList, nullptr);
	SemanticAnalysisPass::Analyze(translationUnitList);
}


void ValidationPass::Visit(FunctionDefinition *functionDefinition)
{
	const TypeDescriptor *returnType = functionDefinition->GetPrototype()->GetReturnType();
	BoolStack::Element endsWithJumpElement(mEndsWithJump, false);
	BoolStack::Element hasReturnElement(mHasReturn, false);
	SizeStack::Element nextJumpTargetIdElement(mNextJumpTargetId, 0);
	SizeStack::Element switchJumpTargetIdElement(mSwitchJumpTargetId, 0);
	TypeStack::Element returnTypeElement(mReturnType, returnType);
	FunctionStack::Element functionElement(mFunction, functionDefinition);
	SemanticAnalysisPass::Visit(functionDefinition);

	functionDefinition->SetNumReservedJumpTargetIds(nextJumpTargetIdElement.GetValue());

	if (!returnType->IsVoidType() && !hasReturnElement && !functionDefinition->IsNative())
	{
		mErrorBuffer.PushError(CompilerError::NOT_ALL_PATHS_RETURN_A_VALUE, functionDefinition->GetName());
	}
}


void ValidationPass::Visit(FunctionPrototype *functionPrototype)
{
	const TypeDescriptor *typeDescriptor = functionPrototype->GetReturnType();
	if (!typeDescriptor->IsInstantiable())
	{
		mErrorBuffer.PushError(CompilerError::RETURN_OF_UNDEFINED_SIZE, functionPrototype->GetName(), typeDescriptor);
	}
	ParseNodeTraverser::Visit(functionPrototype);
}


void ValidationPass::Visit(Parameter *parameter)
{
	const TypeDescriptor *typeDescriptor = parameter->GetTypeDescriptor();
	if (!typeDescriptor->IsInstantiable())
	{
		mErrorBuffer.PushError(CompilerError::DECLARATION_OF_UNDEFINED_SIZE, parameter->GetName(), typeDescriptor);
	}
}


void ValidationPass::Visit(NamedInitializer *namedInitializer)
{
	TypeAndValue &tav = *namedInitializer->GetTypeAndValue();
	if ((namedInitializer->GetScope() == SCOPE_LOCAL) && tav.IsValueDefined() &&
	    tav.GetTypeDescriptor().IsPrimitiveType())
	{
		namedInitializer->SetElidable(true);
	}

	Initializer *initializer = namedInitializer->GetInitializer();

	if (initializer != nullptr)
	{
		const TypeDescriptor &typeDescriptor = tav.GetTypeDescriptor();
		ValidateInitializer(initializer, typeDescriptor);
	}

	ParseNodeTraverser::Visit(namedInitializer);
}


void ValidationPass::Visit(IfStatement *ifStatement)
{
	AssertReachableCode(ifStatement);
	mEndsWithJump.SetTop(false);
	Traverse(ifStatement->GetCondition());

	bool hasReturn = ifStatement->GetElseStatement() != nullptr;
	bool endsWithJump = hasReturn;
	{
		BoolStack::Element hasReturnElement(mHasReturn, false);
		BoolStack::Element endsWithJumpElement(mEndsWithJump, false);
		Traverse(ifStatement->GetThenStatement());
		hasReturn = hasReturn && hasReturnElement;
		endsWithJump = endsWithJump && endsWithJumpElement;
	}

	{
		BoolStack::Element hasReturnElement(mHasReturn, false);
		BoolStack::Element endsWithJumpElement(mEndsWithJump, false);
		Traverse(ifStatement->GetElseStatement());
		hasReturn = hasReturn && hasReturnElement;
		endsWithJump = endsWithJump && endsWithJumpElement;
	}
	mHasReturn.SetTop(mHasReturn.GetTop() || hasReturn);
	mEndsWithJump.SetTop(endsWithJump || hasReturn);
}


void ValidationPass::Visit(SwitchStatement *switchStatement)
{
	AssertReachableCode(switchStatement);
	BoolStack::Element isInSwitchElement(mIsInSwitch, true);
	SwitchLabelStack::Element switchLabelListElement(mSwitchLabelList, nullptr);
	mEndsWithJump.SetTop(false);
	Traverse(switchStatement->GetControl());

	bool hasReturn = true;
	SwitchSection *sectionList = switchStatement->GetSectionList();
	if (sectionList != nullptr)
	{
		BoolStack::Element hasDefaultLabelElement(mHasDefaultLabel, false);
		while (sectionList != nullptr)
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
	switchStatement->SetResolvedLabelList(switchLabelListElement.GetValue());

	const ResolvedSwitchLabel *currLabel = switchLabelListElement.GetValue();
	uint32_t numMatches = 0;
	int32_t minMatch = BOND_INT_MAX;
	int32_t maxMatch = BOND_INT_MIN;
	while (currLabel != nullptr)
	{
		if (!currLabel->IsDefault())
		{
			++numMatches;
			minMatch = Min(minMatch, currLabel->GetMatch());
			maxMatch = Max(maxMatch, currLabel->GetMatch());
		}

		const ResolvedSwitchLabel *prevLabel = currLabel;
		currLabel = NextNode(currLabel);

		if ((currLabel != nullptr) && (*prevLabel == *currLabel))
		{
			if (currLabel->IsDefault())
			{
				mErrorBuffer.PushError(CompilerError::DUPLICATE_DEFAULT_IN_SWITCH, switchStatement->GetContextToken());
			}
			else
			{
				mErrorBuffer.PushErrorInt(CompilerError::DUPLICATE_CASE_IN_SWITCH, switchStatement->GetContextToken(), currLabel->GetMatch());
			}

			while ((currLabel != nullptr) && (*prevLabel == *currLabel))
			{
				prevLabel = currLabel;
				currLabel = NextNode(currLabel);
			}
		}
	}

	switchStatement->SetNumMatches(numMatches);
	switchStatement->SetMinMatch(minMatch);
	switchStatement->SetMaxMatch(maxMatch);
}


void ValidationPass::Visit(SwitchSection *switchSection)
{
	BoolStack::Element endsWithJumpElement(mEndsWithJump, false);

	const size_t jumpTargetId = GetJumpTargetId();
	switchSection->SetJumpTargetId(jumpTargetId);
	SizeStack::Element switchJumpTargetIdElement(mSwitchJumpTargetId, jumpTargetId);

	SemanticAnalysisPass::Visit(switchSection);

	if (!endsWithJumpElement)
	{
		mErrorBuffer.PushError(CompilerError::UNTERMINATED_SWITCH_SECTION, switchSection->GetLabelList()->GetContextToken());
	}
}


void ValidationPass::Visit(SwitchLabel *switchLabel)
{
	ParseNodeTraverser::Visit(switchLabel);

	ResolvedSwitchLabel &resolvedLabel = switchLabel->GetResolvedLabel();
	resolvedLabel.SetJumpTargetId(mSwitchJumpTargetId.GetTop());

	if (switchLabel->IsDefaultLabel())
	{
		resolvedLabel.SetIsDefault(true);
		mHasDefaultLabel.SetTop(true);
	}
	else
	{
		resolvedLabel.SetMatch(switchLabel->GetExpression()->GetTypeAndValue().GetIntValue());
	}

	auto switchLabelComparator = [](const ResolvedSwitchLabel &a, const ResolvedSwitchLabel &b)
		{
			if (a.IsDefault())
			{
				return !b.IsDefault();
			}
			return a.GetMatch() < b.GetMatch();
		};

	mSwitchLabelList.SetTop(InsertNode(mSwitchLabelList.GetTop(), &resolvedLabel, switchLabelComparator));
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
			mErrorBuffer.PushError(CompilerError::INVALID_BREAK, jumpStatement->GetContextToken());
			endsWithJump = false;
		}
	}
	else if (jumpStatement->IsContinue())
	{
		if (!mIsInLoop.GetTop())
		{
			mErrorBuffer.PushError(CompilerError::INVALID_CONTINUE, jumpStatement->GetContextToken());
			endsWithJump = false;
		}
	}
	else if (jumpStatement->IsReturn())
	{
		mHasReturn.SetTop(true);
		AssertAssignableTypes(
			jumpStatement->GetRhs()->GetTypeAndValue(),
			*mReturnType.GetTop(),
			jumpStatement->GetContextToken(),
			CompilerError::INVALID_RETURN_TYPE_CONVERSION,
			true);
	}
	mEndsWithJump.SetTop(mEndsWithJump.GetTop() || endsWithJump);
}


void ValidationPass::Visit(DeclarativeStatement *declarativeStatement)
{
	const TypeDescriptor *typeDescriptor = declarativeStatement->GetTypeDescriptor();
	if (!typeDescriptor->IsInstantiable())
	{
		mErrorBuffer.PushError(
			CompilerError::DECLARATION_OF_UNDEFINED_SIZE,
			declarativeStatement->GetNamedInitializerList()->GetName(),
			typeDescriptor);
	}
	AssertReachableCode(declarativeStatement);
	mEndsWithJump.SetTop(false);
	ParseNodeTraverser::Visit(declarativeStatement);
}


void ValidationPass::Visit(ExpressionStatement *expressionStatement)
{
	if (expressionStatement->GetExpression() != nullptr)
	{
		AssertReachableCode(expressionStatement);
	}
	mEndsWithJump.SetTop(false);
	ParseNodeTraverser::Visit(expressionStatement);
}


void ValidationPass::Visit(BinaryExpression *binaryExpression)
{
	const Token *op = binaryExpression->GetOperator();
	const TypeDescriptor &lhDescriptor = binaryExpression->GetLhs()->GetTypeDescriptor();
	switch (op->GetTokenType())
	{
		case Token::ASSIGN:
		{
			const TypeDescriptor &typeDescriptor = binaryExpression->GetTypeDescriptor();
			if (!typeDescriptor.IsInstantiable())
			{
				mErrorBuffer.PushError(
					CompilerError::ASSIGNMENT_OF_UNDEFINED_SIZE,
					binaryExpression->GetContextToken(),
					&typeDescriptor);
			}
			const TypeAndValue &rhTav = binaryExpression->GetRhs()->GetTypeAndValue();
			AssertAssignableTypes(rhTav, lhDescriptor, op, CompilerError::INVALID_TYPE_ASSIGNMENT);
		}
		break;
		case Token::ASSIGN_PLUS:
		case Token::ASSIGN_MINUS:
		case Token::OP_PLUS:
		case Token::OP_MINUS:
		{
			if (lhDescriptor.IsPointerType())
			{
				const TypeDescriptor parentType = lhDescriptor.GetDereferencedType();
				if (!parentType.IsInstantiable())
				{
					mErrorBuffer.PushError(
						CompilerError::POINTER_ARITHMETIC_OF_UNDEFINED_SIZE,
						binaryExpression->GetContextToken(),
						&lhDescriptor);
				}
			}
		}
		break;
		default:
			break;
	}
	ParseNodeTraverser::Visit(binaryExpression);
}


void ValidationPass::Visit(UnaryExpression *unaryExpression)
{
	const Token *op = unaryExpression->GetOperator();
	if (op->GetTokenType() == Token::OP_AMP)
	{
		IdentifierExpression *identifierExpression = CastNode<IdentifierExpression>(unaryExpression->GetRhs());
		if (identifierExpression != nullptr)
		{
			NamedInitializer *namedInitializer = CastNode<NamedInitializer>(identifierExpression->GetDefinition());
			if (namedInitializer != nullptr)
			{
				namedInitializer->SetElidable(false);
			}
		}
	}
	ParseNodeTraverser::Visit(unaryExpression);
}


void ValidationPass::Visit(ArraySubscriptExpression *arraySubscriptExpression)
{
	const TypeDescriptor &typeDescriptor = arraySubscriptExpression->GetTypeDescriptor();
	if (!typeDescriptor.IsInstantiable())
	{
		mErrorBuffer.PushError(
			CompilerError::POINTER_ARITHMETIC_OF_UNDEFINED_SIZE,
			arraySubscriptExpression->GetContextToken(),
			&arraySubscriptExpression->GetLhs()->GetTypeDescriptor());
	}
	ParseNodeTraverser::Visit(arraySubscriptExpression);
}


void ValidationPass::Visit(FunctionCallExpression *functionCallExpression)
{
	ParseNodeTraverser::Visit(functionCallExpression);

	const TypeDescriptor &lhDescriptor = functionCallExpression->GetLhs()->GetTypeDescriptor();
	const TypeSpecifier *lhSpecifier = lhDescriptor.GetTypeSpecifier();
	const FunctionDefinition *function = CastNode<FunctionDefinition>(lhSpecifier->GetDefinition());
	const FunctionPrototype *prototype = function->GetPrototype();
	const Parameter *paramList = prototype->GetParameterList();
	const Expression *argList = functionCallExpression->GetArgumentList();

	while ((paramList != nullptr) && (argList != nullptr))
	{
		const TypeDescriptor &paramDescriptor = *paramList->GetTypeDescriptor();
		const TypeAndValue &argTav = argList->GetTypeAndValue();
		AssertAssignableTypes(
			argTav,
			paramDescriptor,
			argList->GetContextToken(),
			CompilerError::INVALID_TYPE_ASSIGNMENT,
			true);
		paramList = NextNode(paramList);
		argList = NextNode(argList);
	}
}


void ValidationPass::ValidateInitializer(Initializer *initializer, const TypeDescriptor &typeDescriptor)
{
	const TypeDescriptor &descriptor = initializer->GetTypeDescriptor();
	const Expression *expression = initializer->GetExpression();
	Initializer *initializerList = initializer->GetInitializerList();

	if (descriptor.IsArrayType())
	{
		uint32_t numElements = typeDescriptor.GetLengthExpressionList()->GetTypeAndValue().GetUIntValue();
		const TypeDescriptor elementDescriptor = descriptor.GetDereferencedType();
		if (initializerList != nullptr)
		{
			while ((numElements > 0) && (initializerList != nullptr))
			{
				--numElements;
				ValidateInitializer(initializerList, elementDescriptor);
				initializerList = NextNode(initializerList);
			}

			if (initializerList != nullptr)
			{
				mErrorBuffer.PushError(
					CompilerError::TOO_MANY_INITIALIZERS,
					initializerList->GetContextToken(),
					&descriptor);
			}
		}
		else if (expression != nullptr)
		{
			const ConstantLiteralExpression *constantExpression = CastNode<ConstantLiteralExpression>(expression);
			const bool isStringInitializer =
				elementDescriptor.IsCharType() &&
				(constantExpression != nullptr) &&
				constantExpression->GetTypeDescriptor().IsStringType();

			if (isStringInitializer)
			{
				const uint32_t stringLength = uint32_t(constantExpression->GetValueToken()->GetStringLength() + 1);
				if (stringLength > numElements)
				{
					mErrorBuffer.PushError(
						CompilerError::TOO_MANY_INITIALIZERS,
						expression->GetContextToken(),
						&descriptor);
				}
			}
		}
	}
	else if (expression != nullptr)
	{
		AssertAssignableTypes(
			expression->GetTypeAndValue(),
			descriptor,
			expression->GetContextToken(),
			CompilerError::INVALID_TYPE_ASSIGNMENT,
			true);
	}
	else if (descriptor.IsStructType())
	{
		const TypeSpecifier *structSpecifier = descriptor.GetTypeSpecifier();
		const StructDeclaration *structDeclaration = CastNode<StructDeclaration>(structSpecifier->GetDefinition());
		const DeclarativeStatement *memberDeclarationList = structDeclaration->GetMemberVariableList();

		if (!structDeclaration->IsNative())
		{
			while ((memberDeclarationList != nullptr) && (initializerList != nullptr))
			{
				const TypeDescriptor *memberDescriptor = memberDeclarationList->GetTypeDescriptor();
				const NamedInitializer *nameList = memberDeclarationList->GetNamedInitializerList();
				while ((nameList != nullptr) && (initializerList != nullptr))
				{
					ValidateInitializer(initializerList, *memberDescriptor);
					nameList = NextNode(nameList);
					initializerList = NextNode(initializerList);
				}
				memberDeclarationList = NextNode(memberDeclarationList);
			}
		}
	}
}


void ValidationPass::AssertReachableCode(const ParseNode *node)
{
	if (mEndsWithJump.GetTop())
	{
		mErrorBuffer.PushError(CompilerError::UNREACHABLE_CODE, node->GetContextToken());
	}
}


size_t ValidationPass::GetJumpTargetId()
{
	const size_t targetId = mNextJumpTargetId.GetTop();
	mNextJumpTargetId.SetTop(targetId + 1);
	return targetId;
}


bool ValidationPass::AssertAssignableTypes(
	const TypeAndValue &fromTav,
	const TypeDescriptor &toType,
	const Token *context,
	CompilerError::Type errorType,
	bool ignoreConst)
{
	const TypeDescriptor &fromType = fromTav.GetTypeDescriptor();
	if (ignoreConst)
	{
		TypeDescriptor nonConstType = toType;
		nonConstType.ClearConst();
		nonConstType.SetUnaddressableLValue();
		if (!AreAssignableTypes(fromType, nonConstType))
		{
			mErrorBuffer.PushError(errorType, context, &fromType, &toType);
			return false;
		}
	}
	else
	{
		if (!AreAssignableTypes(fromType, toType))
		{
			mErrorBuffer.PushError(errorType, context, &fromType, &toType);
			return false;
		}
	}
	if (IsLossyConversion(fromTav, toType))
	{
		mErrorBuffer.PushError(CompilerError::LOSSY_CONVERSION, context, &fromType, &toType);
		return false;
	}
	return true;
}

}
