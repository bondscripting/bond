#ifndef BOND_PARSENODES_H
#define BOND_PARSENODES_H

#include "bond/parsenodesfwd.h"
#include "bond/parsenodevisitor.h"
#include "bond/token.h"

namespace Bond
{

class ParseNode
{
public:
	virtual ~ParseNode() {}

	virtual void Accept(ParseNodeVisitor &visitor) = 0;
	virtual void Accept(ConstParseNodeVisitor &visitor) const = 0;

protected:
	ParseNode() {}
};


template <typename T>
class ListParseNode: public ParseNode
{
public:
	virtual ~ListParseNode() {}

	T *GetNext() { return mNext; }
	const T *GetNext() const { return mNext; }
	void SetNext(T *next) { mNext = next; }

protected:
	ListParseNode(): mNext(0) {}

private:
	T *mNext;
};


class TranslationUnit: public ParseNode
{
public:
	TranslationUnit(ExternalDeclaration *declarationList): mDeclarationList(declarationList) {}
	virtual ~TranslationUnit() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.VisitTranslationUnit(this); }
	virtual void Accept(ConstParseNodeVisitor &visitor) const { visitor.VisitTranslationUnit(this); }

	ExternalDeclaration *GetExternalDeclarationList() { return mDeclarationList; }
	const ExternalDeclaration *GetExternalDeclarationList() const { return mDeclarationList; }

private:
	ExternalDeclaration *mDeclarationList;
};


class ExternalDeclaration: public ListParseNode<ExternalDeclaration>
{
public:
	virtual ~ExternalDeclaration() {}

protected:
	ExternalDeclaration() {}
};


class NamespaceDefinition: public ExternalDeclaration
{
public:
	NamespaceDefinition(const Token *name, ExternalDeclaration *declarationList):
		mName(name),
		mDeclarationList(declarationList)
	{}

	virtual ~NamespaceDefinition() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.VisitNamespaceDefinition(this); }
	virtual void Accept(ConstParseNodeVisitor &visitor) const { visitor.VisitNamespaceDefinition(this); }

	const Token *GetName() const { return mName; }

	ExternalDeclaration *GetExternalDeclarationList() { return mDeclarationList; }
	const ExternalDeclaration *GetExternalDeclarationList() const { return mDeclarationList; }

private:
	const Token *mName;
	ExternalDeclaration *mDeclarationList;
};


class EnumDeclaration: public ExternalDeclaration
{
public:
	EnumDeclaration(const Token *name, Enumerator *enumeratorList):
		mName(name),
		mEnumeratorList(enumeratorList)
	{}

	virtual ~EnumDeclaration() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.VisitEnumDeclaration(this); }
	virtual void Accept(ConstParseNodeVisitor &visitor) const { visitor.VisitEnumDeclaration(this); }

	const Token *GetName() const { return mName; }

	Enumerator *GetEnumeratorList() { return mEnumeratorList; }
	const Enumerator *GetEnumeratorList() const { return mEnumeratorList; }

private:
	const Token *mName;
	Enumerator *mEnumeratorList;
};


class Enumerator: public ListParseNode<Enumerator>
{
public:
	Enumerator(const Token *name, Expression *value): mName(name), mValue(value) {}
	virtual ~Enumerator() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.VisitEnumerator(this); }
	virtual void Accept(ConstParseNodeVisitor &visitor) const { visitor.VisitEnumerator(this); }

	const Token *GetName() const { return mName; }

	Expression *GetValue() { return mValue; }
	const Expression *GetValue() const { return mValue; }

private:
	const Token *mName;
	Expression *mValue;
};


class FunctionDefinition: public ExternalDeclaration
{
public:
	FunctionDefinition(FunctionPrototype *prototype, CompoundStatement *body):
		mPrototype(prototype),
		mBody(body)
	{}

	virtual ~FunctionDefinition() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.VisitFunctionDefinition(this); }
	virtual void Accept(ConstParseNodeVisitor &visitor) const { visitor.VisitFunctionDefinition(this); }

	FunctionPrototype *GetPrototype() { return mPrototype; }
	const FunctionPrototype *GetPrototype() const { return mPrototype; }

	CompoundStatement *GetBody() { return mBody; }
	const CompoundStatement *GetBody() const { return mBody; }

