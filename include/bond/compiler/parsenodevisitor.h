#ifndef BOND_COMPILER_PARSENODEVISITOR_H
#define BOND_COMPILER_PARSENODEVISITOR_H

#include "bond/compiler/parsenodesfwd.h"

namespace Bond
{

class ParseNodeVisitor
{
public:
	virtual ~ParseNodeVisitor() {}

	virtual void Visit(TranslationUnit *translationUnit) = 0;
	virtual void Visit(const TranslationUnit *translationUnit) = 0;

	virtual void Visit(IncludeDirective *includeDirective) = 0;
	virtual void Visit(const IncludeDirective *includeDirective) = 0;

	virtual void Visit(NamespaceDefinition *namespaceDefinition) = 0;
	virtual void Visit(const NamespaceDefinition *namespaceDefinition) = 0;

	virtual void Visit(NativeBlock *nativeBlock) = 0;
	virtual void Visit(const NativeBlock *nativeBlock) = 0;

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

	virtual void Visit(TranslationUnit *translationUnit) override {}
	virtual void Visit(const TranslationUnit *translationUnit) override {}

	virtual void Visit(IncludeDirective *includeDirective) override {}
	virtual void Visit(const IncludeDirective *includeDirective) override {}

	virtual void Visit(NamespaceDefinition *namespaceDefinition) override {}
	virtual void Visit(const NamespaceDefinition *namespaceDefinition) override {}

	virtual void Visit(NativeBlock *nativeBlock) override {}
	virtual void Visit(const NativeBlock *nativeBlock) override {}

	virtual void Visit(EnumDeclaration *enumDeclaration) override {}
	virtual void Visit(const EnumDeclaration *enumDeclaration) override {}

	virtual void Visit(Enumerator *enumerator) override {}
	virtual void Visit(const Enumerator *enumerator) override {}

	virtual void Visit(StructDeclaration *structDeclaration) override {}
	virtual void Visit(const StructDeclaration *structDeclaration) override {}

	virtual void Visit(FunctionDefinition *functionDefinition) override {}
	virtual void Visit(const FunctionDefinition *functionDefinition) override {}

	virtual void Visit(FunctionPrototype *functionPrototype) override {}
	virtual void Visit(const FunctionPrototype *functionPrototype) override {}

	virtual void Visit(Parameter *parameter) override {}
	virtual void Visit(const Parameter *parameter) override {}

	virtual void Visit(TypeDescriptor *typeDescriptor) override {}
	virtual void Visit(const TypeDescriptor *typeDescriptor) override {}

	virtual void Visit(TypeSpecifier *typeSpecifier) override {}
	virtual void Visit(const TypeSpecifier *typeSpecifier) override {}

	virtual void Visit(NamedInitializer *namedInitializer) override {}
	virtual void Visit(const NamedInitializer *namedInitializer) override {}

	virtual void Visit(Initializer *initializer) override {}
	virtual void Visit(const Initializer *initializer) override {}

	virtual void Visit(QualifiedIdentifier *identifier) override {}
	virtual void Visit(const QualifiedIdentifier *identifier) override {}

	virtual void Visit(CompoundStatement *compoundStatement) override {}
	virtual void Visit(const CompoundStatement *compoundStatement) override {}

	virtual void Visit(IfStatement *ifStatement) override {}
	virtual void Visit(const IfStatement *ifStatement) override {}

	virtual void Visit(SwitchStatement *switchStatement) override {}
	virtual void Visit(const SwitchStatement *switchStatement) override {}

	virtual void Visit(SwitchSection *switchSection) override {}
	virtual void Visit(const SwitchSection *switchSection) override {}

	virtual void Visit(SwitchLabel *switchLabel) override {}
	virtual void Visit(const SwitchLabel *switchLabel) override {}

	virtual void Visit(WhileStatement *whileStatement) override {}
	virtual void Visit(const WhileStatement *whileStatement) override {}

	virtual void Visit(ForStatement *forStatement) override {}
	virtual void Visit(const ForStatement *forStatement) override {}

	virtual void Visit(JumpStatement *jumpStatement) override {}
	virtual void Visit(const JumpStatement *jumpStatement) override {}

	virtual void Visit(DeclarativeStatement *declarativeStatement) override {}
	virtual void Visit(const DeclarativeStatement *declarativeStatement) override {}

	virtual void Visit(ExpressionStatement *expressionStatement) override {}
	virtual void Visit(const ExpressionStatement *expressionStatement) override {}

	virtual void Visit(ConditionalExpression *conditionalExpression) override {}
	virtual void Visit(const ConditionalExpression *conditionalExpression) override {}

	virtual void Visit(BinaryExpression *binaryExpression) override {}
	virtual void Visit(const BinaryExpression *binaryExpression) override {}

	virtual void Visit(UnaryExpression *unaryExpression) override {}
	virtual void Visit(const UnaryExpression *unaryExpression) override {}

	virtual void Visit(PostfixExpression *postfixExpression) override {}
	virtual void Visit(const PostfixExpression *postfixExpression) override {}

	virtual void Visit(MemberExpression *memberExpression) override {}
	virtual void Visit(const MemberExpression *memberExpression) override {}

	virtual void Visit(ArraySubscriptExpression *arraySubscriptExpression) override {}
	virtual void Visit(const ArraySubscriptExpression *arraySubscriptExpression) override {}

	virtual void Visit(FunctionCallExpression *functionCallExpression) override {}
	virtual void Visit(const FunctionCallExpression *functionCallExpression) override {}

	virtual void Visit(CastExpression *castExpression) override {}
	virtual void Visit(const CastExpression *castExpression) override {}

	virtual void Visit(SizeofExpression *sizeofExpression) override {}
	virtual void Visit(const SizeofExpression *sizeofExpression) override {}

	virtual void Visit(ConstantExpression *constantExpression) override {}
	virtual void Visit(const ConstantExpression *constantExpression) override {}

	virtual void Visit(IdentifierExpression *identifierExpression) override {}
	virtual void Visit(const IdentifierExpression *identifierExpression) override {}

	virtual void Visit(ThisExpression *thisExpression) override {}
	virtual void Visit(const ThisExpression *thisExpression) override {}

	virtual void Visit(EmptyExpression *emptyExpression) override {}
	virtual void Visit(const EmptyExpression *emptyExpression) override {}
};

}

#endif
