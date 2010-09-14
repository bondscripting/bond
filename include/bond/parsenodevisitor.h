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
	virtual void VisitFunctionDefinition(FunctionDefinition *functionDefinition) = 0;
	virtual void VisitFunctionPrototype(FunctionPrototype *functionPrototype) = 0;
	virtual void VisitParameter(Parameter *parameter) = 0;
	virtual void VisitTypeDescriptor(TypeDescriptor *typeDescriptor) = 0;
	virtual void VisitTypeSpecifier(TypeSpecifier *typeSpecifier) = 0;
	virtual void VisitQualifiedIdentifier(QualifiedIdentifier *identifier) = 0;
	virtual void VisitConditionalExpression(ConditionalExpression *conditionalExpression) = 0;
	virtual void VisitBinaryExpression(BinaryExpression *binaryExpression) = 0;
	virtual void VisitUnaryExpression(UnaryExpression *unaryExpression) = 0;
	virtual void VisitPostfixExpression(PostfixExpression *postfixExpression) = 0;
	virtual void VisitMemberExpression(MemberExpression *memberExpression) = 0;
	virtual void VisitArraySubscriptExpression(ArraySubscriptExpression *arraySubscriptExpression) = 0;
	virtual void VisitFunctionCallExpression(FunctionCallExpression *functionCallExpression) = 0;
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
	virtual void VisitFunctionDefinition(const FunctionDefinition *functionDefinition) = 0;
	virtual void VisitFunctionPrototype(const FunctionPrototype *functionPrototype) = 0;
	virtual void VisitParameter(const Parameter *parameter) = 0;
	virtual void VisitTypeDescriptor(const TypeDescriptor *typeDescriptor) = 0;
	virtual void VisitTypeSpecifier(const TypeSpecifier *typeSpecifier) = 0;
	virtual void VisitQualifiedIdentifier(const QualifiedIdentifier *identifier) = 0;
	virtual void VisitConditionalExpression(const ConditionalExpression *conditionalExpression) = 0;
	virtual void VisitBinaryExpression(const BinaryExpression *binaryExpression) = 0;
	virtual void VisitUnaryExpression(const UnaryExpression *unaryExpression) = 0;
	virtual void VisitPostfixExpression(const PostfixExpression *postfixExpression) = 0;
	virtual void VisitMemberExpression(const MemberExpression *memberExpression) = 0;
	virtual void VisitArraySubscriptExpression(const ArraySubscriptExpression *arraySubscriptExpression) = 0;
	virtual void VisitFunctionCallExpression(const FunctionCallExpression *functionCallExpression) = 0;
	virtual void VisitCastExpression(const CastExpression *castExpression) = 0;
	virtual void VisitSizeofExpression(const SizeofExpression *sizeofExpression) = 0;
	virtual void VisitConstantExpression(const ConstantExpression *constantExpression) = 0;
	virtual void VisitIdentifierExpression(const IdentifierExpression *identifierValue) = 0;
};

}

#endif
