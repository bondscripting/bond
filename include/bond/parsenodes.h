#ifndef BOND_PARSENODES_H
#define BOND_PARSENODES_H

#include "bond/token.h"

namespace Bond
{

class ParseNode;
class TranslationUnit;
class ExternalDeclaration;
class NamespaceDefinition;
class EnumDeclaration;
class Enumerator;
class TypeDescriptor;
class Expression;
class ConditionalExpression;
class BinaryExpression;
class UnaryExpression;
class CastExpression;
class SizeofExpression;
class ConstantValue;


class ParseNode
{
public:
	ParseNode() {}
	virtual ~ParseNode() {}
};


class TranslationUnit: public ParseNode
{
public:
  TranslationUnit(): mDeclarations(0) {}
  TranslationUnit(ExternalDeclaration *declarations): mDeclarations(declarations) {}
	virtual ~TranslationUnit() {}

	ExternalDeclaration *GetExternalDeclarationList() { return mDeclarations; }
	const ExternalDeclaration *GetExternalDeclarationList() const { return mDeclarations; }

private:
	ExternalDeclaration *mDeclarations;
};


class ExternalDeclaration: public ParseNode
{
public:
	ExternalDeclaration(): mName(0), mNext(0) {}
	ExternalDeclaration(const Token *name): mName(name), mNext(0) {}
	virtual ~ExternalDeclaration() {}

	const Token *mName;
	const Token *GetName() const { return mName; }
	void SetName(const Token *name) { mName = name; }

	ExternalDeclaration *GetNext() { return mNext; }
	const ExternalDeclaration *GetNext() const { return mNext; }
	void SetNext(ExternalDeclaration *next) { mNext = next; }

private:
	const Token *name;
	ExternalDeclaration *mNext;
};


class NamespaceDefinition: public ExternalDeclaration
{
public:
	NamespaceDefinition(): mDeclarations(0) {}

 NamespaceDefinition(const Token *name, ExternalDeclaration *declarations):
		ExternalDeclaration(name),
	 	mDeclarations(declarations)
	{}

	virtual ~NamespaceDefinition() {}

	ExternalDeclaration *GetExternalDeclarationList() { return mDeclarations; }
	const ExternalDeclaration *GetExternalDeclarationList() const { return mDeclarations; }

private:
	ExternalDeclaration *mDeclarations;
};


class EnumDeclaration: public ExternalDeclaration
{
public:
	EnumDeclaration(): mEnumerators(0) {}

 EnumDeclaration(const Token *name, Enumerator *enumerators):
		ExternalDeclaration(name),
	 	mEnumerators(enumerators)
	{}

	virtual ~EnumDeclaration() {}

	Enumerator *GetEnumeratorList() { return mEnumerators; }
	const Enumerator *GetEnumeratorList() const { return mEnumerators; }

private:
	Enumerator *mEnumerators;
};


class Enumerator: public ParseNode
{
public:
	Enumerator(): mName(0), mValue(0), mNext(0) {}
	Enumerator(const Token *name, Expression *value): mName(name), mValue(value), mNext(0) {}
	virtual ~Enumerator() {}

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
};


class Expression: public ParseNode
{
public:
	Expression() {}
	virtual ~Expression() {}
};


class ConditionalExpression: public Expression
{
public:
	ConditionalExpression() {}

	ConditionalExpression(Expression *condition, Expression *trueExpression, Expression *falseExpression):
		mCondition(condition),
		mTrueExpression(trueExpression),
		mFalseExpression(falseExpression)
	{}

	virtual ~ConditionalExpression() {}

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
	BinaryExpression() {}

	BinaryExpression(const Token *op, Expression *lhs, Expression *rhs):
		mOperator(op),
		mLhs(lhs),
		mRhs(rhs)
	{}

	virtual ~BinaryExpression() {}

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
	UnaryExpression() {}

	UnaryExpression(const Token *op, Expression *rhs):
		mOperator(op),
		mRhs(rhs)
	{}

	virtual ~UnaryExpression() {}

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
	CastExpression() {}

	CastExpression(TypeDescriptor *typeDescriptor, Expression *rhs):
		mTypeDescriptor(typeDescriptor),
		mRhs(rhs)
	{}

	virtual ~CastExpression() {}

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
	SizeofExpression() {}

	SizeofExpression(TypeDescriptor *typeDescriptor):
		mTypeDescriptor(typeDescriptor),
		mRhs(0)
	{}

	SizeofExpression(Expression *rhs):
		mTypeDescriptor(0),
		mRhs(rhs)
	{}

	virtual ~SizeofExpression() {}

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
	ConstantValue() {}
	ConstantValue(const Token *value): mValue(value) {}
	virtual ~ConstantValue() {}

	const Token *GetValue() const { return mValue; }

private:
	const Token *mValue;
};

}

#endif
