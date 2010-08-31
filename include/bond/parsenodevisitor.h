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
	virtual void VisitQualifiedIdentifier(QualifiedIdentifier *identifier) = 0;
	virtual void VisitConditionalExpression(ConditionalExpression *conditionalExpression) = 0;
	virtual void VisitBinaryExpression(BinaryExpression *binaryExpression) = 0;
	virtual void VisitUnaryExpression(UnaryExpression *unaryExpression) = 0;
	virtual void VisitCastExpression(CastExpression *castExpression) = 0;
	virtual void VisitSizeofExpression(SizeofExpression *sizeofExpression) = 0;
	virtual void VisitConstantExpression(ConstantExpression *constantExpression) = 0;
	virtual void VisitIdentifierExpression(IdentifierExpression *identifierValue) = 0;
};


class ConstParseNodeVisitor
{
public:
	virtual ~ConstParseNodeVisitor() {}

	virtual void VisitTranslationUnit(const TranslationUnit *translationUnit) = 0;
	virtual void VisitNamespaceDefinition(const NamespaceDefinition *namespaceDefinition) = 0;
	virtual void VisitEnumDeclaration(const EnumDeclaration *enumDeclaration) = 0;
	virtual void VisitEnumerator(const Enumerator *enumerator) = 0;
	virtual void VisitTypeDescriptor(const TypeDescriptor *typeDescriptor) = 0;
	virtual void VisitQualifiedIdentifier(const QualifiedIdentifier *identifier) = 0;
	virtual void VisitConditionalExpression(const ConditionalExpression *conditionalExpression) = 0;
	virtual void VisitBinaryExpression(const BinaryExpression *binaryExpression) = 0;
	virtual void VisitUnaryExpression(const UnaryExpression *unaryExpression) = 0;
	virtual void VisitCastExpression(const CastExpression *castExpression) = 0;
	virtual void VisitSizeofExpression(const SizeofExpression *sizeofExpression) = 0;
	virtual void VisitConstantExpression(const ConstantExpression *constantExpression) = 0;
	virtual void VisitIdentifierExpression(const IdentifierExpression *identifierValue) = 0;
};

}

#endif
