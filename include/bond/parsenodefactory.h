#ifndef BOND_PARSENODEFACTORY_H
#define BOND_PARSENODEFACTORY_H

#include "bond/allocator.h"
#include "bond/parsenodesfwd.h"
#include "bond/parsenodevisitor.h"
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
	ConstantValue *CreateConstantValue(const Token *value);

private:
	//template <typename T> T *Alloc() { return static_cast<T *>(mAllocator.Alloc(sizeof(T))); }
	template <typename T> void *Alloc() { return mAllocator.Alloc(sizeof(T)); }

	Allocator &mAllocator;
};

}

#endif
