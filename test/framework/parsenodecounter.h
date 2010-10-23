#ifndef TEST_FRAMEWORK_PARSENODECOUNTER_H
#define TEST_FRAMEWORK_PARSENODECOUNTER_H

#include "bond/parsenodevisitor.h"

namespace TestFramework
{

struct ParseNodeCount
{
	ParseNodeCount(int defaultValue = 0):
		mTranslationUnit(defaultValue),
		mNamespaceDefinition(defaultValue),
		mEnumDeclaration(defaultValue),
		mEnumerator(defaultValue),
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


class ParseNodeCounter: public Bond::ConstParseNodeVisitor
{
public:
	ParseNodeCounter(): mCount(0) {}
	virtual ~ParseNodeCounter() {}

	const ParseNodeCount &GetCount() const { return mCount; }

	void Count(const Bond::ParseNode *parseNode);

	virtual void VisitTranslationUnit(const Bond::TranslationUnit *translationUnit);
	virtual void VisitNamespaceDefinition(const Bond::NamespaceDefinition *namespaceDefinition);
	virtual void VisitEnumDeclaration(const Bond::EnumDeclaration *enumDeclaration);
	virtual void VisitEnumerator(const Bond::Enumerator *enumerator);
	virtual void VisitFunctionDefinition(const Bond::FunctionDefinition *functionDefinition);
	virtual void VisitFunctionPrototype(const Bond::FunctionPrototype *functionPrototype);
	virtual void VisitParameter(const Bond::Parameter *parameter);
	virtual void VisitTypeDescriptor(const Bond::TypeDescriptor *typeDescriptor);
	virtual void VisitTypeSpecifier(const Bond::TypeSpecifier *typeSpecifier);
	virtual void VisitNamedInitializer(const Bond::NamedInitializer *namedInitializer);
	virtual void VisitInitializer(const Bond::Initializer *initializer);
	virtual void VisitQualifiedIdentifier(const Bond::QualifiedIdentifier *identifier);
	virtual void VisitCompoundStatement(const Bond::CompoundStatement *compoundStatement);
	virtual void VisitIfStatement(const Bond::IfStatement *ifStatement);
	virtual void VisitSwitchStatement(const Bond::SwitchStatement *switchStatement);
	virtual void VisitSwitchSection(const Bond::SwitchSection *switchSection);
	virtual void VisitSwitchLabel(const Bond::SwitchLabel *switchLabel);
	virtual void VisitWhileStatement(const Bond::WhileStatement *whileStatement);
	virtual void VisitJumpStatement(const Bond::JumpStatement *jumpStatement);
	virtual void VisitDeclarativeStatement(const Bond::DeclarativeStatement *declarativeStatement);
	virtual void VisitExpressionStatement(const Bond::ExpressionStatement *expressionStatement);
	virtual void VisitConditionalExpression(const Bond::ConditionalExpression *conditionalExpression);
	virtual void VisitBinaryExpression(const Bond::BinaryExpression *binaryExpression);
	virtual void VisitUnaryExpression(const Bond::UnaryExpression *unaryExpression);
	virtual void VisitPostfixExpression(const Bond::PostfixExpression *postfixExpression);
	virtual void VisitMemberExpression(const Bond::MemberExpression *memberExpression);
	virtual void VisitArraySubscriptExpression(const Bond::ArraySubscriptExpression *arraySubscriptExpression);
	virtual void VisitFunctionCallExpression(const Bond::FunctionCallExpression *functionCallExpression);
	virtual void VisitCastExpression(const Bond::CastExpression *castExpression);
	virtual void VisitSizeofExpression(const Bond::SizeofExpression *sizeofExpression);
	virtual void VisitConstantExpression(const Bond::ConstantExpression *constantExpression);
	virtual void VisitIdentifierExpression(const Bond::IdentifierExpression *identifierValue);

private:
	void CountList(const Bond::ListParseNode *listNode);

	ParseNodeCount mCount;
};

}

#endif
