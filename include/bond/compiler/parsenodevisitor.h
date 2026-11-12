#ifndef BOND_COMPILER_PARSENODEVISITOR_H
#define BOND_COMPILER_PARSENODEVISITOR_H

#include "bond/conf.h"
#include "bond/compiler/parsenodesfwd.h"

namespace Bond
{

/// \addtogroup compiler
/// @{

/// \brief An abstract interface that implements the Visitor pattern over all concrete types
/// that derive from ParseNode.
///
/// Any of the ParseNodeVisitor::Visit methods can be called directly if the concrete type of the
/// ParseNode is known at compile time. If the node's concrete type is not known, its
/// ParseNode::Accept method can be called with the visitor passed in as an argument, which will
/// in turn call the appropriate Visit method.
///
/// \sa ParseNode, ParseNodeVisitorAdapter, ParseNodeTraverser
class BOND_API ParseNodeVisitor
{
public:
	/// \brief Destroys a ParseNodeVisitor object.
	virtual ~ParseNodeVisitor() {}

	/// \brief Visits a TranslationUnit.
	virtual void Visit(TranslationUnit *translationUnit) = 0;

	/// \brief Visits a const TranslationUnit.
	virtual void Visit(const TranslationUnit *translationUnit) = 0;

	/// \brief Visits an IncludeDirective.
	virtual void Visit(IncludeDirective *includeDirective) = 0;

	/// \brief Visits a const IncludeDirective.
	virtual void Visit(const IncludeDirective *includeDirective) = 0;

	/// \brief Visits a NamespaceDefinition.
	virtual void Visit(NamespaceDefinition *namespaceDefinition) = 0;

	/// \brief Visits a const NamespaceDefinition.
	virtual void Visit(const NamespaceDefinition *namespaceDefinition) = 0;

	/// \brief Visits a NativeBlock.
	virtual void Visit(NativeBlock *nativeBlock) = 0;

	/// \brief Visits a const NativeBlock.
	virtual void Visit(const NativeBlock *nativeBlock) = 0;

	/// \brief Visits an EnumDeclaration.
	virtual void Visit(EnumDeclaration *enumDeclaration) = 0;

	/// \brief Visits a const EnumDeclaration.
	virtual void Visit(const EnumDeclaration *enumDeclaration) = 0;

	/// \brief Visits an Enumerator.
	virtual void Visit(Enumerator *enumerator) = 0;

	/// \brief Visits a const Enumerator.
	virtual void Visit(const Enumerator *enumerator) = 0;

	/// \brief Visits a StructDeclaration.
	virtual void Visit(StructDeclaration *structDeclaration) = 0;

	/// \brief Visits a const StructDeclaration.
	virtual void Visit(const StructDeclaration *structDeclaration) = 0;

	/// \brief Visits a FunctionDefinition.
	virtual void Visit(FunctionDefinition *functionDefinition) = 0;

	/// \brief Visits a const FunctionDefinition.
	virtual void Visit(const FunctionDefinition *functionDefinition) = 0;

	/// \brief Visits a FunctionPrototype.
	virtual void Visit(FunctionPrototype *functionPrototype) = 0;

	/// \brief Visits a const FunctionPrototype.
	virtual void Visit(const FunctionPrototype *functionPrototype) = 0;

	/// \brief Visits a Parameter.
	virtual void Visit(Parameter *parameter) = 0;

	/// \brief Visits a const Parameter.
	virtual void Visit(const Parameter *parameter) = 0;

	/// \brief Visits a TypeDescriptor.
	virtual void Visit(TypeDescriptor *typeDescriptor) = 0;

	/// \brief Visits a const TypeDescriptor.
	virtual void Visit(const TypeDescriptor *typeDescriptor) = 0;

	/// \brief Visits a TypeSpecifier.
	virtual void Visit(TypeSpecifier *typeSpecifier) = 0;

	/// \brief Visits a const TypeSpecifier.
	virtual void Visit(const TypeSpecifier *typeSpecifier) = 0;

	/// \brief Visits a NamedInitializer.
	virtual void Visit(NamedInitializer *namedInitializer) = 0;

