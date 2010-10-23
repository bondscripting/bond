#include "bond/parsenodefactory.h"
#include "bond/parsenodes.h"
#include "bond/parsenodevisitor.h"
#include <new>

namespace Bond
{

//------------------------------------------------------------------------------
// ParseNodeDeallocator
//------------------------------------------------------------------------------

class ParseNodeDeallocator: public ParseNodeVisitor
{
public:
	ParseNodeDeallocator(Allocator &allocator):
		mAllocator(allocator)
	{}

	virtual ~ParseNodeDeallocator() {}

	void Destroy(ParseNode *parseNode);

	virtual void VisitTranslationUnit(TranslationUnit *translationUnit);
	virtual void VisitNamespaceDefinition(NamespaceDefinition *namespaceDefinition);
	virtual void VisitEnumDeclaration(EnumDeclaration *enumDeclaration);
	virtual void VisitEnumerator(Enumerator *enumerator);
	virtual void VisitFunctionDefinition(FunctionDefinition *functionDefinition);
	virtual void VisitFunctionPrototype(FunctionPrototype *functionPrototype);
	virtual void VisitParameter(Parameter *parameter);
	virtual void VisitTypeDescriptor(TypeDescriptor *typeDescriptor);
	virtual void VisitTypeSpecifier(TypeSpecifier *typeSpecifier);
	virtual void VisitNamedInitializer(NamedInitializer *namedInitializer);
	virtual void VisitInitializer(Initializer *initializer);
	virtual void VisitQualifiedIdentifier(QualifiedIdentifier *identifier) {}
	virtual void VisitCompoundStatement(CompoundStatement *compoundStatement);
	virtual void VisitIfStatement(IfStatement *ifStatement);
	virtual void VisitSwitchStatement(SwitchStatement *switchStatement);
	virtual void VisitSwitchSection(SwitchSection *switchSection);
	virtual void VisitSwitchLabel(SwitchLabel *switchLabel);
	virtual void VisitWhileStatement(WhileStatement *whileStatement);
	virtual void VisitJumpStatement(JumpStatement *jumpStatement);
	virtual void VisitDeclarativeStatement(DeclarativeStatement *declarativeStatement);
	virtual void VisitExpressionStatement(ExpressionStatement *expressionStatement);
	virtual void VisitConditionalExpression(ConditionalExpression *conditionalExpression);
	virtual void VisitBinaryExpression(BinaryExpression *binaryExpression);
	virtual void VisitUnaryExpression(UnaryExpression *unaryExpression);
	virtual void VisitPostfixExpression(PostfixExpression *postfixExpression);
	virtual void VisitMemberExpression(MemberExpression *memberExpression);
	virtual void VisitArraySubscriptExpression(ArraySubscriptExpression *arraySubscriptExpression);
	virtual void VisitFunctionCallExpression(FunctionCallExpression *functionCallExpression);
	virtual void VisitCastExpression(CastExpression *castExpression);
	virtual void VisitSizeofExpression(SizeofExpression *sizeofExpression);
	virtual void VisitConstantExpression(ConstantExpression *constantExpression) {}
	virtual void VisitIdentifierExpression(IdentifierExpression *identifierExpression);

private:
	void DestroyList(ListParseNode *listNode);

