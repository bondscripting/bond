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
	virtual void Traverse(const ParseNode *parseNode);

	Allocator &mAllocator;
};


void ParseNodeDeallocator::Traverse(ParseNode *parseNode)
{
	ParseNodeTraverser::Traverse(parseNode);
	mAllocator.Free(parseNode);
}


void ParseNodeDeallocator::Traverse(const ParseNode *parseNode)
{
	ParseNodeTraverser::Traverse(parseNode);
	mAllocator.Free(const_cast<ParseNode *>(parseNode));
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
	return new (mAllocator.Alloc<NamespaceDefinition>())
		NamespaceDefinition(name, declarationList);
}


EnumDeclaration *ParseNodeFactory::CreateEnumDeclaration(const Token *name)
{
	return new (mAllocator.Alloc<EnumDeclaration>()) EnumDeclaration(name);
}


Enumerator *ParseNodeFactory::CreateEnumerator(
	const Token *name,
	TypeDescriptor *typeDescriptor,
	Expression *value)
{
	return new (mAllocator.Alloc<Enumerator>()) Enumerator(name, typeDescriptor, value);
}


StructDeclaration *ParseNodeFactory::CreateStructDeclaration(
	const Token *name,
	const Token *size,
	const Token *alignment,
	FunctionDefinition *memberFunctionList,
	DeclarativeStatement *memberVariableList,
	StructDeclaration::Variant variant)
{
	return new (mAllocator.Alloc<StructDeclaration>())
		StructDeclaration(name, size, alignment, memberFunctionList, memberVariableList, variant);
}


FunctionDefinition *ParseNodeFactory::CreateFunctionDefinition(FunctionPrototype *prototype, CompoundStatement *body)
{
	return new (mAllocator.Alloc<FunctionDefinition>()) FunctionDefinition(prototype, body);
}


FunctionPrototype *ParseNodeFactory::CreateFunctionPrototype(
	const Token *name,
	TypeDescriptor *returnType,
	Parameter *parameterList,
	bool isConst)
{
	return new (mAllocator.Alloc<FunctionPrototype>())
		FunctionPrototype(name, returnType, parameterList, isConst);
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


TypeSpecifier *ParseNodeFactory::CreateTypeSpecifier(const Token *primitiveType)
{
	return new (mAllocator.Alloc<TypeSpecifier>()) TypeSpecifier(primitiveType);
}


TypeSpecifier *ParseNodeFactory::CreateTypeSpecifier(QualifiedIdentifier *identifier)
{
	return new (mAllocator.Alloc<TypeSpecifier>()) TypeSpecifier(identifier);
}


NamedInitializer *ParseNodeFactory::CreateNamedInitializer(
	const Token *name,
	Initializer *initializer,
	TypeDescriptor *typeDescriptor)
{
	return new (mAllocator.Alloc<NamedInitializer>())
		NamedInitializer(name, initializer, typeDescriptor);
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
	const Token *keyword,
	Expression *condition,
	ParseNode *thenStatement,
	ParseNode *elseStatement)
{
	return new (mAllocator.Alloc<IfStatement>())
		IfStatement(keyword, condition, thenStatement, elseStatement);
}


SwitchStatement *ParseNodeFactory::CreateSwitchStatement(
	const Token *keyword,
	Expression *control,
	SwitchSection *sectionList)
{
	return new (mAllocator.Alloc<SwitchStatement>()) SwitchStatement(keyword, control, sectionList);
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


WhileStatement *ParseNodeFactory::CreateWhileStatement(
	const Token *keyword,
	Expression *condition,
	ParseNode *body)
{
	return new (mAllocator.Alloc<WhileStatement>()) WhileStatement(keyword, condition, body);
}


ForStatement *ParseNodeFactory::CreateForStatement(
	const Token *keyword,
	ParseNode *initializer,
	Expression *condition,
	Expression *countingExpression,
	ParseNode *body)
{
	return new (mAllocator.Alloc<ForStatement>())
		ForStatement(keyword, initializer, condition, countingExpression, body);
}


JumpStatement *ParseNodeFactory::CreateJumpStatement(const Token *keyword, Expression *rhs)
{
	return new (mAllocator.Alloc<JumpStatement>()) JumpStatement(keyword, rhs);
}


DeclarativeStatement *ParseNodeFactory::CreateDeclarativeStatement(
	TypeDescriptor *typeDescriptor,
	NamedInitializer *initializerList)
{
	return new (mAllocator.Alloc<DeclarativeStatement>())
		DeclarativeStatement(typeDescriptor, initializerList);
}


ExpressionStatement *ParseNodeFactory::CreateExpressionStatement(Expression *expression)
{
	return new (mAllocator.Alloc<ExpressionStatement>()) ExpressionStatement(expression);
}


ConditionalExpression *ParseNodeFactory::CreateConditionalExpression(
	const Token *op,
	Expression *condition,
	Expression *trueExpression,
	Expression *falseExpression)
{
	return new (mAllocator.Alloc<ConditionalExpression>())
		ConditionalExpression(op, condition, trueExpression, falseExpression);
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


ArraySubscriptExpression *ParseNodeFactory::CreateArraySubscriptExpression(const Token *op, Expression *lhs, Expression *index)
{
	return new (mAllocator.Alloc<ArraySubscriptExpression>())
		ArraySubscriptExpression(op, lhs, index);
}


FunctionCallExpression *ParseNodeFactory::CreateFunctionCallExpression(
	const Token *context,
	Expression *lhs,
	Expression *argumentList)
{
	return new (mAllocator.Alloc<FunctionCallExpression>())
		FunctionCallExpression(context, lhs, argumentList);
}


CastExpression *ParseNodeFactory::CreateCastExpression(const Token *op, TypeDescriptor *typeDescriptor, Expression *rhs)
{
	return new (mAllocator.Alloc<CastExpression>()) CastExpression(op, typeDescriptor, rhs);
}


SizeofExpression *ParseNodeFactory::CreateSizeofExpression(const Token *op, TypeDescriptor *typeDescriptor)
{
	return new (mAllocator.Alloc<SizeofExpression>()) SizeofExpression(op, typeDescriptor);
}


SizeofExpression *ParseNodeFactory::CreateSizeofExpression(const Token *op, Expression *rhs)
{
	return new (mAllocator.Alloc<SizeofExpression>()) SizeofExpression(op, rhs);
}


ConstantExpression *ParseNodeFactory::CreateConstantExpression(const Token *value)
{
	return new (mAllocator.Alloc<ConstantExpression>()) ConstantExpression(value);
}


ThisExpression *ParseNodeFactory::CreateThisExpression(const Token *token)
{
	return new (mAllocator.Alloc<ThisExpression>()) ThisExpression(token);
}


IdentifierExpression *ParseNodeFactory::CreateIdentifierExpression(QualifiedIdentifier *identifier)
{
	return new (mAllocator.Alloc<IdentifierExpression>()) IdentifierExpression(identifier);
}


EmptyExpression *ParseNodeFactory::CreateEmptyExpression()
{
	return new (mAllocator.Alloc<EmptyExpression>()) EmptyExpression();
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