	/// \brief Visits a const NamedInitializer.
	virtual void Visit(const NamedInitializer *namedInitializer) = 0;

	/// \brief Visits an Initializer.
	virtual void Visit(Initializer *initializer) = 0;

	/// \brief Visits a const Initializer.
	virtual void Visit(const Initializer *initializer) = 0;

	/// \brief Visits a QualifiedIdentifier.
	virtual void Visit(QualifiedIdentifier *identifier) = 0;

	/// \brief Visits a const QualifiedIdentifier.
	virtual void Visit(const QualifiedIdentifier *identifier) = 0;

	/// \brief Visits a CompoundStatement.
	virtual void Visit(CompoundStatement *compoundStatement) = 0;

	/// \brief Visits a const CompoundStatement.
	virtual void Visit(const CompoundStatement *compoundStatement) = 0;

	/// \brief Visits an IfStatement.
	virtual void Visit(IfStatement *ifStatement) = 0;

	/// \brief Visits a const IfStatement.
	virtual void Visit(const IfStatement *ifStatement) = 0;

	/// \brief Visits a SwitchStatement.
	virtual void Visit(SwitchStatement *switchStatement) = 0;

	/// \brief Visits a const SwitchStatement.
	virtual void Visit(const SwitchStatement *switchStatement) = 0;

	/// \brief Visits a SwitchSection.
	virtual void Visit(SwitchSection *switchSection) = 0;

	/// \brief Visits a const SwitchSection.
	virtual void Visit(const SwitchSection *switchSection) = 0;

	/// \brief Visits a SwitchLabel.
	virtual void Visit(SwitchLabel *switchLabel) = 0;

	/// \brief Visits a const SwitchLabel.
	virtual void Visit(const SwitchLabel *switchLabel) = 0;

	/// \brief Visits a WhileStatement.
	virtual void Visit(WhileStatement *whileStatement) = 0;

	/// \brief Visits a const WhileStatement.
	virtual void Visit(const WhileStatement *whileStatement) = 0;

	/// \brief Visits a ForStatement.
	virtual void Visit(ForStatement *forStatement) = 0;

	/// \brief Visits a const ForStatement.
	virtual void Visit(const ForStatement *forStatement) = 0;

	/// \brief Visits a JumpStatement.
	virtual void Visit(JumpStatement *jumpStatement) = 0;

	/// \brief Visits a const JumpStatement.
	virtual void Visit(const JumpStatement *jumpStatement) = 0;

	/// \brief Visits a DeclarativeStatement.
	virtual void Visit(DeclarativeStatement *declarativeStatement) = 0;

	/// \brief Visits a const DeclarativeStatement.
	virtual void Visit(const DeclarativeStatement *declarativeStatement) = 0;

	/// \brief Visits a UsingDeclaration.
	virtual void Visit(UsingDeclaration *usingDeclaration) = 0;

	/// \brief Visits a const UsingDeclaration.
	virtual void Visit(const UsingDeclaration *usingDeclaration) = 0;

	/// \brief Visits a UsingDirective.
	virtual void Visit(UsingDirective *usingDirective) = 0;

	/// \brief Visits a const UsingDirective.
	virtual void Visit(const UsingDirective *usingDirective) = 0;

	/// \brief Visits an ExpressionStatement.
	virtual void Visit(ExpressionStatement *expressionStatement) = 0;

	/// \brief Visits a const ExpressionStatement.
	virtual void Visit(const ExpressionStatement *expressionStatement) = 0;

	/// \brief Visits a ConditionalExpression.
	virtual void Visit(ConditionalExpression *conditionalExpression) = 0;

	/// \brief Visits a const ConditionalExpression.
	virtual void Visit(const ConditionalExpression *conditionalExpression) = 0;

	/// \brief Visits a BinaryExpression.
	virtual void Visit(BinaryExpression *binaryExpression) = 0;

	/// \brief Visits a const BinaryExpression.
	virtual void Visit(const BinaryExpression *binaryExpression) = 0;

	/// \brief Visits a UnaryExpression.
	virtual void Visit(UnaryExpression *unaryExpression) = 0;

	/// \brief Visits a const UnaryExpression.
	virtual void Visit(const UnaryExpression *unaryExpression) = 0;

