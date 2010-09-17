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
	virtual void Accept(ParseNodeVisitor &visitor) = 0;
	virtual void Accept(ConstParseNodeVisitor &visitor) const = 0;

protected:
	ParseNode() {}
	virtual ~ParseNode() {}
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


class ExternalDeclaration: public ParseNode
{
public:
	ExternalDeclaration *GetNext() { return mNext; }
	const ExternalDeclaration *GetNext() const { return mNext; }
	void SetNext(ExternalDeclaration *next) { mNext = next; }

protected:
	ExternalDeclaration(): mNext(0) {}
	virtual ~ExternalDeclaration() {}

private:
	ExternalDeclaration *mNext;
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


class Enumerator: public ParseNode
{
public:
	Enumerator(const Token *name, Expression *value): mName(name), mValue(value), mNext(0) {}
	virtual ~Enumerator() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.VisitEnumerator(this); }
	virtual void Accept(ConstParseNodeVisitor &visitor) const { visitor.VisitEnumerator(this); }

	const Token *GetName() const { return mName; }

	Expression *GetValue() { return mValue; }
	const Expression *GetValue() const { return mValue; }

	Enumerator *GetNext() { return mNext; }
	const Enumerator *GetNext() const { return mNext; }
	void SetNext(Enumerator *next) { mNext = next; }

private:
	const Token *mName;
	Expression *mValue;
	Enumerator *mNext;
};


class FunctionDefinition: public ExternalDeclaration
{
public:
	FunctionDefinition(FunctionPrototype *prototype):
		mPrototype(prototype)
	{}

	virtual ~FunctionDefinition() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.VisitFunctionDefinition(this); }
	virtual void Accept(ConstParseNodeVisitor &visitor) const { visitor.VisitFunctionDefinition(this); }

	FunctionPrototype *GetPrototype() { return mPrototype; }
	const FunctionPrototype *GetPrototype() const { return mPrototype; }

private:
	FunctionPrototype *mPrototype;
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


class Parameter: public ParseNode
{
public:
	Parameter(const Token *name, TypeDescriptor *typeDescriptor):
		mName(name),
		mTypeDescriptor(typeDescriptor)
	{}

	virtual ~Parameter() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.VisitParameter(this); }
	virtual void Accept(ConstParseNodeVisitor &visitor) const { visitor.VisitParameter(this); }

	const Token *GetName() const { return mName; }

	TypeDescriptor *GetTypeDescriptor() { return mTypeDescriptor; }
	const TypeDescriptor *GetTypeDescriptor() const { return mTypeDescriptor; }

	Parameter *GetNext() { return mNext; }
	const Parameter *GetNext() const { return mNext; }
	void SetNext(Parameter *next) { mNext = next; }

private:
	const Token *mName;
	TypeDescriptor *mTypeDescriptor;
	Parameter *mNext;
};


class TypeDescriptor: public ParseNode
{
public:
	enum Descriptor
	{
		DESC_VALUE,
		DESC_POINTER,
		DESC_ARRAY,
	};

	TypeDescriptor() {}
	TypeDescriptor(TypeSpecifier *specifier, bool isConst):
		mSpecifier(specifier),
		mParent(0),
		mLength(0),
		mDescriptor(DESC_VALUE),
		mIsConst(isConst)
	{}

	TypeDescriptor(TypeDescriptor *parent, bool isConst):
		mSpecifier(0),
		mParent(parent),
		mLength(0),
		mDescriptor(DESC_POINTER),
		mIsConst(isConst)
	{}

	TypeDescriptor(TypeDescriptor *parent, Expression *length):
		mSpecifier(0),
		mParent(parent),
		mLength(length),
		mDescriptor(DESC_ARRAY),
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

	Descriptor GetDescriptor() const { return mDescriptor; }
	bool IsConst() const { return mIsConst; }

private:
	TypeSpecifier *mSpecifier;
	TypeDescriptor *mParent;
	Expression *mLength;
	Descriptor mDescriptor;
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


class QualifiedIdentifier: public ParseNode
{
public:
	QualifiedIdentifier(const Token *name): mName(name), mNext(0) {}
	virtual ~QualifiedIdentifier() {}
	virtual void Accept(ParseNodeVisitor &visitor) { visitor.VisitQualifiedIdentifier(this); }
	virtual void Accept(ConstParseNodeVisitor &visitor) const { visitor.VisitQualifiedIdentifier(this); }

	const Token *GetName() const { return mName; }

	QualifiedIdentifier *GetNext() { return mNext; }
	const QualifiedIdentifier *GetNext() const { return mNext; }
	void SetNext(QualifiedIdentifier *next) { mNext = next; }

private:
	const Token *mName;
	QualifiedIdentifier *mNext;
};


class Expression: public ParseNode
{
public:
	Expression *GetNext() { return mNext; }
	const Expression *GetNext() const { return mNext; }
	void SetNext(Expression *next) { mNext = next; }

protected:
	Expression(): mNext(0) {}
	virtual ~Expression() {}

private:
	Expression *mNext;
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
	UnaryExpression(const Token *op, Expression *rhs):
		mOperator(op),
		mRhs(rhs)
	{}

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
	PostfixExpression(const Token *op, Expression *lhs):
		mOperator(op),
		mLhs(lhs)
	{}

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
	ArraySubscriptExpression(Expression *lhs, Expression *index):
		mLhs(lhs),
		mIndex(index)
	{}

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
	FunctionCallExpression(Expression *lhs, Expression *argumentList):
		mLhs(lhs),
		mArgumentList(argumentList)
	{}

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
	SizeofExpression(TypeDescriptor *typeDescriptor):
		mTypeDescriptor(typeDescriptor),
		mRhs(0)
	{}

	SizeofExpression(Expression *rhs):
		mTypeDescriptor(0),
		mRhs(rhs)
	{}

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
