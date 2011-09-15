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
	virtual void Visit(const TranslationUnit *translationUnit) = 0;

	virtual void Visit(NamespaceDefinition *namespaceDefinition) = 0;
	virtual void Visit(const NamespaceDefinition *namespaceDefinition) = 0;

	virtual void Visit(EnumDeclaration *enumDeclaration) = 0;
	virtual void Visit(const EnumDeclaration *enumDeclaration) = 0;

	virtual void Visit(Enumerator *enumerator) = 0;
	virtual void Visit(const Enumerator *enumerator) = 0;

	virtual void Visit(StructDeclaration *structDeclaration) = 0;
	virtual void Visit(const StructDeclaration *structDeclaration) = 0;

	virtual void Visit(FunctionDefinition *functionDefinition) = 0;
	virtual void Visit(const FunctionDefinition *functionDefinition) = 0;

	virtual void Visit(FunctionPrototype *functionPrototype) = 0;
	virtual void Visit(const FunctionPrototype *functionPrototype) = 0;

	virtual void Visit(Parameter *parameter) = 0;
	virtual void Visit(const Parameter *parameter) = 0;

	virtual void Visit(TypeDescriptor *typeDescriptor) = 0;
	virtual void Visit(const TypeDescriptor *typeDescriptor) = 0;

	virtual void Visit(TypeSpecifier *typeSpecifier) = 0;
	virtual void Visit(const TypeSpecifier *typeSpecifier) = 0;

	virtual void Visit(NamedInitializer *namedInitializer) = 0;
	virtual void Visit(const NamedInitializer *namedInitializer) = 0;

	virtual void Visit(Initializer *initializer) = 0;
	virtual void Visit(const Initializer *initializer) = 0;

	virtual void Visit(QualifiedIdentifier *identifier) = 0;
	virtual void Visit(const QualifiedIdentifier *identifier) = 0;

	virtual void Visit(CompoundStatement *compoundStatement) = 0;
	virtual void Visit(const CompoundStatement *compoundStatement) = 0;

	virtual void Visit(IfStatement *ifStatement) = 0;
	virtual void Visit(const IfStatement *ifStatement) = 0;

	virtual void Visit(SwitchStatement *switchStatement) = 0;
	virtual void Visit(const SwitchStatement *switchStatement) = 0;

	virtual void Visit(SwitchSection *switchSection) = 0;
	virtual void Visit(const SwitchSection *switchSection) = 0;

	virtual void Visit(SwitchLabel *switchLabel) = 0;
	virtual void Visit(const SwitchLabel *switchLabel) = 0;

	virtual void Visit(WhileStatement *whileStatement) = 0;
	virtual void Visit(const WhileStatement *whileStatement) = 0;

	virtual void Visit(ForStatement *forStatement) = 0;
	virtual void Visit(const ForStatement *forStatement) = 0;

	virtual void Visit(JumpStatement *jumpStatement) = 0;
	virtual void Visit(const JumpStatement *jumpStatement) = 0;

	virtual void Visit(DeclarativeStatement *declarativeStatement) = 0;
	virtual void Visit(const DeclarativeStatement *declarativeStatement) = 0;

	virtual void Visit(ExpressionStatement *expressionStatement) = 0;
	virtual void Visit(const ExpressionStatement *expressionStatement) = 0;

	virtual void Visit(ConditionalExpression *conditionalExpression) = 0;
	virtual void Visit(const ConditionalExpression *conditionalExpression) = 0;

	virtual void Visit(BinaryExpression *binaryExpression) = 0;
	virtual void Visit(const BinaryExpression *binaryExpression) = 0;

	virtual void Visit(UnaryExpression *unaryExpression) = 0;
	virtual void Visit(const UnaryExpression *unaryExpression) = 0;

	virtual void Visit(PostfixExpression *postfixExpression) = 0;
	virtual void Visit(const PostfixExpression *postfixExpression) = 0;

	virtual void Visit(MemberExpression *memberExpression) = 0;
	virtual void Visit(const MemberExpression *memberExpression) = 0;

	virtual void Visit(ArraySubscriptExpression *arraySubscriptExpression) = 0;
	virtual void Visit(const ArraySubscriptExpression *arraySubscriptExpression) = 0;

	virtual void Visit(FunctionCallExpression *functionCallExpression) = 0;
	virtual void Visit(const FunctionCallExpression *functionCallExpression) = 0;

	virtual void Visit(CastExpression *castExpression) = 0;
	virtual void Visit(const CastExpression *castExpression) = 0;

	virtual void Visit(SizeofExpression *sizeofExpression) = 0;
	virtual void Visit(const SizeofExpression *sizeofExpression) = 0;

	virtual void Visit(ConstantExpression *constantExpression) = 0;
	virtual void Visit(const ConstantExpression *constantExpression) = 0;

	virtual void Visit(IdentifierExpression *identifierExpression) = 0;
	virtual void Visit(const IdentifierExpression *identifierExpression) = 0;

	virtual void Visit(ThisExpression *thisExpression) = 0;
	virtual void Visit(const ThisExpression *thisExpression) = 0;

	virtual void Visit(EmptyExpression *emptyExpression) = 0;
	virtual void Visit(const EmptyExpression *emptyExpression) = 0;
};