private:
	FunctionPrototype *mPrototype;
	CompoundStatement *mBody;
};


class FunctionPrototype: public ParseNode
{
public:
	FunctionPrototype(const Token *name, TypeDescriptor *returnType, Parameter *parameterList):
		mName(name),
		mReturnType(returnType),
		mParameterList(parameterList)
	{}

	virtual ~FunctionPrototype() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.VisitFunctionPrototype(this); }
	virtual void Accept(ConstParseNodeVisitor &visitor) const { visitor.VisitFunctionPrototype(this); }

	const Token *GetName() const { return mName; }

	TypeDescriptor *GetReturnType() { return mReturnType; }
	const TypeDescriptor *GetReturnType() const { return mReturnType; }

	Parameter *GetParameterList() { return mParameterList; }
	const Parameter *GetParameterList() const { return mParameterList; }

private:
	const Token *mName;
	TypeDescriptor *mReturnType;
	Parameter *mParameterList;
};


class StructDeclaration: public ExternalDeclaration
{
public:
	StructDeclaration(const Token *name):
		mName(name)
	{}

	virtual ~StructDeclaration() {}

	//virtual void Accept(ParseNodeVisitor &visitor) { visitor.VisitStructDeclaration(this); }
	//virtual void Accept(ConstParseNodeVisitor &visitor) const { visitor.VisitStructDeclaration(this); }

	const Token *GetName() const { return mName; }
	// TODO

private:
	const Token *mName;
};


class Parameter: public ListParseNode<Parameter>
{
public:
	Parameter(const Token *name, TypeDescriptor *typeDescriptor): mName(name), mTypeDescriptor(typeDescriptor) {}
	virtual ~Parameter() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.VisitParameter(this); }
	virtual void Accept(ConstParseNodeVisitor &visitor) const { visitor.VisitParameter(this); }

	const Token *GetName() const { return mName; }

	TypeDescriptor *GetTypeDescriptor() { return mTypeDescriptor; }
	const TypeDescriptor *GetTypeDescriptor() const { return mTypeDescriptor; }

private:
	const Token *mName;
	TypeDescriptor *mTypeDescriptor;
};


class TypeDescriptor: public ParseNode
{
public:
	enum Variant
	{
		VARIANT_VALUE,
		VARIANT_POINTER,
		VARIANT_ARRAY,
	};

	TypeDescriptor() {}
	TypeDescriptor(TypeSpecifier *specifier, bool isConst):
		mSpecifier(specifier),
		mParent(0),
		mLength(0),
		mVariant(VARIANT_VALUE),
		mIsConst(isConst)
	{}

	TypeDescriptor(TypeDescriptor *parent, bool isConst):
		mSpecifier(0),
		mParent(parent),
		mLength(0),
		mVariant(VARIANT_POINTER),
		mIsConst(isConst)
	{}

	TypeDescriptor(TypeDescriptor *parent, Expression *length):
		mSpecifier(0),
		mParent(parent),
		mLength(length),
		mVariant(VARIANT_ARRAY),
		mIsConst(false)
	{}

	virtual ~TypeDescriptor() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.VisitTypeDescriptor(this); }
	virtual void Accept(ConstParseNodeVisitor &visitor) const { visitor.VisitTypeDescriptor(this); }

	TypeSpecifier *GetTypeSpecifier() { return mSpecifier; }
	const TypeSpecifier *GetTypeSpecifier() const { return mSpecifier; }

	TypeDescriptor *GetParent() { return mParent; }
	const TypeDescriptor *GetParent() const { return mParent; }

	Expression *GetLength() { return mLength; }
	const Expression *GetLength() const { return mLength; }

	Variant GetVariant() const { return mVariant; }
	bool IsConst() const { return mIsConst; }

private:
	TypeSpecifier *mSpecifier;
	TypeDescriptor *mParent;
	Expression *mLength;
	Variant mVariant;
	bool mIsConst;
};


