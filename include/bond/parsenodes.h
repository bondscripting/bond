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

protected:
	ParseNode() {}
	virtual ~ParseNode() {}
};


class TranslationUnit: public ParseNode
{
public:
  TranslationUnit(ExternalDeclaration *declarations): mDeclarations(declarations) {}
	virtual ~TranslationUnit() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.VisitTranslationUnit(this); }

	ExternalDeclaration *GetExternalDeclarationList() { return mDeclarations; }
	const ExternalDeclaration *GetExternalDeclarationList() const { return mDeclarations; }

private:
	ExternalDeclaration *mDeclarations;
};


class ExternalDeclaration: public ParseNode
{
public:
	const Token *GetName() const { return mName; }
	void SetName(const Token *name) { mName = name; }

	ExternalDeclaration *GetNext() { return mNext; }
	const ExternalDeclaration *GetNext() const { return mNext; }
	void SetNext(ExternalDeclaration *next) { mNext = next; }

protected:
	ExternalDeclaration(const Token *name): mName(name), mNext(0) {}
	virtual ~ExternalDeclaration() {}

private:
	const Token *mName;
	ExternalDeclaration *mNext;
};


class NamespaceDefinition: public ExternalDeclaration
{
public:
	NamespaceDefinition(const Token *name, ExternalDeclaration *declarations):
		ExternalDeclaration(name),
	 	mDeclarations(declarations)
	{}

	virtual ~NamespaceDefinition() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.VisitNamespaceDefinition(this); }

	ExternalDeclaration *GetExternalDeclarationList() { return mDeclarations; }
	const ExternalDeclaration *GetExternalDeclarationList() const { return mDeclarations; }

private:
	ExternalDeclaration *mDeclarations;
};


class EnumDeclaration: public ExternalDeclaration
{
public:
	EnumDeclaration(const Token *name, Enumerator *enumerators):
		ExternalDeclaration(name),
	 	mEnumerators(enumerators)
	{}

	virtual ~EnumDeclaration() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.VisitEnumDeclaration(this); }

	Enumerator *GetEnumeratorList() { return mEnumerators; }
	const Enumerator *GetEnumeratorList() const { return mEnumerators; }

private:
	Enumerator *mEnumerators;
};


class Enumerator: public ParseNode
{
public:
	Enumerator(const Token *name, Expression *value): mName(name), mValue(value), mNext(0) {}
	virtual ~Enumerator() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.VisitEnumerator(this); }

	Enumerator *GetNext() { return mNext; }

	Expression *GetValue() { return mValue; }
	const Expression *GetValue() const { return mValue; }

	const Enumerator *GetNext() const { return mNext; }
	void SetNext(Enumerator *next) { mNext = next; }

private:
	const Token *mName;
	Expression *mValue;
	Enumerator *mNext;
};


class TypeDescriptor: public ParseNode
{
public:
	TypeDescriptor() {}
	virtual ~TypeDescriptor() {}
	virtual void Accept(ParseNodeVisitor &visitor) { visitor.VisitTypeDescriptor(this); }
};


class Expression: public ParseNode
{
protected:
	Expression() {}
	virtual ~Expression() {}
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

	const Token *GetOperator() const { return mOperator; }

	Expression *GetRhs() { return mRhs; }
	const Expression *GetRhs() const { return mRhs; }

private:
	const Token *mOperator;
	Expression *mRhs;
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

	const TypeDescriptor *GetTypeDescriptor() const { return mTypeDescriptor; }
	TypeDescriptor *GetTypeDescriptor() { return mTypeDescriptor; }

	Expression *GetRhs() { return mRhs; }
	const Expression *GetRhs() const { return mRhs; }

private:
	TypeDescriptor *mTypeDescriptor;
	Expression *mRhs;
};


class ConstantValue: public Expression
{
public:
	ConstantValue(const Token *value): mValue(value) {}
	virtual ~ConstantValue() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.VisitConstantValue(this); }

	const Token *GetValue() const { return mValue; }

private:
	const Token *mValue;
};

}

#endif
