#ifndef BOND_PARSENODETRAVERSER_H
#define BOND_PARSENODETRAVERSER_H

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

	virtual void Visit(TranslationUnit *translationUnit);
	virtual void Visit(const TranslationUnit *translationUnit);

	virtual void Visit(IncludeDirective *includeDirective) {}
	virtual void Visit(const IncludeDirective *includeDirective) {}

	virtual void Visit(NamespaceDefinition *namespaceDefinition);
	virtual void Visit(const NamespaceDefinition *namespaceDefinition);

	virtual void Visit(EnumDeclaration *enumDeclaration);
	virtual void Visit(const EnumDeclaration *enumDeclaration);

	virtual void Visit(Enumerator *enumerator);
	virtual void Visit(const Enumerator *enumerator);

	virtual void Visit(StructDeclaration *structDeclaration);
	virtual void Visit(const StructDeclaration *structDeclaration);

	virtual void Visit(FunctionDefinition *functionDefinition);
	virtual void Visit(const FunctionDefinition *functionDefinition);

	virtual void Visit(FunctionPrototype *functionPrototype);
	virtual void Visit(const FunctionPrototype *functionPrototype);

	virtual void Visit(Parameter *parameter);
	virtual void Visit(const Parameter *parameter);

	virtual void Visit(TypeDescriptor *typeDescriptor);
	virtual void Visit(const TypeDescriptor *typeDescriptor);

	virtual void Visit(TypeSpecifier *typeSpecifier);
	virtual void Visit(const TypeSpecifier *typeSpecifier);

	virtual void Visit(NamedInitializer *namedInitializer);
	virtual void Visit(const NamedInitializer *namedInitializer);

	virtual void Visit(Initializer *initializer);
	virtual void Visit(const Initializer *initializer);

	virtual void Visit(QualifiedIdentifier *identifier) {}
	virtual void Visit(const QualifiedIdentifier *identifier) {}

	virtual void Visit(CompoundStatement *compoundStatement);
	virtual void Visit(const CompoundStatement *compoundStatement);

	virtual void Visit(IfStatement *ifStatement);
	virtual void Visit(const IfStatement *ifStatement);

	virtual void Visit(SwitchStatement *switchStatement);
	virtual void Visit(const SwitchStatement *switchStatement);

	virtual void Visit(SwitchSection *switchSection);
	virtual void Visit(const SwitchSection *switchSection);

	virtual void Visit(SwitchLabel *switchLabel);
	virtual void Visit(const SwitchLabel *switchLabel);

	virtual void Visit(WhileStatement *whileStatement);
	virtual void Visit(const WhileStatement *whileStatement);

	virtual void Visit(ForStatement *forStatement);
	virtual void Visit(const ForStatement *forStatement);

	virtual void Visit(JumpStatement *jumpStatement);
	virtual void Visit(const JumpStatement *jumpStatement);

	virtual void Visit(DeclarativeStatement *declarativeStatement);
	virtual void Visit(const DeclarativeStatement *declarativeStatement);

	virtual void Visit(ExpressionStatement *expressionStatement);
	virtual void Visit(const ExpressionStatement *expressionStatement);

	virtual void Visit(ConditionalExpression *conditionalExpression);
	virtual void Visit(const ConditionalExpression *conditionalExpression);

	virtual void Visit(BinaryExpression *binaryExpression);
	virtual void Visit(const BinaryExpression *binaryExpression);

	virtual void Visit(UnaryExpression *unaryExpression);
	virtual void Visit(const UnaryExpression *unaryExpression);

	virtual void Visit(PostfixExpression *postfixExpression);
	virtual void Visit(const PostfixExpression *postfixExpression);

	virtual void Visit(MemberExpression *memberExpression);
	virtual void Visit(const MemberExpression *memberExpression);

	virtual void Visit(ArraySubscriptExpression *arraySubscriptExpression);
	virtual void Visit(const ArraySubscriptExpression *arraySubscriptExpression);

	virtual void Visit(FunctionCallExpression *functionCallExpression);
	virtual void Visit(const FunctionCallExpression *functionCallExpression);

	virtual void Visit(CastExpression *castExpression);
	virtual void Visit(const CastExpression *castExpression);

	virtual void Visit(SizeofExpression *sizeofExpression);
	virtual void Visit(const SizeofExpression *sizeofExpression);

	virtual void Visit(ConstantExpression *constantExpression) {}
	virtual void Visit(const ConstantExpression *constantExpression) {}

	virtual void Visit(IdentifierExpression *identifierExpression);
	virtual void Visit(const IdentifierExpression *identifierExpression);

	virtual void Visit(ThisExpression *thisExpression) {}
	virtual void Visit(const ThisExpression *thisExpression) {}

	virtual void Visit(EmptyExpression *emptyExpression) {}
	virtual void Visit(const EmptyExpression *emptyExpression) {}
};

}

#endif