class ParseNodeVisitorAdapter: public ParseNodeVisitor
{
public:
	virtual ~ParseNodeVisitorAdapter() {}

	virtual void Visit(TranslationUnit *translationUnit) {}
	virtual void Visit(const TranslationUnit *translationUnit) {}

	virtual void Visit(NamespaceDefinition *namespaceDefinition) {}
	virtual void Visit(const NamespaceDefinition *namespaceDefinition) {}

	virtual void Visit(EnumDeclaration *enumDeclaration) {}
	virtual void Visit(const EnumDeclaration *enumDeclaration) {}

	virtual void Visit(Enumerator *enumerator) {}
	virtual void Visit(const Enumerator *enumerator) {}

	virtual void Visit(StructDeclaration *structDeclaration) {}
	virtual void Visit(const StructDeclaration *structDeclaration) {}

	virtual void Visit(FunctionDefinition *functionDefinition) {}
	virtual void Visit(const FunctionDefinition *functionDefinition) {}

	virtual void Visit(FunctionPrototype *functionPrototype) {}
	virtual void Visit(const FunctionPrototype *functionPrototype) {}

	virtual void Visit(Parameter *parameter) {}
	virtual void Visit(const Parameter *parameter) {}

	virtual void Visit(TypeDescriptor *typeDescriptor) {}
	virtual void Visit(const TypeDescriptor *typeDescriptor) {}

	virtual void Visit(TypeSpecifier *typeSpecifier) {}
	virtual void Visit(const TypeSpecifier *typeSpecifier) {}

	virtual void Visit(NamedInitializer *namedInitializer) {}
	virtual void Visit(const NamedInitializer *namedInitializer) {}

	virtual void Visit(Initializer *initializer) {}
	virtual void Visit(const Initializer *initializer) {}

	virtual void Visit(QualifiedIdentifier *identifier) {}
	virtual void Visit(const QualifiedIdentifier *identifier) {}

	virtual void Visit(CompoundStatement *compoundStatement) {}
	virtual void Visit(const CompoundStatement *compoundStatement) {}

	virtual void Visit(IfStatement *ifStatement) {}
	virtual void Visit(const IfStatement *ifStatement) {}

	virtual void Visit(SwitchStatement *switchStatement) {}
	virtual void Visit(const SwitchStatement *switchStatement) {}

	virtual void Visit(SwitchSection *switchSection) {}
	virtual void Visit(const SwitchSection *switchSection) {}

	virtual void Visit(SwitchLabel *switchLabel) {}
	virtual void Visit(const SwitchLabel *switchLabel) {}

	virtual void Visit(WhileStatement *whileStatement) {}
	virtual void Visit(const WhileStatement *whileStatement) {}

	virtual void Visit(ForStatement *forStatement) {}
	virtual void Visit(const ForStatement *forStatement) {}

	virtual void Visit(JumpStatement *jumpStatement) {}
	virtual void Visit(const JumpStatement *jumpStatement) {}

	virtual void Visit(DeclarativeStatement *declarativeStatement) {}
	virtual void Visit(const DeclarativeStatement *declarativeStatement) {}

	virtual void Visit(ExpressionStatement *expressionStatement) {}
	virtual void Visit(const ExpressionStatement *expressionStatement) {}

	virtual void Visit(ConditionalExpression *conditionalExpression) {}
	virtual void Visit(const ConditionalExpression *conditionalExpression) {}

	virtual void Visit(BinaryExpression *binaryExpression) {}
	virtual void Visit(const BinaryExpression *binaryExpression) {}

	virtual void Visit(UnaryExpression *unaryExpression) {}
	virtual void Visit(const UnaryExpression *unaryExpression) {}

	virtual void Visit(PostfixExpression *postfixExpression) {}
	virtual void Visit(const PostfixExpression *postfixExpression) {}

	virtual void Visit(MemberExpression *memberExpression) {}
	virtual void Visit(const MemberExpression *memberExpression) {}

	virtual void Visit(ArraySubscriptExpression *arraySubscriptExpression) {}
	virtual void Visit(const ArraySubscriptExpression *arraySubscriptExpression) {}

	virtual void Visit(FunctionCallExpression *functionCallExpression) {}
	virtual void Visit(const FunctionCallExpression *functionCallExpression) {}

	virtual void Visit(CastExpression *castExpression) {}
	virtual void Visit(const CastExpression *castExpression) {}

	virtual void Visit(SizeofExpression *sizeofExpression) {}
	virtual void Visit(const SizeofExpression *sizeofExpression) {}

	virtual void Visit(ConstantExpression *constantExpression) {}
	virtual void Visit(const ConstantExpression *constantExpression) {}

	virtual void Visit(IdentifierExpression *identifierExpression) {}
	virtual void Visit(const IdentifierExpression *identifierExpression) {}

	virtual void Visit(ThisExpression *thisExpression) {}
	virtual void Visit(const ThisExpression *thisExpression) {}

	virtual void Visit(EmptyExpression *emptyExpression) {}
	virtual void Visit(const EmptyExpression *emptyExpression) {}
};

}

#endif
