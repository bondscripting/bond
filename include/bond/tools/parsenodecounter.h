#ifndef BOND_TOOLS_PARSENODECOUNTER_H
#define BOND_TOOLS_PARSENODECOUNTER_H

#include "bond/conf.h"
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
		mUsingDeclaration(defaultValue),
		mUsingDirective(defaultValue),
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

	/// \brief Number of TranslationUnit nodes.
	int mTranslationUnit;

	/// \brief Number of IncludeDirective nodes.
	int mIncludeDirective;

	/// \brief Number of NamespaceDefinition nodes.
	int mNamespaceDefinition;

	/// \brief Number of NativeBlock nodes.
	int mNativeBlock;

	/// \brief Number of EnumDeclaration nodes.
	int mEnumDeclaration;

	/// \brief Number of Enumerator nodes.
	int mEnumerator;

	/// \brief Number of StructDeclaration nodes.
	int mStructDeclaration;

	/// \brief Number of FunctionDefinition nodes.
	int mFunctionDefinition;

	/// \brief Number of FunctionPrototype nodes.
	int mFunctionPrototype;

	/// \brief Number of Parameter nodes.
	int mParameter;

	/// \brief Number of TypeDescriptor nodes.
	int mTypeDescriptor;

	/// \brief Number of TypeSpecifier nodes.
	int mTypeSpecifier;

	/// \brief Number of NamedInitializer nodes.
	int mNamedInitializer;

	/// \brief Number of Initializer nodes.
	int mInitializer;

	/// \brief Number of QualifiedIdentifier nodes.
	int mQualifiedIdentifier;

	/// \brief Number of CompoundStatement nodes.
	int mCompoundStatement;

	/// \brief Number of IfStatement nodes.
	int mIfStatement;

	/// \brief Number of SwitchStatement nodes.
	int mSwitchStatement;

	/// \brief Number of SwitchSection nodes.
	int mSwitchSection;

	/// \brief Number of SwitchLabel nodes.
	int mSwitchLabel;

	/// \brief Number of WhileStatement nodes.
	int mWhileStatement;

	/// \brief Number of ForStatement nodes.
	int mForStatement;

	/// \brief Number of JumpStatement nodes.
	int mJumpStatement;

	/// \brief Number of UsingDeclaration nodes.
	int mUsingDeclaration;

	/// \brief Number of UsingDirective nodes.
	int mUsingDirective;

	/// \brief Number of DeclarativeStatement nodes.
	int mDeclarativeStatement;

	/// \brief Number of ExpressionStatement nodes.
	int mExpressionStatement;

	/// \brief Number of ConditionalExpression nodes.
	int mConditionalExpression;

	/// \brief Number of BinaryExpression nodes.
	int mBinaryExpression;

	/// \brief Number of UnaryExpression nodes.
	int mUnaryExpression;

	/// \brief Number of PostfixExpression nodes.
	int mPostfixExpression;

	/// \brief Number of MemberExpression nodes.
	int mMemberExpression;

	/// \brief Number of ArraySubscriptExpression nodes.
	int mArraySubscriptExpression;

	/// \brief Number of FunctionCallExpression nodes.
	int mFunctionCallExpression;

	/// \brief Number of CastExpression nodes.
	int mCastExpression;

	/// \brief Number of PropertyofExpression nodes.
	int mPropertyofExpression;

	/// \brief Number of ConstantLiteralExpression nodes.
	int mConstantLiteralExpression;

	/// \brief Number of IdentifierExpression nodes.
	int mIdentifierExpression;

	/// \brief Number of ThisExpression nodes.
	int mThisExpression;

	/// \brief Number of EmptyExpression nodes.
	int mEmptyExpression;
};


/// \brief A parse tree traverser that counts encountered parse nodes.
///
/// ParseNodeCounter traverses parse trees and increments counters in ParseNodeCount for each
/// parse node type visited.
///
/// \sa ParseNodeCount, ParseNodeTraverser
class BOND_API ParseNodeCounter: private ParseNodeTraverser
{
public:
	/// \brief Constructs a ParseNodeCounter with all counters initialized to zero.
	ParseNodeCounter(): mCount(0) {}

	/// \brief Destroys a ParseNodeCounter object.
	virtual ~ParseNodeCounter() {}

	/// \brief Returns the current parse node counters.
	/// \returns The accumulated ParseNodeCount values.
	const ParseNodeCount &GetCount() const { return mCount; }

	/// \brief Traverses a parse tree and adds its node counts to the counters, which are not
	/// reset between calls.
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
	virtual void Visit(const UsingDeclaration *usingDeclaration) override;
	virtual void Visit(const UsingDirective *usingDirective) override;
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
