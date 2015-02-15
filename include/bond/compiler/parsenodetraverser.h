#ifndef BOND_COMPILER_PARSENODETRAVERSER_H
#define BOND_COMPILER_PARSENODETRAVERSER_H

#include "bond/compiler/parsenodevisitor.h"

namespace Bond
{

class ParseNodeTraverser: public ParseNodeVisitor
{
public:
	virtual ~ParseNodeTraverser() {}

	virtual void Traverse(ParseNode *parseNode);
	virtual void Traverse(const ParseNode *parseNode);

	virtual void TraverseList(ListParseNode *listNode);
	virtual void TraverseList(const ListParseNode *listNode);

	virtual void Visit(TranslationUnit *translationUnit) override;
	virtual void Visit(const TranslationUnit *translationUnit) override;

	virtual void Visit(IncludeDirective *includeDirective) override {}
	virtual void Visit(const IncludeDirective *includeDirective) override {}

	virtual void Visit(NamespaceDefinition *namespaceDefinition) override;
	virtual void Visit(const NamespaceDefinition *namespaceDefinition) override;

	virtual void Visit(NativeBlock *nativeBlock) override;
	virtual void Visit(const NativeBlock *nativeBlock) override;

	virtual void Visit(EnumDeclaration *enumDeclaration) override;
	virtual void Visit(const EnumDeclaration *enumDeclaration) override;

	virtual void Visit(Enumerator *enumerator) override;
	virtual void Visit(const Enumerator *enumerator) override;

	virtual void Visit(StructDeclaration *structDeclaration) override;
	virtual void Visit(const StructDeclaration *structDeclaration) override;

	virtual void Visit(FunctionDefinition *functionDefinition) override;
	virtual void Visit(const FunctionDefinition *functionDefinition) override;

	virtual void Visit(FunctionPrototype *functionPrototype) override;
	virtual void Visit(const FunctionPrototype *functionPrototype) override;

	virtual void Visit(Parameter *parameter) override;
	virtual void Visit(const Parameter *parameter) override;

	virtual void Visit(TypeDescriptor *typeDescriptor) override;
	virtual void Visit(const TypeDescriptor *typeDescriptor) override;

	virtual void Visit(TypeSpecifier *typeSpecifier) override;
	virtual void Visit(const TypeSpecifier *typeSpecifier) override;

	virtual void Visit(NamedInitializer *namedInitializer) override;
	virtual void Visit(const NamedInitializer *namedInitializer) override;

	virtual void Visit(Initializer *initializer) override;
	virtual void Visit(const Initializer *initializer) override;

	virtual void Visit(QualifiedIdentifier *identifier) override {}
	virtual void Visit(const QualifiedIdentifier *identifier) override {}

	virtual void Visit(CompoundStatement *compoundStatement) override;
	virtual void Visit(const CompoundStatement *compoundStatement) override;

	virtual void Visit(IfStatement *ifStatement) override;
	virtual void Visit(const IfStatement *ifStatement) override;

	virtual void Visit(SwitchStatement *switchStatement) override;
	virtual void Visit(const SwitchStatement *switchStatement) override;

	virtual void Visit(SwitchSection *switchSection) override;
	virtual void Visit(const SwitchSection *switchSection) override;

	virtual void Visit(SwitchLabel *switchLabel) override;
	virtual void Visit(const SwitchLabel *switchLabel) override;

	virtual void Visit(WhileStatement *whileStatement) override;
	virtual void Visit(const WhileStatement *whileStatement) override;

	virtual void Visit(ForStatement *forStatement) override;
	virtual void Visit(const ForStatement *forStatement) override;

	virtual void Visit(JumpStatement *jumpStatement) override;
	virtual void Visit(const JumpStatement *jumpStatement) override;

	virtual void Visit(DeclarativeStatement *declarativeStatement) override;
	virtual void Visit(const DeclarativeStatement *declarativeStatement) override;

	virtual void Visit(ExpressionStatement *expressionStatement) override;
	virtual void Visit(const ExpressionStatement *expressionStatement) override;

	virtual void Visit(ConditionalExpression *conditionalExpression) override;
	virtual void Visit(const ConditionalExpression *conditionalExpression) override;

	virtual void Visit(BinaryExpression *binaryExpression) override;
	virtual void Visit(const BinaryExpression *binaryExpression) override;

	virtual void Visit(UnaryExpression *unaryExpression) override;
	virtual void Visit(const UnaryExpression *unaryExpression) override;

	virtual void Visit(PostfixExpression *postfixExpression) override;
	virtual void Visit(const PostfixExpression *postfixExpression) override;

	virtual void Visit(MemberExpression *memberExpression) override;
	virtual void Visit(const MemberExpression *memberExpression) override;

	virtual void Visit(ArraySubscriptExpression *arraySubscriptExpression) override;
	virtual void Visit(const ArraySubscriptExpression *arraySubscriptExpression) override;

	virtual void Visit(FunctionCallExpression *functionCallExpression) override;
	virtual void Visit(const FunctionCallExpression *functionCallExpression) override;

	virtual void Visit(CastExpression *castExpression) override;
	virtual void Visit(const CastExpression *castExpression) override;

	virtual void Visit(SizeofExpression *sizeofExpression) override;
	virtual void Visit(const SizeofExpression *sizeofExpression) override;

	virtual void Visit(ConstantLiteralExpression *constantExpression) override {}
	virtual void Visit(const ConstantLiteralExpression *constantExpression) override {}

	virtual void Visit(IdentifierExpression *identifierExpression) override;
	virtual void Visit(const IdentifierExpression *identifierExpression) override;

	virtual void Visit(ThisExpression *thisExpression) override {}
	virtual void Visit(const ThisExpression *thisExpression) override {}

	virtual void Visit(EmptyExpression *emptyExpression) override {}
	virtual void Visit(const EmptyExpression *emptyExpression) override {}
};

}

#endif
