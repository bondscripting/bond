#ifndef BOND_PARSENODETRAVERSER_H
#define BOND_PARSENODETRAVERSER_H

#include "bond/parsenodevisitor.h"

namespace Bond
{

class ParseNodeTraverser : public ParseNodeVisitor
{
public:
	virtual ~ParseNodeTraverser() {}

	virtual void Traverse(ParseNode *parseNode);
	virtual void TraverseList(ListParseNode *listNode);

	virtual void VisitTranslationUnit(TranslationUnit *translationUnit);
	virtual void VisitNamespaceDefinition(NamespaceDefinition *namespaceDefinition);
	virtual void VisitEnumDeclaration(EnumDeclaration *enumDeclaration);
	virtual void VisitEnumerator(Enumerator *enumerator);
	virtual void VisitStructDeclaration(StructDeclaration *structDeclaration);
	virtual void VisitFunctionDefinition(FunctionDefinition *functionDefinition);
	virtual void VisitFunctionPrototype(FunctionPrototype *functionPrototype);
	virtual void VisitParameter(Parameter *parameter);
	virtual void VisitTypeDescriptor(TypeDescriptor *typeDescriptor);
	virtual void VisitTypeSpecifier(TypeSpecifier *typeSpecifier);
	virtual void VisitNamedInitializer(NamedInitializer *namedInitializer);
	virtual void VisitInitializer(Initializer *initializer);
	virtual void VisitQualifiedIdentifier(QualifiedIdentifier *identifier) {}
	virtual void VisitCompoundStatement(CompoundStatement *compoundStatement);
	virtual void VisitIfStatement(IfStatement *ifStatement);
	virtual void VisitSwitchStatement(SwitchStatement *switchStatement);
	virtual void VisitSwitchSection(SwitchSection *switchSection);
	virtual void VisitSwitchLabel(SwitchLabel *switchLabel);
	virtual void VisitWhileStatement(WhileStatement *whileStatement);
	virtual void VisitForStatement(ForStatement *forStatement);
	virtual void VisitJumpStatement(JumpStatement *jumpStatement);
	virtual void VisitDeclarativeStatement(DeclarativeStatement *declarativeStatement);
	virtual void VisitExpressionStatement(ExpressionStatement *expressionStatement);
	virtual void VisitConditionalExpression(ConditionalExpression *conditionalExpression);
	virtual void VisitBinaryExpression(BinaryExpression *binaryExpression);
	virtual void VisitUnaryExpression(UnaryExpression *unaryExpression);
	virtual void VisitPostfixExpression(PostfixExpression *postfixExpression);
	virtual void VisitMemberExpression(MemberExpression *memberExpression);
	virtual void VisitArraySubscriptExpression(ArraySubscriptExpression *arraySubscriptExpression);
	virtual void VisitFunctionCallExpression(FunctionCallExpression *functionCallExpression);
	virtual void VisitCastExpression(CastExpression *castExpression);
	virtual void VisitSizeofExpression(SizeofExpression *sizeofExpression);
	virtual void VisitConstantExpression(ConstantExpression *constantExpression) {}
	virtual void VisitIdentifierExpression(IdentifierExpression *identifierValue);
};


class ConstParseNodeTraverser : public ConstParseNodeVisitor
{
public:
	virtual ~ConstParseNodeTraverser() {}

	virtual void Traverse(const ParseNode *parseNode);
	virtual void TraverseList(const ListParseNode *listNode);

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
	virtual void VisitQualifiedIdentifier(const QualifiedIdentifier *identifier) {}
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
	virtual void VisitConstantExpression(const ConstantExpression *constantExpression) {}
	virtual void VisitIdentifierExpression(const IdentifierExpression *identifierValue);
};

}

#endif
