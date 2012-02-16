#include "bond/math.h"
#include "bond/parseerror.h"
#include "bond/parsenodes.h"
#include "bond/parsenodeutil.h"
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
	IntStack::Element variableOffsetElement(mVariableOffset, 0);
	TypeStack::Element returnTypeElement(mReturnType, NULL);
	FunctionStack::Element functionElement(mFunction, NULL);
	SemanticAnalysisPass::Analyze(translationUnitList);
}


void ValidationPass::Visit(FunctionDefinition *functionDefinition)
{
	const TypeDescriptor *returnType = functionDefinition->GetPrototype()->GetReturnType();
	BoolStack::Element endsWithJumpElement(mEndsWithJump, false);
	BoolStack::Element hasReturnElement(mHasReturn, false);
	IntStack::Element variableOffsetElement(mVariableOffset, 0);
	TypeStack::Element returnTypeElement(mReturnType, returnType);
	FunctionStack::Element functionElement(mFunction, functionDefinition);
	SemanticAnalysisPass::Visit(functionDefinition);

	if (!returnType->IsVoidType() && !hasReturnElement && !functionDefinition->IsNative())
	{
		mErrorBuffer.PushError(ParseError::NOT_ALL_PATHS_RETURN_A_VALUE, functionDefinition->GetName());
	}
}


void ValidationPass::Visit(FunctionPrototype *functionPrototype)
{
	const bi32_t variableOffset = (mFunction.GetTop()->GetThisTypeDescriptor() != NULL) ? -static_cast<bi32_t>(mPointerSize) : 0;
	IntStack::Element variableOffsetElement(mVariableOffset, variableOffset);
	ParseNodeTraverser::Visit(functionPrototype);
}


void ValidationPass::Visit(Parameter *parameter)
{
	const TypeDescriptor *typeDescriptor = parameter->GetTypeDescriptor();
	const bi32_t variableAlignment = static_cast<bi32_t>(typeDescriptor->GetAlignment());
	const bi32_t variableSize = static_cast<bi32_t>(typeDescriptor->GetSize(mPointerSize));
	const bi32_t variableOffset = AlignDown(mVariableOffset.GetTop() - variableSize, variableAlignment);
	parameter->SetOffset(variableOffset);
	mVariableOffset.SetTop(variableOffset);
	ParseNodeTraverser::Visit(parameter);
}


void ValidationPass::Visit(NamedInitializer *namedInitializer)
{
	if (namedInitializer->GetScope() == SCOPE_LOCAL)
	{
		const TypeDescriptor *typeDescriptor = namedInitializer->GetTypeAndValue()->GetTypeDescriptor();
		const bi32_t variableAlignment = static_cast<bi32_t>(typeDescriptor->GetAlignment());
		const bi32_t variableSize = static_cast<bi32_t>(typeDescriptor->GetSize(mPointerSize));
		const bi32_t variableOffset = AlignUp(mVariableOffset.GetTop(), variableAlignment);
		namedInitializer->SetOffset(variableOffset);
		mVariableOffset.SetTop(variableOffset + variableSize);
	}
	ParseNodeTraverser::Visit(namedInitializer);
}


void ValidationPass::Visit(CompoundStatement *compoundStatement)
{
	IntStack::Element variableOffsetElement(mVariableOffset, mVariableOffset.GetTop());
	SemanticAnalysisPass::Visit(compoundStatement);
}


void ValidationPass::Visit(IfStatement *ifStatement)
{
	AssertReachableCode(ifStatement);
	mEndsWithJump.SetTop(false);
	Traverse(ifStatement->GetCondition());

	bool hasReturn = ifStatement->GetElseStatement() != NULL;
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
	mEndsWithJump.SetTop(false);
	Traverse(switchStatement->GetControl());

	bool hasReturn = true;
	SwitchSection *sectionList = switchStatement->GetSectionList();
	if (sectionList != NULL)
	{
		BoolStack::Element hasDefaultLabelElement(mHasDefaultLabel, false);
		while (sectionList != NULL)
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
	IntStack::Element variableOffsetElement(mVariableOffset, mVariableOffset.GetTop());
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
		const TypeDescriptor *returnType = jumpStatement->GetRhs()->GetTypeDescriptor();
		if (!AreConvertibleTypes(returnType, mReturnType.GetTop()))
		{
			mErrorBuffer.PushError(
				ParseError::INVALID_RETURN_TYPE_CONVERSION,
				jumpStatement->GetContextToken(),
				returnType,
				mReturnType.GetTop());
		}
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
	if (expressionStatement->GetExpression() != NULL)
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
