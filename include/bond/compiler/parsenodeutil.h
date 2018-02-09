#ifndef BOND_COMPILER_PARSENODEUTIL_H
#define BOND_COMPILER_PARSENODEUTIL_H

#include "bond/compiler/parsenodes.h"
#include "bond/compiler/parsenodevisitor.h"

namespace Bond
{

/// \addtogroup compiler
/// @{

/// \brief Counts and returns the length of a list of ListParseNodes.
/// \param list Pointer to the list of nodes to be counted.
uint32_t GetLength(const ListParseNode *list);


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

bool AreComparableTypes(const TypeDescriptor *typeA, const TypeDescriptor *typeB);
bool AreConvertibleTypes(const TypeDescriptor *fromType, const TypeDescriptor *toType);
bool AreSameTypes(const TypeDescriptor *fromType, const TypeDescriptor *toType);
bool AreAssignableTypes(const TypeDescriptor *fromType, const TypeDescriptor *toType);

TypeDescriptor PromoteType(const TypeDescriptor *type);
TypeDescriptor CombineOperandTypes(const TypeDescriptor *typeA, const TypeDescriptor *typeB);

Value CastValue(const Value &value, Token::TokenType sourceType, Token::TokenType destType);
Value CastValue(const TypeAndValue &value, const TypeDescriptor *type);

bool IsNegativeIntegerConstant(const Token *token);

/// @}

}

#endif
