#ifndef BOND_PARSENODEFACTORY_H
#define BOND_PARSENODEFACTORY_H

#include "bond/allocator.h"
#include "bond/parsenodes.h"
#include "bond/token.h"

namespace Bond
{

class ParseNodeFactory
{
public:
	ParseNodeFactory(Allocator &allocator):
		mAllocator(allocator)
	{}

	TranslationUnit *CreateTranslationUnit(ListParseNode *declarationList);
	NamespaceDefinition *CreateNamespaceDefinition(const Token *name, ListParseNode *declarationList);
	EnumDeclaration *CreateEnumDeclaration(const Token *name);

	Enumerator *CreateEnumerator(
		const Token *name,
		TypeDescriptor *typeDescriptor,
		Expression *value);

	StructDeclaration *CreateStructDeclaration(
		const Token *name,
		const Token *size,
		const Token *alignment,
		FunctionDefinition *memberFunctionList,
		DeclarativeStatement *memberVariableList,
		StructDeclaration::Variant variant);

	FunctionDefinition *CreateFunctionDefinition(FunctionPrototype *prototype, CompoundStatement *body);

	FunctionPrototype *CreateFunctionPrototype(
		const Token *name,
		TypeDescriptor *returnType,
		Parameter *parameterList,
		bool isConst);

	Parameter *CreateParameter(const Token *name, TypeDescriptor *typeDescriptor);
	TypeDescriptor *CreateTypeDescriptor(TypeSpecifier *specifier, bool isConst);
	TypeDescriptor *CreateTypeDescriptor(TypeDescriptor *parent, bool isConst);
	TypeSpecifier *CreateTypeSpecifier(const Token *primitiveType);
	TypeSpecifier *CreateTypeSpecifier(QualifiedIdentifier *identifier);

	NamedInitializer *CreateNamedInitializer(
	 const Token *name,
	 Initializer *initializer,
	 TypeDescriptor *typeDescriptor);

	Initializer *CreateInitializer(Expression *expression);
	Initializer *CreateInitializer(Initializer *initializerList);
	QualifiedIdentifier *CreateQualifiedIdentifier(const Token *name);
	CompoundStatement *CreateCompoundStatement(ListParseNode *statementList);

	IfStatement *CreateIfStatement(
		Expression *condition,
		ParseNode *thenStatement,
		ParseNode *elseStatement);

	SwitchStatement *CreateSwitchStatement(Expression *control, SwitchSection *sectionList);
	SwitchSection *CreateSwitchSection(SwitchLabel *labelList, ListParseNode* statementList);
	SwitchLabel *CreateSwitchLabel(const Token *label, Expression *expression);
	SwitchLabel *CreateDefaultLabel(const Token *label);
	WhileStatement *CreateWhileStatement(Expression *condition, ParseNode *body);
	WhileStatement *CreateDoWhileStatement(Expression *condition, ParseNode *body);

	ForStatement *CreateForStatement(
		ParseNode *initializer,
		Expression *condition,
		Expression *countingExpression,
		ParseNode *body);

	JumpStatement *CreateJumpStatement(const Token *op, Expression *rhs);

	DeclarativeStatement *CreateDeclarativeStatement(
		TypeDescriptor *typeDescriptor,
		NamedInitializer *initializerList);

	ExpressionStatement *CreateExpressionStatement(Expression *expression);

	ConditionalExpression *CreateConditionalExpression(
		const Token *op,
		Expression *condition,
		Expression *trueExpression,
		Expression *falseExpression);

	BinaryExpression *CreateBinaryExpression(const Token *op, Expression *lhs, Expression *rhs);
	UnaryExpression *CreateUnaryExpression(const Token *op, Expression *rhs);
	PostfixExpression *CreatePostfixExpression(const Token *op, Expression *lhs);
	MemberExpression *CreateMemberExpression(const Token *op, const Token *memberName, Expression *lhs);
	ArraySubscriptExpression *CreateArraySubscriptExpression(const Token *op, Expression *lhs, Expression *index);

	FunctionCallExpression *CreateFunctionCallExpression(
		const Token *context,
		Expression *lhs,
		Expression *argumentList);

	CastExpression *CreateCastExpression(const Token *op, TypeDescriptor *typeDescriptor, Expression *rhs);
	SizeofExpression *CreateSizeofExpression(const Token *op, TypeDescriptor *typeDescriptor);
	SizeofExpression *CreateSizeofExpression(const Token *op, Expression *rhs);
	ConstantExpression *CreateConstantExpression(const Token *value);
	IdentifierExpression *CreateIdentifierExpression(QualifiedIdentifier *identifier);
	ThisExpression *CreateThisExpression(const Token *token);
	EmptyExpression *CreateEmptyExpression();

	void Destroy(ParseNode *parseNode);
	void DestroyHierarchy(ParseNode *parseNode);
	void DestroyListHierarchy(ListParseNode *listNode);

private:
	Allocator &mAllocator;
};

}

#endif