class TypeSpecifier: public ParseNode
{
public:
	TypeSpecifier(const Token *primitiveType): mPrimitiveType(primitiveType), mIdentifier(0) {}
	TypeSpecifier(QualifiedIdentifier *identifier): mPrimitiveType(0), mIdentifier(identifier) {}
	virtual ~TypeSpecifier() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.VisitTypeSpecifier(this); }
	virtual void Accept(ConstParseNodeVisitor &visitor) const { visitor.VisitTypeSpecifier(this); }

	const Token *GetPrimitiveType() const { return mPrimitiveType; }

	QualifiedIdentifier *GetIdentifier() { return mIdentifier; }
	const QualifiedIdentifier *GetIdentifier() const { return mIdentifier; }

private:
	const Token *mPrimitiveType;
	QualifiedIdentifier *mIdentifier;
};


class NamedInitializer: public ListParseNode<NamedInitializer>
{
public:
	NamedInitializer(const Token *name):
		mName(name)
	{}

	virtual ~NamedInitializer() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.VisitNamedInitializer(this); }
	virtual void Accept(ConstParseNodeVisitor &visitor) const { visitor.VisitNamedInitializer(this); }

	const Token *GetName() const { return mName; }

private:
	const Token *mName;
};


class QualifiedIdentifier: public ListParseNode<QualifiedIdentifier>
{
public:
	QualifiedIdentifier(const Token *name): mName(name) {}
	virtual ~QualifiedIdentifier() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.VisitQualifiedIdentifier(this); }
	virtual void Accept(ConstParseNodeVisitor &visitor) const { visitor.VisitQualifiedIdentifier(this); }

	const Token *GetName() const { return mName; }

private:
	const Token *mName;
};


class Statement: public ListParseNode<Statement>
{
public:
	virtual ~Statement() {}

protected:
	Statement() {}
};


class CompoundStatement: public Statement
{
public:
	CompoundStatement(Statement *statementList): mStatementList(statementList) {}
	virtual ~CompoundStatement() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.VisitCompoundStatement(this); }
	virtual void Accept(ConstParseNodeVisitor &visitor) const { visitor.VisitCompoundStatement(this); }

	Statement *GetStatementList() { return mStatementList; }
	const Statement *GetStatementList() const { return mStatementList; }

private:
	Statement *mStatementList;
};


class IfStatement: public Statement
{
public:
	IfStatement(Expression *condition, Statement *thenStatement, Statement *elseStatement):
		mCondition(condition),
		mThenStatement(thenStatement),
		mElseStatement(elseStatement)
	{}

	virtual ~IfStatement() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.VisitIfStatement(this); }
	virtual void Accept(ConstParseNodeVisitor &visitor) const { visitor.VisitIfStatement(this); }

	Expression *GetCondition() { return mCondition; }
	const Expression *GetCondition() const { return mCondition; }

	Statement *GetThenStatement() { return mThenStatement; }
	const Statement *GetThenStatement() const { return mThenStatement; }

	Statement *GetElseStatement() { return mElseStatement; }
	const Statement *GetElseStatement() const { return mElseStatement; }

private:
	Expression *mCondition;
	Statement *mThenStatement;
	Statement *mElseStatement;
};


class SwitchStatement: public Statement
{
public:
	SwitchStatement(Expression *control, SwitchSection *sectionList):
		mControl(control),
		mSectionList(sectionList)
	{}

	virtual ~SwitchStatement() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.VisitSwitchStatement(this); }
	virtual void Accept(ConstParseNodeVisitor &visitor) const { visitor.VisitSwitchStatement(this); }

	Expression *GetControl() { return mControl; }
	const Expression *GetControl() const { return mControl; }

	SwitchSection *GetSectionList() { return mSectionList; }
	const SwitchSection *GetSectionList() const { return mSectionList; }

private:
	Expression *mControl;
	SwitchSection *mSectionList;
};


