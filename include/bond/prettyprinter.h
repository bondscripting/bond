#ifndef BOND_PRETTYPRINTER_H
#define BOND_PRETTYPRINTER_H

#include "bond/parsenodevisitor.h"

namespace Bond
{

class Token;

class PrettyPrinter: public ConstParseNodeVisitor
{
public:
	PrettyPrinter():
		mTabLevel(0)
	{}

	virtual ~PrettyPrinter() {}

	void Print(const ParseNode *parseNode);

	virtual void VisitTranslationUnit(const TranslationUnit *translationUnit);
	virtual void VisitNamespaceDefinition(const NamespaceDefinition *namespaceDefinition);
	virtual void VisitEnumDeclaration(const EnumDeclaration *enumDeclaration);
	virtual void VisitEnumerator(const Enumerator *enumerator);
	virtual void VisitFunctionDefinition(const FunctionDefinition *functionDefinition);
	virtual void VisitFunctionPrototype(const FunctionPrototype *functionPrototype);
	virtual void VisitParameter(const Parameter *parameter);
	virtual void VisitTypeDescriptor(const TypeDescriptor *typeDescriptor);
	virtual void VisitTypeSpecifier(const TypeSpecifier *typeSpecifier);
	virtual void VisitQualifiedIdentifier(const QualifiedIdentifier *identifier);
	virtual void VisitConditionalExpression(const ConditionalExpression *conditionalExpression);
	virtual void VisitBinaryExpression(const BinaryExpression *binaryExpression);
	virtual void VisitUnaryExpression(const UnaryExpression *unaryExpression);
	virtual void VisitPostfixExpression(const PostfixExpression *postfixExpression);
	virtual void VisitMemberExpression(const MemberExpression *memberExpression);
	virtual void VisitArraySubscriptExpression(const ArraySubscriptExpression *arraySubscriptExpression);
	virtual void VisitFunctionCallExpression(const FunctionCallExpression *functionCallExpression);
	virtual void VisitCastExpression(const CastExpression *castExpression);
	virtual void VisitSizeofExpression(const SizeofExpression *sizeofExpression);
	virtual void VisitConstantExpression(const ConstantExpression *constantExpression);
	virtual void VisitIdentifierExpression(const IdentifierExpression *identifierValue);

private:
	void PrintExternalDeclarationList(const ExternalDeclaration *declarationList);
	void PrintEnumeratorList(const Enumerator *enumeratorList);
	void PrintParameterList(const Parameter *parameterList);
	void PrintArgumentList(const Expression *argumentList);
	void PrintQualifiedIdentifier(const QualifiedIdentifier *identifier);

	void IncrementTab() { ++mTabLevel; }
	void DecrementTab() { --mTabLevel; }
	void Tab();
	void Print(const char *text);
	void Print(const Token *token);

	int mTabLevel;
};

}

#endif
