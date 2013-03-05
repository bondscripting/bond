#ifndef BOND_PARSENODECOUNTER_H
#define BOND_PARSENODECOUNTER_H

#include "bond/compiler/parsenodetraverser.h"

namespace Bond
{

struct ParseNodeCount
{
	ParseNodeCount(int defaultValue = 0):
		mTranslationUnit(defaultValue),
		mIncludeDirective(defaultValue),
		mNamespaceDefinition(defaultValue),
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
		mSizeofExpression(defaultValue),
		mConstantExpression(defaultValue),
		mIdentifierExpression(defaultValue),
		mThisExpression(defaultValue),
		mEmptyExpression(defaultValue)
	{}

	int mTranslationUnit;
	int mIncludeDirective;
	int mNamespaceDefinition;
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
	int mSizeofExpression;
	int mConstantExpression;
	int mIdentifierExpression;
	int mThisExpression;
	int mEmptyExpression;
};


class ParseNodeCounter: private ParseNodeTraverser
{
public:
	ParseNodeCounter(): mCount(0) {}
	virtual ~ParseNodeCounter() {}

	const ParseNodeCount &GetCount() const { return mCount; }
	void Count(const ParseNode *parseNode) { Traverse(parseNode); }
	void CountList(const ListParseNode *listNode) { TraverseList(listNode); }

private:
	virtual void Visit(const TranslationUnit *translationUnit);
	virtual void Visit(const IncludeDirective *includeDirective);
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
	virtual void Visit(const IdentifierExpression *identifierExpression);
	virtual void Visit(const ThisExpression *thisExpression);
	virtual void Visit(const EmptyExpression *emptyExpression);

	ParseNodeCount mCount;
};

}

#endif
