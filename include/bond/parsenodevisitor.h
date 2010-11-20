#ifndef BOND_PARSENODEVISITOR_H
#define BOND_PARSENODEVISITOR_H

#include "bond/parsenodesfwd.h"

namespace Bond
{

class ParseNodeVisitor
{
public:
	virtual ~ParseNodeVisitor() {}

	virtual void Visit(TranslationUnit *translationUnit) = 0;
	virtual void Visit(NamespaceDefinition *namespaceDefinition) = 0;
	virtual void Visit(EnumDeclaration *enumDeclaration) = 0;
	virtual void Visit(Enumerator *enumerator) = 0;
	virtual void Visit(StructDeclaration *structDeclaration) = 0;
	virtual void Visit(FunctionDefinition *functionDefinition) = 0;
	virtual void Visit(FunctionPrototype *functionPrototype) = 0;
	virtual void Visit(Parameter *parameter) = 0;
	virtual void Visit(TypeDescriptor *typeDescriptor) = 0;
	virtual void Visit(TypeSpecifier *typeSpecifier) = 0;
	virtual void Visit(NamedInitializer *namedInitializer) = 0;
	virtual void Visit(Initializer *initializer) = 0;
	virtual void Visit(QualifiedIdentifier *identifier) = 0;
	virtual void Visit(CompoundStatement *compoundStatement) = 0;
	virtual void Visit(IfStatement *ifStatement) = 0;
	virtual void Visit(SwitchStatement *switchStatement) = 0;
	virtual void Visit(SwitchSection *switchSection) = 0;
	virtual void Visit(SwitchLabel *switchLabel) = 0;
	virtual void Visit(WhileStatement *whileStatement) = 0;
	virtual void Visit(ForStatement *forStatement) = 0;
	virtual void Visit(JumpStatement *jumpStatement) = 0;
	virtual void Visit(DeclarativeStatement *declarativeStatement) = 0;
	virtual void Visit(ExpressionStatement *expressionStatement) = 0;
	virtual void Visit(ConditionalExpression *conditionalExpression) = 0;
	virtual void Visit(BinaryExpression *binaryExpression) = 0;
	virtual void Visit(UnaryExpression *unaryExpression) = 0;
	virtual void Visit(PostfixExpression *postfixExpression) = 0;
	virtual void Visit(MemberExpression *memberExpression) = 0;
	virtual void Visit(ArraySubscriptExpression *arraySubscriptExpression) = 0;
	virtual void Visit(FunctionCallExpression *functionCallExpression) = 0;
	virtual void Visit(CastExpression *castExpression) = 0;
	virtual void Visit(SizeofExpression *sizeofExpression) = 0;
	virtual void Visit(ConstantExpression *constantExpression) = 0;
	virtual void Visit(IdentifierExpression *identifierValue) = 0;
};


class ConstParseNodeVisitor
{
public:
	virtual ~ConstParseNodeVisitor() {}

	virtual void Visit(const TranslationUnit *translationUnit) = 0;
	virtual void Visit(const NamespaceDefinition *namespaceDefinition) = 0;
	virtual void Visit(const EnumDeclaration *enumDeclaration) = 0;
	virtual void Visit(const Enumerator *enumerator) = 0;
	virtual void Visit(const StructDeclaration *structDeclaration) = 0;
	virtual void Visit(const FunctionDefinition *functionDefinition) = 0;
	virtual void Visit(const FunctionPrototype *functionPrototype) = 0;
	virtual void Visit(const Parameter *parameter) = 0;
	virtual void Visit(const TypeDescriptor *typeDescriptor) = 0;
	virtual void Visit(const TypeSpecifier *typeSpecifier) = 0;
	virtual void Visit(const NamedInitializer *namedInitializer) = 0;
	virtual void Visit(const Initializer *initializer) = 0;
	virtual void Visit(const QualifiedIdentifier *identifier) = 0;
	virtual void Visit(const CompoundStatement *compoundStatement) = 0;
	virtual void Visit(const IfStatement *ifStatement) = 0;
	virtual void Visit(const SwitchStatement *switchStatement) = 0;
	virtual void Visit(const SwitchSection *switchSection) = 0;
	virtual void Visit(const SwitchLabel *switchLabel) = 0;
	virtual void Visit(const WhileStatement *whileStatement) = 0;
	virtual void Visit(const ForStatement *forStatement) = 0;
	virtual void Visit(const JumpStatement *jumpStatement) = 0;
	virtual void Visit(const DeclarativeStatement *declarativeStatement) = 0;
	virtual void Visit(const ExpressionStatement *expressionStatement) = 0;
	virtual void Visit(const ConditionalExpression *conditionalExpression) = 0;
	virtual void Visit(const BinaryExpression *binaryExpression) = 0;
	virtual void Visit(const UnaryExpression *unaryExpression) = 0;
	virtual void Visit(const PostfixExpression *postfixExpression) = 0;
	virtual void Visit(const MemberExpression *memberExpression) = 0;
	virtual void Visit(const ArraySubscriptExpression *arraySubscriptExpression) = 0;
	virtual void Visit(const FunctionCallExpression *functionCallExpression) = 0;
	virtual void Visit(const CastExpression *castExpression) = 0;
	virtual void Visit(const SizeofExpression *sizeofExpression) = 0;
	virtual void Visit(const ConstantExpression *constantExpression) = 0;
	virtual void Visit(const IdentifierExpression *identifierValue) = 0;
};

}

#endif
