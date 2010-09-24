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

	TranslationUnit *CreateTranslationUnit(ExternalDeclaration *declarationList);
	NamespaceDefinition *CreateNamespaceDefinition(const Token *name, ExternalDeclaration *declarationList);
	EnumDeclaration *CreateEnumDeclaration(const Token *name, Enumerator *enumeratorList);
	Enumerator *CreateEnumerator(const Token *name, Expression *value);
	FunctionDefinition *CreateFunctionDefinition(FunctionPrototype *prototype, CompoundStatement *body);

	FunctionPrototype *CreateFunctionPrototype(
		const Token *name,
		TypeDescriptor *returnType,
		Parameter *parameterList);

	Parameter *CreateParameter(const Token *name, TypeDescriptor *typeDescriptor);
	TypeDescriptor *CreateTypeDescriptor(TypeSpecifier *specifier, bool isConst);
	TypeDescriptor *CreateTypeDescriptor(TypeDescriptor *parent, bool isConst);
	TypeDescriptor *CreateTypeDescriptor(TypeDescriptor *parent, Expression *length);
	TypeSpecifier *CreateTypeSpecifier(const Token *primitiveType);
	TypeSpecifier *CreateTypeSpecifier(QualifiedIdentifier *identifier);
	QualifiedIdentifier *CreateQualifiedIdentifier(const Token *name);
	CompoundStatement *CreateCompoundStatement(Statement *statementList);
	IfStatement *CreateIfStatement(Expression *condition, Statement *thenStatement, Statement *elseStatement);
	SwitchStatement *CreateSwitchStatement(Expression *control, SwitchSection *sectionList);
	SwitchSection *CreateSwitchSection(SwitchLabel *labelList, Statement* statementList);
	SwitchLabel *CreateSwitchLabel(const Token *label, Expression *expression);
	SwitchLabel *CreateDefaultLabel(const Token *label);
	WhileStatement *CreateWhileStatement(Expression *condition, Statement *body);
	WhileStatement *CreateDoWhileStatement(Expression *condition, Statement *body);
	JumpStatement *CreateJumpStatement(const Token *op, Expression *rhs);

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