class SwitchSection: public ListParseNode<SwitchSection>
{
public:
	SwitchSection(SwitchLabel *labelList, Statement* statementList):
		mLabelList(labelList),
		mStatementList(statementList)
	{}

	virtual ~SwitchSection() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.VisitSwitchSection(this); }
	virtual void Accept(ConstParseNodeVisitor &visitor) const { visitor.VisitSwitchSection(this); }

	SwitchLabel *GetLabelList() { return mLabelList; }
	const SwitchLabel *GetLabelList() const { return mLabelList; }

	Statement *GetStatementList() { return mStatementList; }
	const Statement *GetStatementList() const { return mStatementList; }

private:
	SwitchLabel *mLabelList;
	Statement *mStatementList;
};


class SwitchLabel: public ListParseNode<SwitchLabel>
{
public:
	enum Variant
	{
		VARIANT_CASE,
		VARIANT_DEFAULT,
	};

	SwitchLabel(const Token *label):
		mLabel(label),
		mExpression(0),
		mVariant(VARIANT_DEFAULT)
	{}

	SwitchLabel(const Token *label, Expression *expression):
		mLabel(label),
		mExpression(expression),
		mVariant(VARIANT_CASE)
	{}

	virtual ~SwitchLabel() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.VisitSwitchLabel(this); }
	virtual void Accept(ConstParseNodeVisitor &visitor) const { visitor.VisitSwitchLabel(this); }

	const Token *GetLabel() const { return mLabel; }

	Expression *GetExpression() { return mExpression; }
	const Expression *GetExpression() const { return mExpression; }

	Variant GetVariant() const { return mVariant; }

private:
	const Token *mLabel;
	Expression *mExpression;
	Variant mVariant;
};


class WhileStatement: public Statement
{
public:
	enum Variant
	{
		VARIANT_WHILE,
		VARIANT_DO_WHILE,
	};

	WhileStatement(Expression *condition, Statement *body, Variant variant):
		mCondition(condition),
		mBody(body),
		mVariant(variant)
	{}

	virtual ~WhileStatement() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.VisitWhileStatement(this); }
	virtual void Accept(ConstParseNodeVisitor &visitor) const { visitor.VisitWhileStatement(this); }

	Expression *GetCondition() { return mCondition; }
	const Expression *GetCondition() const { return mCondition; }

	Statement *GetBody() { return mBody; }
	const Statement *GetBody() const { return mBody; }

	Variant GetVariant() const { return mVariant; }

private:
	Expression *mCondition;
	Statement *mBody;
	Variant mVariant;
};


class JumpStatement: public Statement
{
public:
	JumpStatement(const Token *op, Expression *rhs):	mOperator(op), mRhs(rhs) {}
	virtual ~JumpStatement() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.VisitJumpStatement(this); }
	virtual void Accept(ConstParseNodeVisitor &visitor) const { visitor.VisitJumpStatement(this); }

	const Token *GetOperator() const { return mOperator; }

	Expression *GetRhs() { return mRhs; }
	const Expression *GetRhs() const { return mRhs; }

private:
	const Token *mOperator;
	Expression *mRhs;
};


class DeclarativeStatement: public Statement
{
public:
	DeclarativeStatement(TypeDescriptor *typeDescriptor, NamedInitializer *initializerList):
		mTypeDescriptor(typeDescriptor),
		mNamedInitializerList(initializerList)
	{}

	virtual ~DeclarativeStatement() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.VisitDeclarativeStatement(this); }
	virtual void Accept(ConstParseNodeVisitor &visitor) const { visitor.VisitDeclarativeStatement(this); }

	const TypeDescriptor *GetTypeDescriptor() const { return mTypeDescriptor; }
	TypeDescriptor *GetTypeDescriptor() { return mTypeDescriptor; }

	const NamedInitializer *GetNamedInitializerList() const { return mNamedInitializerList; }
	NamedInitializer *GetNamedInitializerList() { return mNamedInitializerList; }

private:
	TypeDescriptor *mTypeDescriptor;
	NamedInitializer *mNamedInitializerList;
};


