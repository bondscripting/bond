#include "bond/expressiontypeevaluator.h"
#include "bond/parseerror.h"
#include "bond/parsenodes.h"
#include "bond/typeandvalue.h"

namespace Bond
{

void ExpressionTypeEvaluator::Visit(ConditionalExpression *conditionalExpression)
{
	ParseNodeTraverser::Visit(conditionalExpression);
	TypeAndValue &tav = conditionalExpression->GetTypeAndValue();

	if (!tav.IsTypeDefined())
	{
		tav.SetTypeDescriptor(&CONST_BOOL_TYPE_DESCRIPTOR);
		mMadeChanges = true;
	}
}


void ExpressionTypeEvaluator::Visit(UnaryExpression *unaryExpression)
{
	TypeAndValue &tav = unaryExpression->GetTypeAndValue();

	if (!tav.IsTypeDefined())
	{
		ParseNodeTraverser::Visit(unaryExpression);
		const TypeAndValue &rhs = unaryExpression->GetRhs()->GetTypeAndValue();

		if (rhs.IsTypeDefined())
		{
			const Token *op = unaryExpression->GetOperator();
			const TypeDescriptor *descriptor = rhs.GetTypeDescriptor();

			switch (op->GetTokenType())
			{
				case Token::OP_PLUS:
				case Token::OP_MINUS:
				case Token::OP_MULT:
				case Token::OP_INC:
				case Token::OP_DEC:
					AssertNumericType(descriptor, op);
					break;

				case Token::OP_NOT:
					AssertBooleanType(descriptor, op);
					break;

				case Token::OP_BIT_AND:
				case Token::OP_BIT_NOT:
					AssertIntegerType(descriptor, op);
					break;

				default:
					break;
			}

			tav.SetTypeDescriptor(descriptor);
			tav.Resolve();
			mMadeChanges = true;
		}
	}
}


void ExpressionTypeEvaluator::Visit(PostfixExpression *postfixExpression)
{
	TypeAndValue &tav = postfixExpression->GetTypeAndValue();

	if (!tav.IsTypeDefined())
	{
		ParseNodeTraverser::Visit(postfixExpression);
		const TypeAndValue &lhs = postfixExpression->GetLhs()->GetTypeAndValue();

		if (lhs.IsTypeDefined())
		{
			const Token *op = postfixExpression->GetOperator();
			const TypeDescriptor *descriptor = lhs.GetTypeDescriptor();
			AssertNumericType(descriptor, op);
			tav.SetTypeDescriptor(descriptor);
			tav.Resolve();
			mMadeChanges = true;
		}
	}
}


void ExpressionTypeEvaluator::Visit(CastExpression *castExpression)
{
}


void ExpressionTypeEvaluator::Visit(SizeofExpression *sizeofExpression)
{
	ParseNodeTraverser::Visit(sizeofExpression);
	TypeAndValue &tav = sizeofExpression->GetTypeAndValue();

	if (!tav.IsTypeDefined())
	{
		tav.SetTypeDescriptor(&CONST_UINT_TYPE_DESCRIPTOR);
		mMadeChanges = true;
	}
}


void ExpressionTypeEvaluator::Visit(ConstantExpression *constantExpression)
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


void ExpressionTypeEvaluator::AssertBooleanType(const TypeDescriptor *descriptor, const Token *op)
{
	if (!descriptor->IsBooleanType())
	{
		mErrorBuffer.PushError(ParseError::INVALID_TYPE_FOR_OPERATOR, op, descriptor);
	}
}


void ExpressionTypeEvaluator::AssertIntegerType(const TypeDescriptor *descriptor, const Token *op)
{
	if (!descriptor->IsIntegerType())
	{
		mErrorBuffer.PushError(ParseError::INVALID_TYPE_FOR_OPERATOR, op, descriptor);
	}
}


void ExpressionTypeEvaluator::AssertNumericType(const TypeDescriptor *descriptor, const Token *op)
{
	if (!descriptor->IsNumericType())
	{
		mErrorBuffer.PushError(ParseError::INVALID_TYPE_FOR_OPERATOR, op, descriptor);
	}
}

}
