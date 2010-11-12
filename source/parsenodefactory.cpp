#include "bond/parsenodefactory.h"
#include "bond/parsenodes.h"
#include "bond/parsenodetraverser.h"
#include <new>

namespace Bond
{

//------------------------------------------------------------------------------
// ParseNodeDeallocator
//------------------------------------------------------------------------------

class ParseNodeDeallocator: private ParseNodeTraverser
{
public:
	ParseNodeDeallocator(Allocator &allocator):
		mAllocator(allocator)
	{}

	void Destroy(ParseNode *parseNode) { Traverse(parseNode); }
	void DestroyList(ListParseNode *listNode) { TraverseList(listNode); }

private:
	virtual void Traverse(ParseNode *parseNode);

	Allocator &mAllocator;
};


void ParseNodeDeallocator::Traverse(ParseNode *parseNode)
{
	ParseNodeTraverser::Traverse(parseNode);
	mAllocator.Free(parseNode);
}


//------------------------------------------------------------------------------
// ParseNodeFactory
//------------------------------------------------------------------------------

TranslationUnit *ParseNodeFactory::CreateTranslationUnit(ListParseNode *declarationList)
{
	return new (mAllocator.Alloc<TranslationUnit>()) TranslationUnit(declarationList);
}


NamespaceDefinition *ParseNodeFactory::CreateNamespaceDefinition(
	const Token *name,
	ListParseNode *declarationList)
{
	return new (mAllocator.Alloc<NamespaceDefinition>()) NamespaceDefinition(name, declarationList);
}


EnumDeclaration *ParseNodeFactory::CreateEnumDeclaration(const Token *name, Enumerator *enumeratorList)
{
	return new (mAllocator.Alloc<EnumDeclaration>()) EnumDeclaration(name, enumeratorList);
}


Enumerator *ParseNodeFactory::CreateEnumerator(const Token *name, Expression *value)
{
	return new (mAllocator.Alloc<Enumerator>()) Enumerator(name, value);
}


StructDeclaration *ParseNodeFactory::CreateStructDeclaration(const Token *name, ListParseNode *memberList)
{
	return new (mAllocator.Alloc<StructDeclaration>()) StructDeclaration(name, memberList);
}


FunctionDefinition *ParseNodeFactory::CreateFunctionDefinition(FunctionPrototype *prototype, CompoundStatement *body)
{
	return new (mAllocator.Alloc<FunctionDefinition>()) FunctionDefinition(prototype, body);
}


FunctionPrototype *ParseNodeFactory::CreateFunctionPrototype(
	const Token *name,
	TypeDescriptor *returnType,
	Parameter *parameterList)
{
	return new (mAllocator.Alloc<FunctionPrototype>()) FunctionPrototype(name, returnType, parameterList);
}


Parameter *ParseNodeFactory::CreateParameter(const Token *name, TypeDescriptor *typeDescriptor)
{
	return new (mAllocator.Alloc<Parameter>()) Parameter(name, typeDescriptor);
}


TypeDescriptor *ParseNodeFactory::CreateTypeDescriptor(TypeSpecifier *specifier, bool isConst)
{
	return new (mAllocator.Alloc<TypeDescriptor>()) TypeDescriptor(specifier, isConst);
}


TypeDescriptor *ParseNodeFactory::CreateTypeDescriptor(TypeDescriptor *parent, bool isConst)
{
	return new (mAllocator.Alloc<TypeDescriptor>()) TypeDescriptor(parent, isConst);
}


TypeDescriptor *ParseNodeFactory::CreateTypeDescriptor(TypeDescriptor *parent, Expression *length)
{
	return new (mAllocator.Alloc<TypeDescriptor>()) TypeDescriptor(parent, length);
}


TypeSpecifier *ParseNodeFactory::CreateTypeSpecifier(const Token *primitiveType)
{
	return new (mAllocator.Alloc<TypeSpecifier>()) TypeSpecifier(primitiveType);
}


TypeSpecifier *ParseNodeFactory::CreateTypeSpecifier(QualifiedIdentifier *identifier)
{
	return new (mAllocator.Alloc<TypeSpecifier>()) TypeSpecifier(identifier);
}


NamedInitializer *ParseNodeFactory::CreateNamedInitializer(const Token *name, Initializer *initializer)
{
	return new (mAllocator.Alloc<NamedInitializer>()) NamedInitializer(name, initializer);
}


Initializer *ParseNodeFactory::CreateInitializer(Expression *expression)
{
	return new (mAllocator.Alloc<Initializer>()) Initializer(expression);
}


Initializer *ParseNodeFactory::CreateInitializer(Initializer *initializerList)
{
	return new (mAllocator.Alloc<Initializer>()) Initializer(initializerList);
}


QualifiedIdentifier *ParseNodeFactory::CreateQualifiedIdentifier(const Token *name)
{
	return new (mAllocator.Alloc<QualifiedIdentifier>()) QualifiedIdentifier(name);
}


CompoundStatement *ParseNodeFactory::CreateCompoundStatement(ListParseNode *statementList)
{
	return new (mAllocator.Alloc<CompoundStatement>()) CompoundStatement(statementList);
}


IfStatement *ParseNodeFactory::CreateIfStatement(
	Expression *condition,
	ParseNode *thenStatement,
	ParseNode *elseStatement)
{
	return new (mAllocator.Alloc<IfStatement>()) IfStatement(condition, thenStatement, elseStatement);
}


SwitchStatement *ParseNodeFactory::CreateSwitchStatement(Expression *control, SwitchSection *sectionList)
{
	return new (mAllocator.Alloc<SwitchStatement>()) SwitchStatement(control, sectionList);
}


SwitchSection *ParseNodeFactory::CreateSwitchSection(SwitchLabel *labelList, ListParseNode* statementList)
{
	return new (mAllocator.Alloc<SwitchSection>()) SwitchSection(labelList, statementList);
}


SwitchLabel *ParseNodeFactory::CreateSwitchLabel(const Token *label, Expression *expression)
{
	return new (mAllocator.Alloc<SwitchLabel>()) SwitchLabel(label, expression);
}


SwitchLabel *ParseNodeFactory::CreateDefaultLabel(const Token *label)
{
	return new (mAllocator.Alloc<SwitchLabel>()) SwitchLabel(label);
}


WhileStatement *ParseNodeFactory::CreateWhileStatement(Expression *condition, ParseNode *body)
{
	return new (mAllocator.Alloc<WhileStatement>()) WhileStatement(condition, body, WhileStatement::VARIANT_WHILE);
}


WhileStatement *ParseNodeFactory::CreateDoWhileStatement(Expression *condition, ParseNode *body)
{
	return new (mAllocator.Alloc<WhileStatement>()) WhileStatement(condition, body, WhileStatement::VARIANT_DO_WHILE);
}


ForStatement *ParseNodeFactory::CreateForStatement(
	ParseNode *initializer,
	Expression *condition,
	Expression *countingExpression,
	ParseNode *body)
{
	return new (mAllocator.Alloc<ForStatement>()) ForStatement(initializer, condition, countingExpression, body);
}


JumpStatement *ParseNodeFactory::CreateJumpStatement(const Token *op, Expression *rhs)
{
	return new (mAllocator.Alloc<JumpStatement>()) JumpStatement(op, rhs);
}


DeclarativeStatement *ParseNodeFactory::CreateDeclarativeStatement(
	TypeDescriptor *typeDescriptor,
	NamedInitializer *initializerList)
{
	return new (mAllocator.Alloc<DeclarativeStatement>()) DeclarativeStatement(typeDescriptor, initializerList);
}


ExpressionStatement *ParseNodeFactory::CreateExpressionStatement(Expression *expression)
{
	return new (mAllocator.Alloc<ExpressionStatement>()) ExpressionStatement(expression);
}


ConditionalExpression *ParseNodeFactory::CreateConditionalExpression(
	Expression *condition,
	Expression *trueExpression,
	Expression *falseExpression)
{
	return new (mAllocator.Alloc<ConditionalExpression>()) ConditionalExpression(condition, trueExpression, falseExpression);
}


BinaryExpression *ParseNodeFactory::CreateBinaryExpression(const Token *op, Expression *lhs, Expression *rhs)
{
	return new (mAllocator.Alloc<BinaryExpression>()) BinaryExpression(op, lhs, rhs);
}


UnaryExpression *ParseNodeFactory::CreateUnaryExpression(const Token *op, Expression *rhs)
{
	return new (mAllocator.Alloc<UnaryExpression>()) UnaryExpression(op, rhs);
}


PostfixExpression *ParseNodeFactory::CreatePostfixExpression(const Token *op, Expression *lhs)
{
	return new (mAllocator.Alloc<PostfixExpression>()) PostfixExpression(op, lhs);
}


MemberExpression *ParseNodeFactory::CreateMemberExpression(const Token *op, const Token *memberName, Expression *lhs)
{
	return new (mAllocator.Alloc<MemberExpression>()) MemberExpression(op, memberName, lhs);
}


ArraySubscriptExpression *ParseNodeFactory::CreateArraySubscriptExpression(Expression *lhs, Expression *index)
{
	return new (mAllocator.Alloc<ArraySubscriptExpression>()) ArraySubscriptExpression(lhs, index);
}


FunctionCallExpression *ParseNodeFactory::CreateFunctionCallExpression(Expression *lhs, Expression *argumentList)
{
	return new (mAllocator.Alloc<FunctionCallExpression>()) FunctionCallExpression(lhs, argumentList);
}


CastExpression *ParseNodeFactory::CreateCastExpression(TypeDescriptor *typeDescriptor, Expression *rhs)
{
	return new (mAllocator.Alloc<CastExpression>()) CastExpression(typeDescriptor, rhs);
}


SizeofExpression *ParseNodeFactory::CreateSizeofExpression(TypeDescriptor *typeDescriptor)
{
	return new (mAllocator.Alloc<SizeofExpression>()) SizeofExpression(typeDescriptor);
}


SizeofExpression *ParseNodeFactory::CreateSizeofExpression(Expression *rhs)
{
	return new (mAllocator.Alloc<SizeofExpression>()) SizeofExpression(rhs);
}


ConstantExpression *ParseNodeFactory::CreateConstantExpression(const Token *value)
{
	return new (mAllocator.Alloc<ConstantExpression>()) ConstantExpression(value);
}


IdentifierExpression *ParseNodeFactory::CreateIdentifierExpression(QualifiedIdentifier *identifier)
{
	return new (mAllocator.Alloc<IdentifierExpression>()) IdentifierExpression(identifier);
}


void ParseNodeFactory::Destroy(ParseNode *parseNode)
{
	mAllocator.Free(parseNode);
}


void ParseNodeFactory::DestroyHierarchy(ParseNode *parseNode)
{
	ParseNodeDeallocator deallocator(mAllocator);
	deallocator.Destroy(parseNode);
}

void ParseNodeFactory::DestroyListHierarchy(ListParseNode *listNode)
{
	ParseNodeDeallocator deallocator(mAllocator);
	deallocator.DestroyList(listNode);
}

}
