#include "bond/parsenodefactory.h"
#include "bond/parsenodes.h"
#include <new>

namespace Bond
{

//------------------------------------------------------------------------------
// ParseNodeDeallocator
//------------------------------------------------------------------------------

class ParseNodeDeallocator: ParseNodeVisitor
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
	virtual void VisitTypeDescriptor(TypeDescriptor *typeDescriptor);
	virtual void VisitConditionalExpression(ConditionalExpression *conditionalExpression);
	virtual void VisitBinaryExpression(BinaryExpression *binaryExpression);
	virtual void VisitUnaryExpression(UnaryExpression *unaryExpression);
	virtual void VisitCastExpression(CastExpression *castExpression);
	virtual void VisitSizeofExpression(SizeofExpression *sizeofExpression);
	virtual void VisitConstantValue(ConstantValue *constantValue);

private:
	void DestroyExternalDeclarationList(ExternalDeclaration *declarationList);
	void DestroyEnumeratorList(Enumerator *enumeratorList);

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


void ParseNodeDeallocator::VisitTypeDescriptor(TypeDescriptor *typeDescriptor)
{
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


void ParseNodeDeallocator::VisitCastExpression(CastExpression *castExpression)
{
	Destroy(castExpression->GetRhs());
}


void ParseNodeDeallocator::VisitSizeofExpression(SizeofExpression *sizeofExpression)
{
	Destroy(sizeofExpression->GetRhs());
}


void ParseNodeDeallocator::VisitConstantValue(ConstantValue *constantValue)
{
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


//------------------------------------------------------------------------------
// ParseNodeFactory
//------------------------------------------------------------------------------

TranslationUnit *ParseNodeFactory::CreateTranslationUnit(ExternalDeclaration *declarations)
{
	return new (mAllocator.Alloc<TranslationUnit>()) TranslationUnit(declarations);
}


NamespaceDefinition *ParseNodeFactory::CreateNamespaceDefinition(
	const Token *name,
	ExternalDeclaration *declarations)
{
	return new (mAllocator.Alloc<NamespaceDefinition>()) NamespaceDefinition(name, declarations);
}


EnumDeclaration *ParseNodeFactory::CreateEnumDeclaration(const Token *name, Enumerator *enumerators)
{
	return new (mAllocator.Alloc<EnumDeclaration>()) EnumDeclaration(name, enumerators);
}


Enumerator *ParseNodeFactory::CreateEnumerator(const Token *name, Expression *value)
{
	return new (mAllocator.Alloc<Enumerator>()) Enumerator(name, value);
}


TypeDescriptor *ParseNodeFactory::CreateTypeDescriptor()
{
	return new (mAllocator.Alloc<TypeDescriptor>()) TypeDescriptor();
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


ConstantValue *ParseNodeFactory::CreateConstantValue(const Token *value)
{
	return new (mAllocator.Alloc<ConstantValue>()) ConstantValue(value);
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
