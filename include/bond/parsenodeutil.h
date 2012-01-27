#ifndef BOND_PARSENODEUTIL_H
#define BOND_PARSENODEUTIL_H

#include "bond/parsenodes.h"
#include "bond/parsenodevisitor.h"

namespace Bond
{

class Symbol;

bu32_t GetLength(const ListParseNode *list);


template <typename NodeType>
NodeType *NextNode(NodeType *list) { return static_cast<NodeType *>(list->GetNextNode()); }


template<typename T>
class CastVisitor: public ParseNodeVisitorAdapter
{
public:
	CastVisitor(): mNode(NULL) {}

	virtual void Visit(T *node) { mNode = node; }

	T *GetNode() { return mNode; }

private:
	T *mNode;
};


template<typename T>
T *CastNode(ParseNode *node)
{
	if (node != NULL)
	{
		CastVisitor<T> castVisitor;
		node->Accept(castVisitor);
		return castVisitor.GetNode();
	}
	return static_cast<T *>(NULL);
}


template<typename T>
const T *CastNode(const ParseNode *node)
{
	if (node != NULL)
	{
		CastVisitor<const T> castVisitor;
		node->Accept(castVisitor);
		return castVisitor.GetNode();
	}
	return static_cast<const T *>(NULL);
}


template <typename NodeType, typename Comparator>
NodeType *Insert(NodeType *head, NodeType *node)
{
	Comparator compare;
	if (head == NULL)
	{
		node->SetNextNode(NULL);
		return node;
	}

	if (compare(*node, *head))
	{
		node->SetNextNode(head);
		return node;
	}

	NodeType *current = head;
	NodeType *next = NextNode(head);
	while ((next != NULL) && !compare(*node, *next))
	{
		current = next;
		next = NextNode(next);
	}

	current->SetNextNode(node);
	node->SetNextNode(next);
	return head;
}


template <typename NodeType, typename Comparator>
NodeType *Sort(NodeType *list)
{
	NodeType *node = list;
	NodeType *head = NULL;
	while (node != NULL)
	{
		NodeType *current = node;
		node = NextNode(node);
		head = Insert<NodeType, Comparator>(head, current);
	}
	return head;
}


bool AreComparableTypes(const TypeDescriptor *typeA, const TypeDescriptor *typeB);
bool AreConvertibleTypes(const TypeDescriptor *fromType, const TypeDescriptor *toType);

TypeDescriptor PromoteType(const TypeDescriptor *type);
TypeDescriptor CombineOperandTypes(const TypeDescriptor *typeA, const TypeDescriptor *typeB);

Value CastValue(const Value &value, Token::TokenType sourceType, Token::TokenType destType);
Value CastValue(const TypeAndValue &value, const TypeDescriptor *type);


template<typename Operator>
Value NumericBinaryOperator(const TypeAndValue &lhs, const TypeAndValue &rhs, const TypeDescriptor *type, Operator op)
{
	const Value l = CastValue(lhs, type);
	const Value r = CastValue(rhs, type);
	Value resultValue;

	switch (type->GetPrimitiveType())
	{
		case Token::KEY_FLOAT:
			resultValue.mFloat = op(l.mFloat, r.mFloat);
			break;
		case Token::KEY_INT:
			resultValue.mInt = op(l.mInt, r.mInt);
			break;
		case Token::KEY_UINT:
			resultValue.mUInt = op(l.mUInt, r.mUInt);
			break;
		default:
			break;
	}

	return resultValue;
}


template<typename Operator>
Value IntegerBinaryOperator(const TypeAndValue &lhs, const TypeAndValue &rhs, const TypeDescriptor *type, Operator op)
{
	const Value l = CastValue(lhs, type);
	const Value r = CastValue(rhs, type);
	Value resultValue;

	switch (type->GetPrimitiveType())
	{
		case Token::KEY_INT:
			resultValue.mInt = op(l.mInt, r.mInt);
			break;
		case Token::KEY_UINT:
			resultValue.mUInt = op(l.mUInt, r.mUInt);
			break;
		default:
			break;
	}

	return resultValue;
}


template<typename Operator>
Value ComparisonBinaryOperator(const TypeAndValue &lhs, const TypeAndValue &rhs, Operator op)
{
	TypeDescriptor type = CombineOperandTypes(lhs.GetTypeDescriptor(), rhs.GetTypeDescriptor());
	const Value l = CastValue(lhs, &type);
	const Value r = CastValue(rhs, &type);
	Value resultValue;

	switch (type.GetPrimitiveType())
	{
		case Token::KEY_BOOL:
			resultValue.mBool = op(l.mBool, r.mBool);
			break;
		case Token::KEY_FLOAT:
			resultValue.mBool = op(l.mFloat, r.mFloat);
			break;
		case Token::KEY_INT:
			resultValue.mBool = op(l.mInt, r.mInt);
			break;
		case Token::KEY_UINT:
			resultValue.mBool = op(l.mUInt, r.mUInt);
			break;
		default:
			break;
	}

	return resultValue;
}


struct BinaryAddOperator { template<typename T> T operator()(T a, T b) { return a + b; } };
inline Value BinaryAdd(const TypeAndValue &lhs, const TypeAndValue &rhs, const TypeDescriptor *type)
{
	return NumericBinaryOperator(lhs, rhs, type, BinaryAddOperator());
}


struct BinarySubOperator { template<typename T> T operator()(T a, T b) { return a - b; } };
inline Value BinarySub(const TypeAndValue &lhs, const TypeAndValue &rhs, const TypeDescriptor *type)
{
	return NumericBinaryOperator(lhs, rhs, type, BinarySubOperator());
}


struct BinaryMultOperator { template<typename T> T operator()(T a, T b) { return a * b; } };
inline Value BinaryMult(const TypeAndValue &lhs, const TypeAndValue &rhs, const TypeDescriptor *type)
{
	return NumericBinaryOperator(lhs, rhs, type, BinaryMultOperator());
}


struct BinaryDivOperator { template<typename T> T operator()(T a, T b) { return a / b; } };
inline Value BinaryDiv(const TypeAndValue &lhs, const TypeAndValue &rhs, const TypeDescriptor *type)
{
	return NumericBinaryOperator(lhs, rhs, type, BinaryDivOperator());
}


struct BinaryModOperator { template<typename T> T operator()(T a, T b) { return a % b; } };
inline Value BinaryMod(const TypeAndValue &lhs, const TypeAndValue &rhs, const TypeDescriptor *type)
{
	return IntegerBinaryOperator(lhs, rhs, type, BinaryModOperator());
}


struct BinaryLeftOperator { template<typename T> T operator()(T a, T b) { return a >> b; } };
inline Value BinaryLeft(const TypeAndValue &lhs, const TypeAndValue &rhs, const TypeDescriptor *type)
{
	return IntegerBinaryOperator(lhs, rhs, type, BinaryLeftOperator());
}


struct BinaryRightOperator { template<typename T> T operator()(T a, T b) { return a >> b; } };
inline Value BinaryRight(const TypeAndValue &lhs, const TypeAndValue &rhs, const TypeDescriptor *type)
{
	return IntegerBinaryOperator(lhs, rhs, type, BinaryRightOperator());
}


struct BinaryBitAndOperator { template<typename T> T operator()(T a, T b) { return a & b; } };
inline Value BinaryBitAnd(const TypeAndValue &lhs, const TypeAndValue &rhs, const TypeDescriptor *type)
{
	return IntegerBinaryOperator(lhs, rhs, type, BinaryBitAndOperator());
}


struct BinaryBitOrOperator { template<typename T> T operator()(T a, T b) { return a | b; } };
inline Value BinaryBitOr(const TypeAndValue &lhs, const TypeAndValue &rhs, const TypeDescriptor *type)
{
	return IntegerBinaryOperator(lhs, rhs, type, BinaryBitOrOperator());
}


struct BinaryBitXOrOperator { template<typename T> T operator()(T a, T b) { return a ^ b; } };
inline Value BinaryBitXOr(const TypeAndValue &lhs, const TypeAndValue &rhs, const TypeDescriptor *type)
{
	return IntegerBinaryOperator(lhs, rhs, type, BinaryBitXOrOperator());
}


struct BinaryLTOperator { template<typename T> bool operator()(T a, T b) { return a < b; } };
inline Value BinaryLT(const TypeAndValue &lhs, const TypeAndValue &rhs)
{
	return ComparisonBinaryOperator(lhs, rhs, BinaryLTOperator());
}


struct BinaryLTEOperator { template<typename T> bool operator()(T a, T b) { return a <= b; } };
inline Value BinaryLTE(const TypeAndValue &lhs, const TypeAndValue &rhs)
{
	return ComparisonBinaryOperator(lhs, rhs, BinaryLTEOperator());
}


struct BinaryGTOperator { template<typename T> bool operator()(T a, T b) { return a > b; } };
inline Value BinaryGT(const TypeAndValue &lhs, const TypeAndValue &rhs)
{
	return ComparisonBinaryOperator(lhs, rhs, BinaryGTOperator());
}


struct BinaryGTEOperator { template<typename T> bool operator()(T a, T b) { return a >= b; } };
inline Value BinaryGTE(const TypeAndValue &lhs, const TypeAndValue &rhs)
{
	return ComparisonBinaryOperator(lhs, rhs, BinaryGTEOperator());
}


struct BinaryEqualOperator { template<typename T> bool operator()(T a, T b) { return a == b; } };
inline Value BinaryEqual(const TypeAndValue &lhs, const TypeAndValue &rhs)
{
	return ComparisonBinaryOperator(lhs, rhs, BinaryEqualOperator());
}


struct BinaryNotEqualOperator { template<typename T> bool operator()(T a, T b) { return a != b; } };
inline Value BinaryNotEqual(const TypeAndValue &lhs, const TypeAndValue &rhs)
{
	return ComparisonBinaryOperator(lhs, rhs, BinaryNotEqualOperator());
}


Value UnaryMinus(const TypeAndValue &value);

Value UnaryBitNot(const TypeAndValue &value);

}

#endif
