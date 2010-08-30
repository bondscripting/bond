#ifndef BOND_PARSENODEFACTORY_H
#define BOND_PARSENODEFACTORY_H

#include "bond/allocator.h"
#include "bond/parsenodesfwd.h"
#include "bond/token.h"

namespace Bond
{

class ParseNodeFactory
{
public:
	ParseNodeFactory(Allocator &allocator):
		mAllocator(allocator)
	{}

	TranslationUnit *CreateTranslationUnit(ExternalDeclaration *declarations);
	NamespaceDefinition *CreateNamespaceDefinition(const Token *name, ExternalDeclaration *declarations);
	EnumDeclaration *CreateEnumDeclaration(const Token *name, Enumerator *enumerators);
	Enumerator *CreateEnumerator(const Token *name, Expression *value);
	TypeDescriptor *CreateTypeDescriptor();

	ConditionalExpression *CreateConditionalExpression(
		Expression *condition,
		Expression *trueExpression,
		Expression *falseExpression);

	BinaryExpression *CreateBinaryExpression(const Token *op, Expression *lhs, Expression *rhs);
	UnaryExpression *CreateUnaryExpression(const Token *op, Expression *rhs);
	CastExpression *CreateCastExpression(TypeDescriptor *typeDescriptor, Expression *rhs);
	SizeofExpression *CreateSizeofExpression(TypeDescriptor *typeDescriptor);
	SizeofExpression *CreateSizeofExpression(Expression *rhs);
	ConstantValue *CreateConstantValue(const Token *value);

	void Destroy(ParseNode *parseNode);
	void DestroyHierarchy(ParseNode *parseNode);

private:
	Allocator &mAllocator;
};

}

#endif