class ExpressionStatement: public Statement
{
public:
	ExpressionStatement(Expression *expression): mExpression(expression) {}
	virtual ~ExpressionStatement() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.VisitExpressionStatement(this); }
	virtual void Accept(ConstParseNodeVisitor &visitor) const { visitor.VisitExpressionStatement(this); }

	Expression *GetExpression() { return mExpression; }
	const Expression *GetExpression() const { return mExpression; }

private:
	Expression *mExpression;
};


class Expression: public ListParseNode<Expression>
{
public:
	virtual ~Expression() {}

protected:
	Expression() {}
};


class ConditionalExpression: public Expression
{
public:
	ConditionalExpression(Expression *condition, Expression *trueExpression, Expression *falseExpression):
		mCondition(condition),
		mTrueExpression(trueExpression),
		mFalseExpression(falseExpression)
	{}

	virtual ~ConditionalExpression() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.VisitConditionalExpression(this); }
	virtual void Accept(ConstParseNodeVisitor &visitor) const { visitor.VisitConditionalExpression(this); }

	Expression *GetCondition() { return mCondition; }
	const Expression *GetCondition() const { return mCondition; }

	Expression *GetTrueExpression() { return mTrueExpression; }
	const Expression *GetTrueExpression() const { return mTrueExpression; }

	Expression *GetFalseExpression() { return mFalseExpression; }
	const Expression *GetFalseExpression() const { return mFalseExpression; }

private:
	Expression *mCondition;
	Expression *mTrueExpression;
	Expression *mFalseExpression;
};


class BinaryExpression: public Expression
{
public:
	BinaryExpression(const Token *op, Expression *lhs, Expression *rhs):
		mOperator(op),
		mLhs(lhs),
		mRhs(rhs)
	{}

	virtual ~BinaryExpression() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.VisitBinaryExpression(this); }
	virtual void Accept(ConstParseNodeVisitor &visitor) const { visitor.VisitBinaryExpression(this); }

	const Token *GetOperator() const { return mOperator; }

	Expression *GetLhs() { return mLhs; }
	const Expression *GetLhs() const { return mLhs; }

	Expression *GetRhs() { return mRhs; }
	const Expression *GetRhs() const { return mRhs; }

private:
	const Token *mOperator;
	Expression *mLhs;
	Expression *mRhs;
};


class UnaryExpression: public Expression
{
public:
	UnaryExpression(const Token *op, Expression *rhs): mOperator(op), mRhs(rhs) {}
	virtual ~UnaryExpression() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.VisitUnaryExpression(this); }
	virtual void Accept(ConstParseNodeVisitor &visitor) const { visitor.VisitUnaryExpression(this); }

	const Token *GetOperator() const { return mOperator; }

	Expression *GetRhs() { return mRhs; }
	const Expression *GetRhs() const { return mRhs; }

private:
	const Token *mOperator;
	Expression *mRhs;
};


class PostfixExpression: public Expression
{
public:
	PostfixExpression(const Token *op, Expression *lhs): mOperator(op), mLhs(lhs) {}
	virtual ~PostfixExpression() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.VisitPostfixExpression(this); }
	virtual void Accept(ConstParseNodeVisitor &visitor) const { visitor.VisitPostfixExpression(this); }

	const Token *GetOperator() const { return mOperator; }

	Expression *GetLhs() { return mLhs; }
	const Expression *GetLhs() const { return mLhs; }

private:
	const Token *mOperator;
	Expression *mLhs;
};


class MemberExpression: public Expression
{
public:
	MemberExpression(const Token *op, const Token *memberName, Expression *lhs):
		mOperator(op),
		mMemberName(memberName),
		mLhs(lhs)
	{}

	virtual ~MemberExpression() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.VisitMemberExpression(this); }
	virtual void Accept(ConstParseNodeVisitor &visitor) const { visitor.VisitMemberExpression(this); }

	const Token *GetOperator() const { return mOperator; }
	const Token *GetMemberName() const { return mMemberName; }

	Expression *GetLhs() { return mLhs; }
	const Expression *GetLhs() const { return mLhs; }

