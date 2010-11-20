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

	virtual void Visit(const TranslationUnit *translationUnit);
	virtual void Visit(const NamespaceDefinition *namespaceDefinition);
	virtual void Visit(const EnumDeclaration *enumDeclaration);
	virtual void Visit(const Enumerator *enumerator);
	virtual void Visit(const StructDeclaration *structDeclaration);
	virtual void Visit(const FunctionDefinition *functionDefinition);
	virtual void Visit(const FunctionPrototype *functionPrototype);
	virtual void Visit(const Parameter *parameter);
	virtual void Visit(const TypeDescriptor *typeDescriptor);
	virtual void Visit(const TypeSpecifier *typeSpecifier);
	virtual void Visit(const NamedInitializer *namedInitializer);
	virtual void Visit(const Initializer *initializer);
	virtual void Visit(const QualifiedIdentifier *identifier);
	virtual void Visit(const CompoundStatement *compoundStatement);
	virtual void Visit(const IfStatement *ifStatement);
	virtual void Visit(const SwitchStatement *switchStatement);
	virtual void Visit(const SwitchSection *switchSection);
	virtual void Visit(const SwitchLabel *switchLabel);
	virtual void Visit(const WhileStatement *whileStatement);
	virtual void Visit(const ForStatement *forStatement);
	virtual void Visit(const JumpStatement *jumpStatement);
	virtual void Visit(const DeclarativeStatement *declarativeStatement);
	virtual void Visit(const ExpressionStatement *expressionStatement);
	virtual void Visit(const ConditionalExpression *conditionalExpression);
	virtual void Visit(const BinaryExpression *binaryExpression);
	virtual void Visit(const UnaryExpression *unaryExpression);
	virtual void Visit(const PostfixExpression *postfixExpression);
	virtual void Visit(const MemberExpression *memberExpression);
	virtual void Visit(const ArraySubscriptExpression *arraySubscriptExpression);
	virtual void Visit(const FunctionCallExpression *functionCallExpression);
	virtual void Visit(const CastExpression *castExpression);
	virtual void Visit(const SizeofExpression *sizeofExpression);
	virtual void Visit(const ConstantExpression *constantExpression);
	virtual void Visit(const IdentifierExpression *identifierValue);

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
