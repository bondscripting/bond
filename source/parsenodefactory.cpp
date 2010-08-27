#include "bond/parsenodefactory.h"
#include "bond/parsenodes.h"
#include <new>

namespace Bond
{

TranslationUnit *ParseNodeFactory::CreateTranslationUnit(ExternalDeclaration *declarations)
{
	return new (Alloc<TranslationUnit>()) TranslationUnit(declarations);
}


NamespaceDefinition *ParseNodeFactory::CreateNamespaceDefinition(
	const Token *name,
	ExternalDeclaration *declarations)
{
	return new (Alloc<NamespaceDefinition>()) NamespaceDefinition(name, declarations);
}


EnumDeclaration *ParseNodeFactory::CreateEnumDeclaration(const Token *name, Enumerator *enumerators)
{
	return new (Alloc<EnumDeclaration>()) EnumDeclaration(name, enumerators);
}


Enumerator *ParseNodeFactory::CreateEnumerator(const Token *name, Expression *value)
{
	return new (Alloc<Enumerator>()) Enumerator(name, value);
}


TypeDescriptor *ParseNodeFactory::CreateTypeDescriptor()
{
	return new (Alloc<TypeDescriptor>()) TypeDescriptor();
}


ConditionalExpression *ParseNodeFactory::CreateConditionalExpression(
	Expression *condition,
	Expression *trueExpression,
	Expression *falseExpression)
{
	return new (Alloc<ConditionalExpression>()) ConditionalExpression(condition, trueExpression, falseExpression);
}


BinaryExpression *ParseNodeFactory::CreateBinaryExpression(const Token *op, Expression *lhs, Expression *rhs)
{
	return new (Alloc<BinaryExpression>()) BinaryExpression(op, lhs, rhs);
}


UnaryExpression *ParseNodeFactory::CreateUnaryExpression(const Token *op, Expression *rhs)
{
	return new (Alloc<UnaryExpression>()) UnaryExpression(op, rhs);
}


CastExpression *ParseNodeFactory::CreateCastExpression(TypeDescriptor *typeDescriptor, Expression *rhs)
{
	return new (Alloc<CastExpression>()) CastExpression(typeDescriptor, rhs);
}


SizeofExpression *ParseNodeFactory::CreateSizeofExpression(TypeDescriptor *typeDescriptor)
{
	return new (Alloc<SizeofExpression>()) SizeofExpression(typeDescriptor);
}


ConstantValue *ParseNodeFactory::CreateConstantValue(const Token *value)
{
	return new (Alloc<ConstantValue>()) ConstantValue(value);
}

}
