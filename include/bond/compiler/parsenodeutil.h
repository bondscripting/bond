#ifndef BOND_COMPILER_PARSENODEUTIL_H
#define BOND_COMPILER_PARSENODEUTIL_H

#include "bond/conf.h"
#include "bond/compiler/parsenodes.h"
#include "bond/compiler/parsenodevisitor.h"

namespace Bond
{

/// \addtogroup compiler
/// @{

/// \brief Counts and returns the length of a list of ListParseNodes.
/// \param list Pointer to the list of nodes to be counted, or null.
BOND_API uint32_t GetLength(const ListParseNode *list);


/// \brief Returns the node that follows the given one in a list of ListParseNodes, cast to the
/// same type, or null if it is the last node.
/// \param list The current node. It must not be null.
template <typename NodeType>
NodeType *NextNode(NodeType *list) { return static_cast<NodeType *>(list->GetNextNode()); }


/// \internal
template <typename T>
class CastVisitor: public ParseNodeVisitorAdapter
{
public:
	CastVisitor(): mNode(nullptr) {}

	using ParseNodeVisitor::Visit;
	virtual void Visit(T *node) override { mNode = node; }

	T *GetNode() { return mNode; }

private:
	T *mNode;
};


/// \brief Returns the given ParseNode cast to the concrete type T, or null if its concrete type
/// is not exactly T. The type is determined with a visitor rather than RTTI.
/// \param node The node to be cast, or null.
template <typename T>
T *CastNode(ParseNode *node)
{
	if (node != nullptr)
	{
		CastVisitor<T> castVisitor;
		node->Accept(castVisitor);
		return castVisitor.GetNode();
	}
	return static_cast<T *>(nullptr);
}


/// \brief Returns the given ParseNode cast to the concrete type T, or null if its concrete type
/// is not exactly T. The type is determined with a visitor rather than RTTI.
/// \param node The node to be cast, or null.
template <typename T>
const T *CastNode(const ParseNode *node)
{
	if (node != nullptr)
	{
		CastVisitor<const T> castVisitor;
		node->Accept(castVisitor);
		return castVisitor.GetNode();
	}
	return static_cast<const T *>(nullptr);
}


/// \brief Inserts a node into a sorted list of ListParseNodes, after any nodes that compare equal
/// to it.
/// \param head The first node of the list, or null if the list is empty.
/// \param node The node to be inserted.
/// \param compare A strict weak ordering that returns whether its first argument precedes its
///        second.
/// \returns The first node of the resulting list.
template <typename NodeType, typename Comparator>
NodeType *InsertNode(NodeType *head, NodeType *node, Comparator compare)
{
	if (head == nullptr)
	{
		node->SetNextNode(nullptr);
		return node;
	}

	if (compare(*node, *head))
	{
		node->SetNextNode(head);
		return node;
	}

	NodeType *current = head;
	NodeType *next = NextNode(head);
	while ((next != nullptr) && !compare(*node, *next))
	{
		current = next;
		next = NextNode(next);
	}

	current->SetNextNode(node);
	node->SetNextNode(next);
	return head;
}

/*
template <typename NodeType, typename Comparator>
NodeType *SortNodes(NodeType *list, Comparator compare)
{
	NodeType *node = list;
	NodeType *head = nullptr;
	while (node != nullptr)
	{
		NodeType *current = node;
		node = NextNode(node);
		head = InsertNode<NodeType, Comparator>(head, current, compare);
	}
	return head;
}
*/

/// \brief Returns whether values of the given types can be compared, i.e. whether both are
/// pointers, both are numeric or both are bool.
BOND_API bool AreComparableTypes(const TypeDescriptor &typeA, const TypeDescriptor &typeB);

/// \brief Returns whether a value of one type can be converted to another, i.e. whether the
/// types are comparable or are the same struct or enum type.
BOND_API bool AreConvertibleTypes(const TypeDescriptor &fromType, const TypeDescriptor &toType);

/// \brief Returns whether two types are the same, recursing through pointers and arrays, where
/// array lengths must also match. The comparison is not symmetric with respect to const: it
/// fails if fromType is const and toType is not, at any level of indirection, but succeeds if
/// only toType is const.
BOND_API bool AreSameTypes(const TypeDescriptor &fromType, const TypeDescriptor &toType);

/// \brief Returns whether a value of one type can be assigned to an object of another type.
///
/// The destination type must be assignable, i.e. a non-const lvalue that is not an array.
/// Pointers can be assigned if the source is null or a void pointer, if the destination is a
/// void pointer, or if they point to the same type. Values can be assigned if their types are
/// convertible.
BOND_API bool AreAssignableTypes(const TypeDescriptor &fromType, const TypeDescriptor &toType);

/// \brief Returns whether converting a numeric value to the given numeric type may lose data.
///
/// If the value is a compile-time constant, the conversion is lossy only if the value is not
/// exactly representable by the destination type. Otherwise, it is lossy if the destination type
/// cannot represent every value of the source type. Conversions involving non-numeric types are
/// never reported as lossy.
/// \param fromTav The type and value being converted.
/// \param toType The type to which the value is converted.
BOND_API bool IsLossyConversion(const TypeAndValue &fromTav, const TypeDescriptor &toType);

/// \brief Returns the type of an operand after C's integer promotions.
///
/// Integer types narrower than int are promoted to int, other numeric types and bool are
/// unchanged, and pointers and arrays become pointer rvalues.
BOND_API TypeDescriptor PromoteType(const TypeDescriptor &type);

/// \brief Returns the type in which a binary operation on operands of the given types is
/// performed, following C's usual arithmetic conversions.
///
/// If either operand is a pointer, the result is that pointer type as an rvalue. Otherwise,
/// double takes precedence over float, which takes precedence over the promoted integer types,
/// among which the wider one wins and unsigned wins when the widths are equal.
BOND_API TypeDescriptor CombineOperandTypes(const TypeDescriptor &typeA, const TypeDescriptor &typeB);

/// \brief Returns true if the operand has a signed integer type, the operation in which it is used has
/// an unsigned integer type and the operand is not a constant known to be non-negative.
/// \param operandTav The type and value of the operand prior to promotion.
/// \param operationType The type in which the operation is performed, as returned by CombineOperandTypes.
BOND_API bool MayConvertNegativeToUnsigned(const TypeAndValue &operandTav, const TypeDescriptor &operationType);

/// \brief Converts the value of a constant literal Token to the given primitive type, as a C
/// cast would.
/// \param fromValue A Token whose type is a numeric or bool constant literal.
/// \param toType The keyword Token type of the primitive type to convert to, e.g. `KEY_INT`.
/// \returns The converted value, or a zero Value if the conversion is not supported.
BOND_API Value CastValue(const Token &fromValue, Token::TokenType toType);

/// \brief Converts a compile-time constant value to the given primitive type, as a C cast would.
/// \param fromTav The type and value to convert. The value must be defined.
/// \param toType The primitive type to convert to.
/// \returns The converted value, or a zero Value if the conversion is not supported.
BOND_API Value CastValue(const TypeAndValue &fromTav, const TypeDescriptor &toType);

/// \brief Returns whether the given Token is a signed integer constant literal whose value is
/// negative.
BOND_API bool IsNegativeIntegerConstant(const Token &token);

/// @}

}

#endif
