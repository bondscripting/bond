#ifndef BOND_PARSENODEVISITOR_H
#define BOND_PARSENODEVISITOR_H

#include "bond/parsenodes.h"

namespace Bond
{

class ParseNodeVisitor
{
public:
	virtual ~ParseNodeVisitor() {}

	void VisitTranslationUnit(TranslationUnit *translationUnit) = 0;
	void VisitExternalDeclaration(ExternalDeclaration *externalDeclaration) = 0;
	void VisitNamespaceDefinition(NamespaceDefinition *namespaceDefinition) = 0;
	void VisitEnumDeclaration(EnumDeclaration *enumDeclaration) = 0;
	void VisitEnumerator(Enumerator *enumerator) = 0;
	void VisitTypeDescriptor(TypeDescriptor *typeDescriptor) = 0;
	void VisitExpression(Expression *expression) = 0;
	void VisitConditionalExpression(ConditionalExpression *conditionalExpression) = 0;
	void VisitBinaryExpression(BinaryExpression *binaryExpression) = 0;
	void VisitUnaryExpression(UnaryExpression *unaryExpression) = 0;
	void VisitCastExpression(CastExpression *castExpression) = 0;
	void VisitSizeofExpression(SizeofExpression *sizeofExpression) = 0;
	void VisitConstantValue(ConstantValue *constantValue) = 0;
};


class ParseNodeVisitorAdapter: ParseNodeVisitor
{
public:
	virtual ~ParseNodeVisitorAdapter() {}

	void VisitTranslationUnit(TranslationUnit *translationUnit) {}
	void VisitExternalDeclaration(ExternalDeclaration *externalDeclaration) {}
	void VisitNamespaceDefinition(NamespaceDefinition *namespaceDefinition) {}
	void VisitEnumDeclaration(EnumDeclaration *enumDeclaration) {}
	void VisitEnumerator(Enumerator *enumerator) {}
	void VisitTypeDescriptor(TypeDescriptor *typeDescriptor) {}
	void VisitExpression(Expression *expression) {}
	void VisitConditionalExpression(ConditionalExpression *conditionalExpression) {}
	void VisitBinaryExpression(BinaryExpression *binaryExpression) {}
	void VisitUnaryExpression(UnaryExpression *unaryExpression) {}
	void VisitCastExpression(CastExpression *castExpression) {}
	void VisitSizeofExpression(SizeofExpression *sizeofExpression) {}
	void VisitConstantValue(ConstantValue *constantValue) {}
};

}

#endif