	Allocator &mAllocator;
};


void ParseNodeDeallocator::Destroy(ParseNode *parseNode)
{
	if (parseNode != 0)
	{
		parseNode->Accept(*this);
		mAllocator.Free(parseNode);
	}
}


void ParseNodeDeallocator::VisitTranslationUnit(TranslationUnit *translationUnit)
{
	DestroyList(translationUnit->GetExternalDeclarationList());
}


void ParseNodeDeallocator::VisitNamespaceDefinition(NamespaceDefinition *namespaceDefinition)
{
	DestroyList(namespaceDefinition->GetExternalDeclarationList());
}


void ParseNodeDeallocator::VisitEnumDeclaration(EnumDeclaration *enumDeclaration)
{
	DestroyList(enumDeclaration->GetEnumeratorList());
}


void ParseNodeDeallocator::VisitEnumerator(Enumerator *enumerator)
{
	Destroy(enumerator->GetValue());
}


void ParseNodeDeallocator::VisitFunctionDefinition(FunctionDefinition *functionDefinition)
{
	Destroy(functionDefinition->GetPrototype());
	Destroy(functionDefinition->GetBody());
}


void ParseNodeDeallocator::VisitFunctionPrototype(FunctionPrototype *functionPrototype)
{
	Destroy(functionPrototype->GetReturnType());
	DestroyList(functionPrototype->GetParameterList());
}


void ParseNodeDeallocator::VisitParameter(Parameter *parameter)
{
	Destroy(parameter->GetTypeDescriptor());
}


void ParseNodeDeallocator::VisitTypeDescriptor(TypeDescriptor *typeDescriptor)
{
	Destroy(typeDescriptor->GetTypeSpecifier());
	Destroy(typeDescriptor->GetParent());
	Destroy(typeDescriptor->GetLength());
}


void ParseNodeDeallocator::VisitTypeSpecifier(TypeSpecifier *typeSpecifier)
{
	DestroyList(typeSpecifier->GetIdentifier());
}


void ParseNodeDeallocator::VisitNamedInitializer(NamedInitializer *namedInitializer)
{
	Destroy(namedInitializer->GetInitializer());
}


void ParseNodeDeallocator::VisitInitializer(Initializer *initializer)
{
	Destroy(initializer->GetExpression());
	DestroyList(initializer->GetInitializerList());
}


void ParseNodeDeallocator::VisitCompoundStatement(CompoundStatement *compoundStatement)
{
	DestroyList(compoundStatement->GetStatementList());
}


void ParseNodeDeallocator::VisitIfStatement(IfStatement *ifStatement)
{
	Destroy(ifStatement->GetCondition());
	Destroy(ifStatement->GetThenStatement());
	Destroy(ifStatement->GetElseStatement());
}


void ParseNodeDeallocator::VisitSwitchStatement(SwitchStatement *switchStatement)
{
	Destroy(switchStatement->GetControl());
	DestroyList(switchStatement->GetSectionList());
}


void ParseNodeDeallocator::VisitSwitchSection(SwitchSection *switchSection)
{
	DestroyList(switchSection->GetLabelList());
	DestroyList(switchSection->GetStatementList());
}


void ParseNodeDeallocator::VisitSwitchLabel(SwitchLabel *switchLabel)
{
	Destroy(switchLabel->GetExpression());
}


void ParseNodeDeallocator::VisitWhileStatement(WhileStatement *whileStatement)
{
	Destroy(whileStatement->GetCondition());
	Destroy(whileStatement->GetBody());
}


void ParseNodeDeallocator::VisitJumpStatement(JumpStatement *jumpStatement)
{
	Destroy(jumpStatement->GetRhs());
}


void ParseNodeDeallocator::VisitDeclarativeStatement(DeclarativeStatement *declarativeStatement)
{
	Destroy(declarativeStatement->GetTypeDescriptor());
	DestroyList(declarativeStatement->GetNamedInitializerList());
}


void ParseNodeDeallocator::VisitExpressionStatement(ExpressionStatement *expressionStatement)
{
	Destroy(expressionStatement->GetExpression());
}


void ParseNodeDeallocator::VisitConditionalExpression(ConditionalExpression *conditionalExpression)
{
	Destroy(conditionalExpression->GetCondition());
	Destroy(conditionalExpression->GetTrueExpression());
	Destroy(conditionalExpression->GetFalseExpression());
}


void ParseNodeDeallocator::VisitBinaryExpression(BinaryExpression *binaryExpression)
{
	Destroy(binaryExpression->GetLhs());
	Destroy(binaryExpression->GetRhs());
}


void ParseNodeDeallocator::VisitUnaryExpression(UnaryExpression *unaryExpression)
{
	Destroy(unaryExpression->GetRhs());
}


void ParseNodeDeallocator::VisitPostfixExpression(PostfixExpression *postfixExpression)
{
	Destroy(postfixExpression->GetLhs());
}


void ParseNodeDeallocator::VisitMemberExpression(MemberExpression *memberExpression)
{
	Destroy(memberExpression->GetLhs());
}


void ParseNodeDeallocator::VisitArraySubscriptExpression(ArraySubscriptExpression *arraySubscriptExpression)
{
	Destroy(arraySubscriptExpression->GetLhs());
	Destroy(arraySubscriptExpression->GetIndex());
}


void ParseNodeDeallocator::VisitFunctionCallExpression(FunctionCallExpression *functionCallExpression)
{
	Destroy(functionCallExpression->GetLhs());
	DestroyList(functionCallExpression->GetArgumentList());
}


void ParseNodeDeallocator::VisitCastExpression(CastExpression *castExpression)
{
	Destroy(castExpression->GetRhs());
}


void ParseNodeDeallocator::VisitSizeofExpression(SizeofExpression *sizeofExpression)
{
	Destroy(sizeofExpression->GetTypeDescriptor());
	Destroy(sizeofExpression->GetRhs());
}


void ParseNodeDeallocator::VisitIdentifierExpression(IdentifierExpression *identifierExpression)
{
	DestroyList(identifierExpression->GetIdentifier());
}


void ParseNodeDeallocator::DestroyList(ListParseNode *listNode)
{
	ListParseNode *current = listNode;
	while (current != 0)
	{
		ListParseNode *next = current->GetNext();
		Destroy(current);
		current = next;
	}
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
	ListParseNode *thenStatement,
	ListParseNode *elseStatement)
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


WhileStatement *ParseNodeFactory::CreateWhileStatement(Expression *condition, ListParseNode *body)
{
	return new (mAllocator.Alloc<WhileStatement>()) WhileStatement(condition, body, WhileStatement::VARIANT_WHILE);
}


WhileStatement *ParseNodeFactory::CreateDoWhileStatement(Expression *condition, ListParseNode *body)
{
	return new (mAllocator.Alloc<WhileStatement>()) WhileStatement(condition, body, WhileStatement::VARIANT_DO_WHILE);
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

}
