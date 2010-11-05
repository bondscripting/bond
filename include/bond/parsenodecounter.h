#ifndef BOND_PARSENODECOUNTER_H
#define BOND_PARSENODECOUNTER_H

#include "bond/parsenodetraverser.h"

namespace Bond
{

struct ParseNodeCount
{
	ParseNodeCount(int defaultValue = 0):
		mTranslationUnit(defaultValue),
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
		mIdentifierExpression(defaultValue)
	{}

	int mTranslationUnit;
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
};


class ParseNodeCounter: private ConstParseNodeTraverser
{
public:
	ParseNodeCounter(): mCount(0) {}
	virtual ~ParseNodeCounter() {}

	const ParseNodeCount &GetCount() const { return mCount; }
	void Count(const ParseNode *parseNode) { Traverse(parseNode); }

private:
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
	virtual void VisitQualifiedIdentifier(const QualifiedIdentifier *identifier);
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
	virtual void VisitConstantExpression(const ConstantExpression *constantExpression);
	virtual void VisitIdentifierExpression(const IdentifierExpression *identifierValue);

	ParseNodeCount mCount;
};

}

#endif
