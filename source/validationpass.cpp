#include "bond/compiler/compilererror.h"
#include "bond/compiler/parsenodes.h"
#include "bond/compiler/parsenodeutil.h"
#include "bond/systems/math.h"
#include "private/validationpass.h"

namespace Bond
{

struct SwitchLabelComparator
{
	bool operator()(const ResolvedSwitchLabel &a, const ResolvedSwitchLabel &b) const;
};


bool SwitchLabelComparator::operator()(const ResolvedSwitchLabel &a, const ResolvedSwitchLabel &b) const
{
	if (a.IsDefault())
	{
		return !b.IsDefault();
	}
	return a.GetMatch() < b.GetMatch();
}


void ValidationPass::Analyze(TranslationUnit *translationUnitList)
{
	BoolStack::Element hasDefaultLabelElement(mHasDefaultLabel, false);
	BoolStack::Element endsWithJumpElement(mEndsWithJump, false);
	BoolStack::Element hasReturnElement(mHasReturn, false);
	BoolStack::Element isInLoopElement(mIsInLoop, false);
	BoolStack::Element isInSwitchElement(mIsInSwitch, false);
	SizeStack::Element nextJumpTargetIdElement(mNextJumpTargetId, 0);
	SizeStack::Element switchJumpTargetIdElement(mSwitchJumpTargetId, 0);
	TypeStack::Element returnTypeElement(mReturnType, NULL);
	FunctionStack::Element functionElement(mFunction, NULL);
	SwitchLabelStack::Element switchLabelListElement(mSwitchLabelList, NULL);
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
	SwitchLabelStack::Element switchLabelListElement(mSwitchLabelList, NULL);
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
	switchStatement->SetResolvedLabelList(switchLabelListElement.GetValue());

	const ResolvedSwitchLabel *currLabel = switchLabelListElement.GetValue();
	bu32_t numMatches = 0;
	bi32_t minMatch = BOND_INT_MAX;
	bi32_t maxMatch = BOND_INT_MIN;
	while (currLabel != NULL)
	{
		if (!currLabel->IsDefault())
		{
			++numMatches;
			minMatch = Min(minMatch, currLabel->GetMatch());
			maxMatch = Max(maxMatch, currLabel->GetMatch());
		}

		const ResolvedSwitchLabel *prevLabel = currLabel;
		currLabel = NextNode(currLabel);

		if ((currLabel != NULL) && (*prevLabel == *currLabel))
		{
			if (currLabel->IsDefault())
			{
				mErrorBuffer.PushError(CompilerError::DUPLICATE_DEFAULT_IN_SWITCH, switchStatement->GetContextToken());
			}
			else
			{
				mErrorBuffer.PushErrorInt(CompilerError::DUPLICATE_CASE_IN_SWITCH, switchStatement->GetContextToken(), currLabel->GetMatch());
			}

			while ((currLabel != NULL) && (*prevLabel == *currLabel))
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

	mSwitchLabelList.SetTop(Insert(mSwitchLabelList.GetTop(), &resolvedLabel, SwitchLabelComparator()));
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
		const TypeDescriptor *returnType = jumpStatement->GetRhs()->GetTypeDescriptor();
		if (!AreConvertibleTypes(returnType, mReturnType.GetTop()))
		{
			mErrorBuffer.PushError(
				CompilerError::INVALID_RETURN_TYPE_CONVERSION,
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
		mErrorBuffer.PushError(CompilerError::UNREACHABLE_CODE, node->GetContextToken());
	}
}


size_t ValidationPass::GetJumpTargetId()
{
	const size_t targetId = mNextJumpTargetId.GetTop();
	mNextJumpTargetId.SetTop(targetId + 1);
	return targetId;
}

}
