#ifndef BOND_PARSENODEUTIL_H
#define BOND_PARSENODEUTIL_H

#include "bond/parsenodes.h"
#include "bond/parsenodevisitor.h"

namespace Bond
{

class Symbol;

template<typename T>
T *CastNode(ParseNode *node)
{
	class CastVisitor: public ParseNodeVisitorAdapter
	{
	public:
		CastVisitor(): mNode(0) {}

		virtual void Visit(T *node) { mNode = node; }

		T *GetNode() { return mNode; }

	private:
		T *mNode;
	};

	CastVisitor castVisitor;
	node->Accept(castVisitor);
	return castVisitor.GetNode();
}


template<typename T>
const T *CastNode(const ParseNode *node)
{
	class CastVisitor: public ConstParseNodeVisitorAdapter
	{
	public:
		CastVisitor(): mNode(0) {}

		virtual void Visit(const T *node) { mNode = node; }

		const T *GetNode() { return mNode; }

	private:
		const T *mNode;
	};

	CastVisitor castVisitor;
	node->Accept(castVisitor);
	return castVisitor.GetNode();
}


Token::TokenType GetPrimitiveType(const TypeDescriptor *typeDescriptor);

bool IsConstantTypeDescriptor(const TypeDescriptor *type);

bool AreMatchingTypeSpecifiers(const Symbol *scope, const TypeSpecifier *typeA, const TypeSpecifier *typeB);
bool AreMatchingTypeDescriptors(const Symbol *scope, const TypeDescriptor *typeA, const TypeDescriptor *typeB);
bool AreMatchingFunctionPrototypes(const Symbol *scope, const FunctionPrototype *functionA, const FunctionPrototype *functionB);

}

#endif