private:
	const Token *mOperator;
	const Token *mMemberName;
	Expression *mLhs;
};


class ArraySubscriptExpression: public Expression
{
public:
	ArraySubscriptExpression(Expression *lhs, Expression *index): mLhs(lhs), mIndex(index) {}
	virtual ~ArraySubscriptExpression() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.VisitArraySubscriptExpression(this); }
	virtual void Accept(ConstParseNodeVisitor &visitor) const { visitor.VisitArraySubscriptExpression(this); }

	Expression *GetLhs() { return mLhs; }
	const Expression *GetLhs() const { return mLhs; }

	Expression *GetIndex() { return mIndex; }
	const Expression *GetIndex() const { return mIndex; }

private:
	Expression *mLhs;
	Expression *mIndex;
};


class FunctionCallExpression: public Expression
{
public:
	FunctionCallExpression(Expression *lhs, Expression *argumentList): mLhs(lhs), mArgumentList(argumentList) {}
	virtual ~FunctionCallExpression() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.VisitFunctionCallExpression(this); }
	virtual void Accept(ConstParseNodeVisitor &visitor) const { visitor.VisitFunctionCallExpression(this); }

	Expression *GetLhs() { return mLhs; }
	const Expression *GetLhs() const { return mLhs; }

	Expression *GetArgumentList() { return mArgumentList; }
	const Expression *GetArgumentList() const { return mArgumentList; }

private:
	Expression *mLhs;
	Expression *mArgumentList;
};


class CastExpression: public Expression
{
public:
	CastExpression(TypeDescriptor *typeDescriptor, Expression *rhs):
		mTypeDescriptor(typeDescriptor),
		mRhs(rhs)
	{}

	virtual ~CastExpression() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.VisitCastExpression(this); }
	virtual void Accept(ConstParseNodeVisitor &visitor) const { visitor.VisitCastExpression(this); }

	const TypeDescriptor *GetTypeDescriptor() const { return mTypeDescriptor; }
	TypeDescriptor *GetTypeDescriptor() { return mTypeDescriptor; }

	Expression *GetRhs() { return mRhs; }
	const Expression *GetRhs() const { return mRhs; }

private:
	TypeDescriptor *mTypeDescriptor;
	Expression *mRhs;
};


class SizeofExpression: public Expression
{
public:
	SizeofExpression(TypeDescriptor *typeDescriptor): mTypeDescriptor(typeDescriptor), mRhs(0) {}
	SizeofExpression(Expression *rhs): mTypeDescriptor(0), mRhs(rhs) {}
	virtual ~SizeofExpression() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.VisitSizeofExpression(this); }
	virtual void Accept(ConstParseNodeVisitor &visitor) const { visitor.VisitSizeofExpression(this); }

	const TypeDescriptor *GetTypeDescriptor() const { return mTypeDescriptor; }
	TypeDescriptor *GetTypeDescriptor() { return mTypeDescriptor; }

	Expression *GetRhs() { return mRhs; }
	const Expression *GetRhs() const { return mRhs; }

private:
	TypeDescriptor *mTypeDescriptor;
	Expression *mRhs;
};


class ConstantExpression: public Expression
{
public:
	ConstantExpression(const Token *value): mValue(value) {}
	virtual ~ConstantExpression() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.VisitConstantExpression(this); }
	virtual void Accept(ConstParseNodeVisitor &visitor) const { visitor.VisitConstantExpression(this); }

	const Token *GetValue() const { return mValue; }

private:
	const Token *mValue;
};


class IdentifierExpression: public Expression
{
public:
	IdentifierExpression(QualifiedIdentifier *identifier): mIdentifier(identifier) {}
	virtual ~IdentifierExpression() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.VisitIdentifierExpression(this); }
	virtual void Accept(ConstParseNodeVisitor &visitor) const { visitor.VisitIdentifierExpression(this); }

	QualifiedIdentifier *GetIdentifier() { return mIdentifier; }
	const QualifiedIdentifier *GetIdentifier() const { return mIdentifier; }

private:
	QualifiedIdentifier *mIdentifier;
};

}

#endif
