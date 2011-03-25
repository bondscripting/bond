#ifndef BOND_PARSENODEUTIL_H
#define BOND_PARSENODEUTIL_H

#include "bond/parsenodes.h"
#include "bond/parsenodevisitor.h"

namespace Bond
{

class Symbol;

template<typename T>
class CastVisitor: public ParseNodeVisitorAdapter
{
public:
	CastVisitor(): mNode(0) {}

	virtual void Visit(T *node) { mNode = node; }

	T *GetNode() { return mNode; }

private:
	T *mNode;
};


template<typename T>
T *CastNode(ParseNode *node)
{
	CastVisitor<T> castVisitor;
	node->Accept(castVisitor);
	return castVisitor.GetNode();
}


template<typename T>
const T *CastNode(const ParseNode *node)
{
	CastVisitor<const T> castVisitor;
	node->Accept(castVisitor);
	return castVisitor.GetNode();
}


bool AreMatchingTypes(const TypeSpecifier *typeA, const TypeSpecifier *typeB);
bool AreMatchingTypes(const TypeDescriptor *typeA, const TypeDescriptor *typeB);
bool AreMatchingFunctionPrototypes(const Symbol *scope, const FunctionPrototype *functionA, const FunctionPrototype *functionB);

bool AreComparableTypes(const TypeDescriptor *typeA, const TypeDescriptor *typeB);

TypeDescriptor CombineOperandTypes(const TypeDescriptor *typeA, const TypeDescriptor *typeB);

Value CastValue(const TypeAndValue &value, const TypeDescriptor *type);

Value UnaryMinus(const TypeAndValue &value);

Value UnaryBitNot(const TypeAndValue &value);

}

#endif
