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
	TypeDescriptor *CreateTypeDescriptor(TypeSpecifier *specifier, bool isConst);
	TypeDescriptor *CreateTypeDescriptor(TypeDescriptor *parent, bool isConst);
	TypeDescriptor *CreateTypeDescriptor(TypeDescriptor *parent, Expression *length);
	TypeSpecifier *CreateTypeSpecifier(const Token *primitiveType);
	TypeSpecifier *CreateTypeSpecifier(QualifiedIdentifier *identifier);
	QualifiedIdentifier *CreateQualifiedIdentifier(const Token *name);

	ConditionalExpression *CreateConditionalExpression(
		Expression *condition,
		Expression *trueExpression,
		Expression *falseExpression);

	BinaryExpression *CreateBinaryExpression(const Token *op, Expression *lhs, Expression *rhs);
	UnaryExpression *CreateUnaryExpression(const Token *op, Expression *rhs);
	PostfixExpression *CreatePostfixExpression(const Token *op, Expression *lhs);
	MemberExpression *CreateMemberExpression(const Token *op, const Token *memberName, Expression *lhs);
	ArraySubscriptExpression *CreateArraySubscriptExpression(Expression *lhs, Expression *index);
	FunctionCallExpression *CreateFunctionCallExpression(Expression *lhs, Expression *argumentList);
	CastExpression *CreateCastExpression(TypeDescriptor *typeDescriptor, Expression *rhs);
	SizeofExpression *CreateSizeofExpression(TypeDescriptor *typeDescriptor);
	SizeofExpression *CreateSizeofExpression(Expression *rhs);
	ConstantExpression *CreateConstantExpression(const Token *value);
	IdentifierExpression *CreateIdentifierExpression(QualifiedIdentifier *identifier);

	void Destroy(ParseNode *parseNode);
	void DestroyHierarchy(ParseNode *parseNode);

private:
	Allocator &mAllocator;
};

}

#endif
