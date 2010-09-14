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
	virtual void VisitQualifiedIdentifier(QualifiedIdentifier *identifier);
	virtual void VisitConditionalExpression(ConditionalExpression *conditionalExpression);
	virtual void VisitBinaryExpression(BinaryExpression *binaryExpression);
	virtual void VisitUnaryExpression(UnaryExpression *unaryExpression);
	virtual void VisitPostfixExpression(PostfixExpression *postfixExpression);
	virtual void VisitMemberExpression(MemberExpression *memberExpression);
	virtual void VisitArraySubscriptExpression(ArraySubscriptExpression *arraySubscriptExpression);
	virtual void VisitFunctionCallExpression(FunctionCallExpression *functionCallExpression);
	virtual void VisitCastExpression(CastExpression *castExpression);
	virtual void VisitSizeofExpression(SizeofExpression *sizeofExpression);
	virtual void VisitConstantExpression(ConstantExpression *constantExpression);
	virtual void VisitIdentifierExpression(IdentifierExpression *identifierExpression);

private:
	void DestroyExternalDeclarationList(ExternalDeclaration *declarationList);
	void DestroyEnumeratorList(Enumerator *enumeratorList);
	void DestroyParameterList(Parameter *parameterList);
	void DestroyQualifiedIdentifier(QualifiedIdentifier *identifier);
	void DestroyExpressionList(Expression *expressionList);

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
	DestroyExternalDeclarationList(translationUnit->GetExternalDeclarationList());
}


void ParseNodeDeallocator::VisitNamespaceDefinition(NamespaceDefinition *namespaceDefinition)
{
	DestroyExternalDeclarationList(namespaceDefinition->GetExternalDeclarationList());
}


void ParseNodeDeallocator::VisitEnumDeclaration(EnumDeclaration *enumDeclaration)
{
	DestroyEnumeratorList(enumDeclaration->GetEnumeratorList());
}


void ParseNodeDeallocator::VisitEnumerator(Enumerator *enumerator)
{
	Destroy(enumerator->GetValue());
}


void ParseNodeDeallocator::VisitFunctionDefinition(FunctionDefinition *functionDefinition)
{
	Destroy(functionDefinition->GetPrototype());
}


void ParseNodeDeallocator::VisitFunctionPrototype(FunctionPrototype *functionPrototype)
{
	Destroy(functionPrototype->GetReturnType());
	DestroyParameterList(functionPrototype->GetParameterList());
}


void ParseNodeDeallocator::VisitParameter(Parameter *parameter)
{
	Destroy(parameter->GetTypeDescriptor());
}


void ParseNodeDeallocator::VisitTypeDescriptor(TypeDescriptor *typeDescriptor) {}


void ParseNodeDeallocator::VisitTypeSpecifier(TypeSpecifier *typeSpecifier)
{
	DestroyQualifiedIdentifier(typeSpecifier->GetIdentifier());
}


void ParseNodeDeallocator::VisitQualifiedIdentifier(QualifiedIdentifier *identifier) {}


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
	DestroyExpressionList(functionCallExpression->GetArgumentList());
}


void ParseNodeDeallocator::VisitCastExpression(CastExpression *castExpression)
{
	Destroy(castExpression->GetRhs());
}


void ParseNodeDeallocator::VisitSizeofExpression(SizeofExpression *sizeofExpression)
{
	Destroy(sizeofExpression->GetRhs());
}


void ParseNodeDeallocator::VisitConstantExpression(ConstantExpression *constantExpression)
{
}


void ParseNodeDeallocator::VisitIdentifierExpression(IdentifierExpression *identifierExpression)
{
	DestroyQualifiedIdentifier(identifierExpression->GetIdentifier());
}


void ParseNodeDeallocator::DestroyExternalDeclarationList(ExternalDeclaration *declarationList)
{
	ExternalDeclaration *current = declarationList;
	while (current != 0)
	{
		ExternalDeclaration *next = current->GetNext();
		Destroy(current);
		current = next;
	}
}


void ParseNodeDeallocator::DestroyEnumeratorList(Enumerator *enumeratorList)
{
	Enumerator *current = enumeratorList;
	while (current != 0)
	{
		Enumerator *next = current->GetNext();
		Destroy(current);
		current = next;
	}
}


void ParseNodeDeallocator::DestroyParameterList(Parameter *parameterList)
{
	Parameter *current = parameterList;
	while (current != 0)
	{
		Parameter *next = current->GetNext();
		Destroy(current);
		current = next;
	}
}


void ParseNodeDeallocator::DestroyQualifiedIdentifier(QualifiedIdentifier *identifier)
{
	QualifiedIdentifier *current = identifier;
	while (current != 0)
	{
		QualifiedIdentifier *next = current->GetNext();
		Destroy(current);
		current = next;
	}
}


void ParseNodeDeallocator::DestroyExpressionList(Expression *expressionList)
{
	Expression *current = expressionList;
	while (current != 0)
	{
		Expression *next = current->GetNext();
		Destroy(current);
		current = next;
	}
}


//------------------------------------------------------------------------------
// ParseNodeFactory
//------------------------------------------------------------------------------

TranslationUnit *ParseNodeFactory::CreateTranslationUnit(ExternalDeclaration *declarationList)
{
	return new (mAllocator.Alloc<TranslationUnit>()) TranslationUnit(declarationList);
}


NamespaceDefinition *ParseNodeFactory::CreateNamespaceDefinition(
	const Token *name,
	ExternalDeclaration *declarationList)
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


FunctionDefinition *ParseNodeFactory::CreateFunctionDefinition(FunctionPrototype *prototype)
{
	return new (mAllocator.Alloc<FunctionDefinition>()) FunctionDefinition(prototype);
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


QualifiedIdentifier *ParseNodeFactory::CreateQualifiedIdentifier(const Token *name)
{
	return new (mAllocator.Alloc<QualifiedIdentifier>()) QualifiedIdentifier(name);
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