	/// \brief Visits a PostfixExpression.
	virtual void Visit(PostfixExpression *postfixExpression) = 0;

	/// \brief Visits a const PostfixExpression.
	virtual void Visit(const PostfixExpression *postfixExpression) = 0;

	/// \brief Visits a MemberExpression.
	virtual void Visit(MemberExpression *memberExpression) = 0;

	/// \brief Visits a const MemberExpression.
	virtual void Visit(const MemberExpression *memberExpression) = 0;

	/// \brief Visits an ArraySubscriptExpression.
	virtual void Visit(ArraySubscriptExpression *arraySubscriptExpression) = 0;

	/// \brief Visits a const ArraySubscriptExpression.
	virtual void Visit(const ArraySubscriptExpression *arraySubscriptExpression) = 0;

	/// \brief Visits a FunctionCallExpression.
	virtual void Visit(FunctionCallExpression *functionCallExpression) = 0;

	/// \brief Visits a const FunctionCallExpression.
	virtual void Visit(const FunctionCallExpression *functionCallExpression) = 0;

	/// \brief Visits a CastExpression.
	virtual void Visit(CastExpression *castExpression) = 0;

	/// \brief Visits a const CastExpression.
	virtual void Visit(const CastExpression *castExpression) = 0;

	/// \brief Visits a PropertyofExpression.
	virtual void Visit(PropertyofExpression *propertyofExpression) = 0;

	/// \brief Visits a const PropertyofExpression.
	virtual void Visit(const PropertyofExpression *propertyofExpression) = 0;

	/// \brief Visits a ConstantLiteralExpression.
	virtual void Visit(ConstantLiteralExpression *constantExpression) = 0;

	/// \brief Visits a const ConstantLiteralExpression.
	virtual void Visit(const ConstantLiteralExpression *constantExpression) = 0;

	/// \brief Visits an IdentifierExpression.
	virtual void Visit(IdentifierExpression *identifierExpression) = 0;

	/// \brief Visits a const IdentifierExpression.
	virtual void Visit(const IdentifierExpression *identifierExpression) = 0;

	/// \brief Visits a ThisExpression.
	virtual void Visit(ThisExpression *thisExpression) = 0;

	/// \brief Visits a const ThisExpression.
	virtual void Visit(const ThisExpression *thisExpression) = 0;

	/// \brief Visits an EmptyExpression.
	virtual void Visit(EmptyExpression *emptyExpression) = 0;

	/// \brief Visits a const EmptyExpression.
	virtual void Visit(const EmptyExpression *emptyExpression) = 0;
};


/// \brief A concrete implementation of ParseNodeVisitor with methods that do nothing.
///
/// It is useful to extend from ParseNodeVisitorAdapter when implementing a visitor that is
/// interested in visiting only a select few types of ParseNodes, that way only the few Visit
/// methods of interest need to be overridden.
///
/// \sa ParseNode, ParseNodeVisitor, ParseNodeTraverser
class BOND_API ParseNodeVisitorAdapter: public ParseNodeVisitor
{
public:
	/// \brief Destroys a ParseNodeVisitorAdapter object.
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

	virtual void Visit(UsingDeclaration *usingDeclaration) override {}
	virtual void Visit(const UsingDeclaration *usingDeclaration) override {}

	virtual void Visit(UsingDirective *usingDirective) override {}
	virtual void Visit(const UsingDirective *usingDirective) override {}

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

	virtual void Visit(PropertyofExpression *propertyofExpression) override {}
	virtual void Visit(const PropertyofExpression *propertyofExpression) override {}

	virtual void Visit(ConstantLiteralExpression *constantExpression) override {}
	virtual void Visit(const ConstantLiteralExpression *constantExpression) override {}

	virtual void Visit(IdentifierExpression *identifierExpression) override {}
	virtual void Visit(const IdentifierExpression *identifierExpression) override {}

	virtual void Visit(ThisExpression *thisExpression) override {}
	virtual void Visit(const ThisExpression *thisExpression) override {}

	virtual void Visit(EmptyExpression *emptyExpression) override {}
	virtual void Visit(const EmptyExpression *emptyExpression) override {}
};

/// @}

}

#endif
