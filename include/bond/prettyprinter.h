#ifndef BOND_PRETTYPRINTER_H
#define BOND_PRETTYPRINTER_H

#include "bond/parsenodevisitor.h"

namespace Bond
{

class TextWriter;
class Token;

class PrettyPrinter: public ConstParseNodeVisitor
{
public:
	PrettyPrinter(TextWriter &writer):
		mWriter(writer),
		mTabLevel(0),
		mPrintTabsAndNewlines(true)
	{}

	virtual ~PrettyPrinter() {}

	void Print(const ParseNode *parseNode);
	void PrintList(const ListParseNode *listNode);

	virtual void VisitTranslationUnit(const TranslationUnit *translationUnit);
	virtual void VisitNamespaceDefinition(const NamespaceDefinition *namespaceDefinition);
	virtual void VisitEnumDeclaration(const EnumDeclaration *enumDeclaration);
	virtual void VisitEnumerator(const Enumerator *enumerator);
	virtual void VisitStructDeclaration(const StructDeclaration *structDeclaration);
	virtual void VisitFunctionDefinition(const FunctionDefinition *functionDefinition);
	virtual void VisitFunctionPrototype(const FunctionPrototype *functionPrototype);
	virtual void VisitParameter(const Parameter *parameter);
	virtual void VisitTypeDescriptor(const TypeDescriptor *typeDescriptor);
	virtual void VisitTypeSpecifier(const TypeSpecifier *typeSpecifier);
	virtual void VisitNamedInitializer(const NamedInitializer *namedInitializer);
	virtual void VisitInitializer(const Initializer *initializer);
	virtual void VisitQualifiedIdentifier(const QualifiedIdentifier *identifier);
	virtual void VisitCompoundStatement(const CompoundStatement *compoundStatement);
	virtual void VisitIfStatement(const IfStatement *ifStatement);
	virtual void VisitSwitchStatement(const SwitchStatement *switchStatement);
	virtual void VisitSwitchSection(const SwitchSection *switchSection);
	virtual void VisitSwitchLabel(const SwitchLabel *switchLabel);
	virtual void VisitWhileStatement(const WhileStatement *whileStatement);
	virtual void VisitForStatement(const ForStatement *forStatement);
	virtual void VisitJumpStatement(const JumpStatement *jumpStatement);
	virtual void VisitDeclarativeStatement(const DeclarativeStatement *declarativeStatement);
	virtual void VisitExpressionStatement(const ExpressionStatement *expressionStatement);
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
	void PrintList(const ListParseNode *listNode, const char *separator);
	void IncrementTab() { ++mTabLevel; }
	void DecrementTab() { --mTabLevel; }
	void Tab();
	void Newline();
	void Print(const Token *token);
	void PrintTabsAndNewlines() { mPrintTabsAndNewlines = true; }
	void SuppressTabsAndNewlines() { mPrintTabsAndNewlines = false; }

	TextWriter &mWriter;
	int mTabLevel;
	bool mPrintTabsAndNewlines;
};

}

#endif
