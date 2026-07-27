#ifndef BOND_TOOLS_PARSENODECOUNTER_H
#define BOND_TOOLS_PARSENODECOUNTER_H

#include "bond/compiler/parsenodetraverser.h"

namespace Bond
{

/// \addtogroup tools
/// @{

/// \brief A set of counters for Bond parse node types.
struct ParseNodeCount
{
	/// \brief Constructs a ParseNodeCount and initializes all counters.
	/// \param defaultValue Initial value assigned to each counter.
	explicit ParseNodeCount(int defaultValue = 0):
		mTranslationUnit(defaultValue),
		mIncludeDirective(defaultValue),
		mNamespaceDefinition(defaultValue),
		mNativeBlock(defaultValue),
		mEnumDeclaration(defaultValue),
		mEnumerator(defaultValue),
		mStructDeclaration(defaultValue),
		mFunctionDefinition(defaultValue),
		mFunctionPrototype(defaultValue),
		mParameter(defaultValue),
		mTypeDescriptor(defaultValue),
		mTypeSpecifier(defaultValue),
		mNamedInitializer(defaultValue),
		mInitializer(defaultValue),
		mQualifiedIdentifier(defaultValue),
		mCompoundStatement(defaultValue),
		mIfStatement(defaultValue),
		mSwitchStatement(defaultValue),
		mSwitchSection(defaultValue),
		mSwitchLabel(defaultValue),
		mWhileStatement(defaultValue),
		mForStatement(defaultValue),
		mJumpStatement(defaultValue),
		mDeclarativeStatement(defaultValue),
		mExpressionStatement(defaultValue),
		mConditionalExpression(defaultValue),
		mBinaryExpression(defaultValue),
		mUnaryExpression(defaultValue),
		mPostfixExpression(defaultValue),
		mMemberExpression(defaultValue),
		mArraySubscriptExpression(defaultValue),
		mFunctionCallExpression(defaultValue),
		mCastExpression(defaultValue),
		mPropertyofExpression(defaultValue),
		mConstantLiteralExpression(defaultValue),
		mIdentifierExpression(defaultValue),
		mThisExpression(defaultValue),
		mEmptyExpression(defaultValue)
	{}

	int mTranslationUnit;
	int mIncludeDirective;
	int mNamespaceDefinition;
	int mNativeBlock;
	int mEnumDeclaration;
	int mEnumerator;
	int mStructDeclaration;
	int mFunctionDefinition;
	int mFunctionPrototype;
	int mParameter;
	int mTypeDescriptor;
	int mTypeSpecifier;
	int mNamedInitializer;
	int mInitializer;
	int mQualifiedIdentifier;
	int mCompoundStatement;
	int mIfStatement;
	int mSwitchStatement;
	int mSwitchSection;
	int mSwitchLabel;
	int mWhileStatement;
	int mForStatement;
	int mJumpStatement;
	int mDeclarativeStatement;
	int mExpressionStatement;
	int mConditionalExpression;
	int mBinaryExpression;
	int mUnaryExpression;
	int mPostfixExpression;
	int mMemberExpression;
	int mArraySubscriptExpression;
	int mFunctionCallExpression;
	int mCastExpression;
	int mPropertyofExpression;
	int mConstantLiteralExpression;
	int mIdentifierExpression;
	int mThisExpression;
	int mEmptyExpression;
};


/// \brief A parse tree traverser that counts encountered parse nodes.
///
/// ParseNodeCounter traverses parse trees and increments counters in ParseNodeCount for each
/// parse node type visited.
///
/// \sa ParseNodeCount, ParseNodeTraverser
class ParseNodeCounter: private ParseNodeTraverser
{
public:
	/// \brief Constructs a ParseNodeCounter with all counters initialized to zero.
	ParseNodeCounter(): mCount(0) {}
	virtual ~ParseNodeCounter() {}

	/// \brief Returns the current parse node counters.
	/// \returns The accumulated ParseNodeCount values.
	const ParseNodeCount &GetCount() const { return mCount; }

	/// \brief Traverses a parse tree and accumulates node counts.
	/// \param parseNode Root parse node to traverse.
	void Count(const ParseNode *parseNode) { Traverse(parseNode); }

	/// \brief Traverses a linked list of parse nodes and accumulates node counts.
	/// \param listNode Head of the parse node list to traverse.
	void CountList(const ListParseNode *listNode) { TraverseList(listNode); }

private:
	virtual void Visit(const TranslationUnit *translationUnit) override;
	virtual void Visit(const IncludeDirective *includeDirective) override;
	virtual void Visit(const NamespaceDefinition *namespaceDefinition) override;
	virtual void Visit(const NativeBlock *nativeBlock) override;
	virtual void Visit(const EnumDeclaration *enumDeclaration) override;
	virtual void Visit(const Enumerator *enumerator) override;
	virtual void Visit(const StructDeclaration *structDeclaration) override;
	virtual void Visit(const FunctionDefinition *functionDefinition) override;
	virtual void Visit(const FunctionPrototype *functionPrototype) override;
	virtual void Visit(const Parameter *parameter) override;
	virtual void Visit(const TypeDescriptor *typeDescriptor) override;
	virtual void Visit(const TypeSpecifier *typeSpecifier) override;
	virtual void Visit(const NamedInitializer *namedInitializer) override;
	virtual void Visit(const Initializer *initializer) override;
	virtual void Visit(const QualifiedIdentifier *identifier) override;
	virtual void Visit(const CompoundStatement *compoundStatement) override;
	virtual void Visit(const IfStatement *ifStatement) override;
	virtual void Visit(const SwitchStatement *switchStatement) override;
	virtual void Visit(const SwitchSection *switchSection) override;
	virtual void Visit(const SwitchLabel *switchLabel) override;
	virtual void Visit(const WhileStatement *whileStatement) override;
	virtual void Visit(const ForStatement *forStatement) override;
	virtual void Visit(const JumpStatement *jumpStatement) override;
	virtual void Visit(const DeclarativeStatement *declarativeStatement) override;
	virtual void Visit(const ExpressionStatement *expressionStatement) override;
	virtual void Visit(const ConditionalExpression *conditionalExpression) override;
	virtual void Visit(const BinaryExpression *binaryExpression) override;
	virtual void Visit(const UnaryExpression *unaryExpression) override;
	virtual void Visit(const PostfixExpression *postfixExpression) override;
	virtual void Visit(const MemberExpression *memberExpression) override;
	virtual void Visit(const ArraySubscriptExpression *arraySubscriptExpression) override;
	virtual void Visit(const FunctionCallExpression *functionCallExpression) override;
	virtual void Visit(const CastExpression *castExpression) override;
	virtual void Visit(const PropertyofExpression *propertyofExpression) override;
	virtual void Visit(const ConstantLiteralExpression *constantExpression) override;
	virtual void Visit(const IdentifierExpression *identifierExpression) override;
	virtual void Visit(const ThisExpression *thisExpression) override;
	virtual void Visit(const EmptyExpression *emptyExpression) override;

	ParseNodeCount mCount;
};

/// @}

}
#endif
