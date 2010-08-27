#ifndef BOND_PARSENODEVISITOR_H
#define BOND_PARSENODEVISITOR_H

#include "bond/parsenodesfwd.h"

namespace Bond
{

class ParseNodeVisitor
{
public:
	virtual ~ParseNodeVisitor() {}

	virtual void VisitTranslationUnit(TranslationUnit *translationUnit) = 0;
	virtual void VisitNamespaceDefinition(NamespaceDefinition *namespaceDefinition) = 0;
	virtual void VisitEnumDeclaration(EnumDeclaration *enumDeclaration) = 0;
	virtual void VisitEnumerator(Enumerator *enumerator) = 0;
	virtual void VisitTypeDescriptor(TypeDescriptor *typeDescriptor) = 0;
	virtual void VisitConditionalExpression(ConditionalExpression *conditionalExpression) = 0;
	virtual void VisitBinaryExpression(BinaryExpression *binaryExpression) = 0;
	virtual void VisitUnaryExpression(UnaryExpression *unaryExpression) = 0;
	virtual void VisitCastExpression(CastExpression *castExpression) = 0;
	virtual void VisitSizeofExpression(SizeofExpression *sizeofExpression) = 0;
	virtual void VisitConstantValue(ConstantValue *constantValue) = 0;
};


class ParseNodeVisitorAdapter: ParseNodeVisitor
{
public:
	virtual ~ParseNodeVisitorAdapter() {}

	virtual void VisitTranslationUnit(TranslationUnit *translationUnit) {}
	virtual void VisitNamespaceDefinition(NamespaceDefinition *namespaceDefinition) {}
	virtual void VisitEnumDeclaration(EnumDeclaration *enumDeclaration) {}
	virtual void VisitEnumerator(Enumerator *enumerator) {}
	virtual void VisitTypeDescriptor(TypeDescriptor *typeDescriptor) {}
	virtual void VisitConditionalExpression(ConditionalExpression *conditionalExpression) {}
	virtual void VisitBinaryExpression(BinaryExpression *binaryExpression) {}
	virtual void VisitUnaryExpression(UnaryExpression *unaryExpression) {}
	virtual void VisitCastExpression(CastExpression *castExpression) {}
	virtual void VisitSizeofExpression(SizeofExpression *sizeofExpression) {}
	virtual void VisitConstantValue(ConstantValue *constantValue) {}
};

}

#endif
