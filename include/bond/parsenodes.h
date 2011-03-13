#ifndef BOND_PARSENODES_H
#define BOND_PARSENODES_H

#include "bond/parsenodesfwd.h"
#include "bond/parsenodevisitor.h"
#include "bond/token.h"
#include "bond/typeandvalue.h"

namespace Bond
{

class ParseNode
{
public:
	virtual ~ParseNode() {}

	virtual void Accept(ParseNodeVisitor &visitor) = 0;
	virtual void Accept(ParseNodeVisitor &visitor) const = 0;
	virtual const Token *GetContextToken() const { return 0; }

protected:
	ParseNode() {}
};


class ListParseNode: public ParseNode
{
public:
	virtual ~ListParseNode() {}

	ListParseNode *GetNextNode() { return mNextNode; }
	const ListParseNode *GetNextNode() const { return mNextNode; }
	void SetNextNode(ListParseNode *next) { mNextNode = next; }

protected:
	ListParseNode(): mNextNode(0) {}

private:
	ListParseNode *mNextNode;
};


class Symbol: public ListParseNode
{
public:
	enum SymbolType
	{
		TYPE_NAMESPACE,
		TYPE_STRUCT,
		TYPE_ENUM,
		//TYPE_LOCALSCOPE,
		TYPE_VALUE,
		TYPE_FUNCTION,
	};

	virtual ~Symbol() {}

	virtual const Token *GetContextToken() const { return GetName(); }

	virtual SymbolType GetSymbolType() const = 0;
	virtual const Token *GetName() const = 0;

	virtual TypeAndValue *GetTypeAndValue() { return 0; }
	virtual const TypeAndValue *GetTypeAndValue() const { return 0; }

	Symbol *GetParentSymbol() { return mParentSymbol; }
	const Symbol *GetParentSymbol() const { return mParentSymbol; }
	void SetParentSymbol(Symbol *parent) { mParentSymbol = parent; }

	const Symbol *FindSymbol(const char *name) const;

	Symbol *FindSymbol(const Token *name);
	const Symbol *FindSymbol(const Token *name) const;

	Symbol *FindSymbol(const QualifiedIdentifier *identifier);
	const Symbol *FindSymbol(const QualifiedIdentifier *identifier) const;

	void InsertSymbol(Symbol *symbol);

	bool IsAnonymous() const { return GetName() == 0; }

	bool Matches(bu32_t hashCode, const char *name) const;
	bool Matches(const Token *name) const;
	/*
	Symbol *GetSymbol() { return mSymbol; }
	const Symbol *GetSymbol() const { return mSymbol; }
	void SetSymbol(Symbol *symbol) { mSymbol = symbol; }
	*/
protected:
	Symbol():
		mNextSymbol(0),
		mParentSymbol(0),
		mSymbolList(0)
	{}

private:
	Symbol *FindQualifiedSymbol(const QualifiedIdentifier *identifier);
	const Symbol *FindQualifiedSymbol(const QualifiedIdentifier *identifier) const;

	//Symbol *mSymbol;
	Symbol *mNextSymbol;
	Symbol *mParentSymbol;
	Symbol *mSymbolList;
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

	TypeDescriptor():
		mTypeSpecifier(0),
		mParent(0),
		mLength(0),
		mVariant(VARIANT_VALUE),
		mIsConst(false),
		mIsLValue(false)
	{}

	TypeDescriptor(const TypeSpecifier *specifier, bool isConst):
		mTypeSpecifier(specifier),
		mParent(0),
		mLength(0),
		mVariant(VARIANT_VALUE),
		mIsConst(isConst),
		mIsLValue(false)
	{}

	TypeDescriptor(const TypeDescriptor *parent, bool isConst):
		mTypeSpecifier(0),
		mParent(parent),
		mLength(0),
		mVariant(VARIANT_POINTER),
		mIsConst(isConst),
		mIsLValue(false)
	{}

	TypeDescriptor(const TypeDescriptor *parent, Expression *length):
		mTypeSpecifier(0),
		mParent(parent),
		mLength(length),
		mVariant(VARIANT_ARRAY),
		mIsConst(false),
		mIsLValue(false)
	{}

	virtual ~TypeDescriptor() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const { visitor.Visit(this); }

	virtual const Token *GetContextToken() const;

	const TypeSpecifier *GetTypeSpecifier() const { return mTypeSpecifier; }

	const TypeDescriptor *GetParent() const { return mParent; }

	Expression *GetLength() { return mLength; }
	const Expression *GetLength() const { return mLength; }

	Variant GetVariant() const { return mVariant; }
	void SetVariant(Variant variant) { mVariant = variant; }

	bool IsConst() const { return mIsConst; }

	bool IsLValue() const { return mIsLValue; }
	void SetLValue() { mIsLValue = true; }

	bool IsRValue() const { return !mIsLValue; }
	void SetRValue() { mIsLValue = false; }

	bool IsAssignable() const { return !mIsConst && mIsLValue && (mVariant != VARIANT_ARRAY); }

	Token::TokenType GetPrimitiveType() const;
	bool IsBooleanType() const;
	bool IsIntegerType() const;
	bool IsNumericType() const;
	bool IsPointerType() const { return mVariant != VARIANT_VALUE; }

private:
	const TypeSpecifier *mTypeSpecifier;
	const TypeDescriptor *mParent;
	Expression *mLength;
	Variant mVariant;
	bool mIsConst;
	bool mIsLValue;
};


class TypeSpecifier: public ParseNode
{
public:
	explicit TypeSpecifier(const Token *primitiveType, QualifiedIdentifier *identifier = 0):
		mPrimitiveType(primitiveType),
		mIdentifier(identifier)
	{}

	explicit TypeSpecifier(QualifiedIdentifier *identifier): mPrimitiveType(0), mIdentifier(identifier) {}

	virtual ~TypeSpecifier() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const { visitor.Visit(this); }

	virtual const Token *GetContextToken() const;

	const Token *GetPrimitiveTypeToken() const { return mPrimitiveType; }

	QualifiedIdentifier *GetIdentifier() { return mIdentifier; }
	const QualifiedIdentifier *GetIdentifier() const { return mIdentifier; }

	Token::TokenType GetPrimitiveType() const;
	bool IsBooleanType() const;
	bool IsIntegerType() const;
	bool IsNumericType() const;

private:
	const Token *mPrimitiveType;
	QualifiedIdentifier *mIdentifier;
};


class QualifiedIdentifier: public ListParseNode
{
public:
	explicit QualifiedIdentifier(const Token *name): mName(name) {}
	virtual ~QualifiedIdentifier() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const { visitor.Visit(this); }

	virtual const Token *GetContextToken() const { return mName; }

	const Token *GetName() const { return mName; }

	QualifiedIdentifier *GetNextIdentifier() { return static_cast<QualifiedIdentifier *>(GetNextNode()); }
	const QualifiedIdentifier *GetNextIdentifier() const { return static_cast<const QualifiedIdentifier *>(GetNextNode()); }

	bool IsTerminal() const { return GetNextNode() == 0; }

private:
	const Token *mName;
};


class TranslationUnit: public ListParseNode
{
public:
	explicit TranslationUnit(ListParseNode *declarationList): mDeclarationList(declarationList) {}
	virtual ~TranslationUnit() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const { visitor.Visit(this); }

	ListParseNode *GetExternalDeclarationList() { return mDeclarationList; }
	const ListParseNode *GetExternalDeclarationList() const { return mDeclarationList; }

private:
	ListParseNode *mDeclarationList;
};


class NamespaceDefinition: public Symbol
{
public:
	NamespaceDefinition(const Token *name, ListParseNode *declarationList):
		mName(name),
		mDeclarationList(declarationList)
	{}

	virtual ~NamespaceDefinition() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const { visitor.Visit(this); }

	virtual SymbolType GetSymbolType() const { return TYPE_NAMESPACE; }
	virtual const Token *GetName() const { return mName; }

	ListParseNode *GetExternalDeclarationList() { return mDeclarationList; }
	const ListParseNode *GetExternalDeclarationList() const { return mDeclarationList; }

private:
	const Token *mName;
	ListParseNode *mDeclarationList;
};


class EnumDeclaration: public Symbol
{
public:
	EnumDeclaration(const Token *name):
		mIdentifier(name),
		mTypeSpecifier(&INT_TOKEN, &mIdentifier),
		mTypeDescriptor(&mTypeSpecifier, true),
		mEnumeratorList(0)
	{}

	virtual ~EnumDeclaration() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const { visitor.Visit(this); }

	virtual SymbolType GetSymbolType() const { return TYPE_ENUM; }
	virtual const Token *GetName() const { return mIdentifier.GetName(); }

	const TypeDescriptor *GetTypeDescriptor() const { return &mTypeDescriptor; }

	Enumerator *GetEnumeratorList() { return mEnumeratorList; }
	const Enumerator *GetEnumeratorList() const { return mEnumeratorList; }
	void SetEnumeratorList(Enumerator *enumeratorList) { mEnumeratorList = enumeratorList; }

private:
	QualifiedIdentifier mIdentifier;
	TypeSpecifier mTypeSpecifier;
	TypeDescriptor mTypeDescriptor;
	Enumerator *mEnumeratorList;
};


class Enumerator: public Symbol
{
public:
	Enumerator(const Token *name, const TypeDescriptor *typeDescriptor, Expression *value):
		mTypeAndValue(typeDescriptor),
		mName(name),
		mValue(value) {}
	virtual ~Enumerator() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const { visitor.Visit(this); }

	virtual SymbolType GetSymbolType() const { return TYPE_VALUE; }
	virtual const Token *GetName() const { return mName; }

	virtual TypeAndValue *GetTypeAndValue() { return &mTypeAndValue; }
	virtual const TypeAndValue *GetTypeAndValue() const { return &mTypeAndValue; }

	Expression *GetValue() { return mValue; }
	const Expression *GetValue() const { return mValue; }

private:
	TypeAndValue mTypeAndValue;
	const Token *mName;
	Expression *mValue;
};


class StructDeclaration: public Symbol
{
public:
	explicit StructDeclaration(const Token *name, ListParseNode *memberList):
		mName(name),
		mMemberList(memberList)
	{}

	virtual ~StructDeclaration() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const { visitor.Visit(this); }

	virtual SymbolType GetSymbolType() const { return TYPE_STRUCT; }
	virtual const Token *GetName() const { return mName; }

	ListParseNode *GetMemberList() { return mMemberList; }
	const ListParseNode *GetMemberList() const { return mMemberList; }

private:
	const Token *mName;
	ListParseNode *mMemberList;
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

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const { visitor.Visit(this); }

	virtual const Token *GetContextToken() const { return mName; }

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


class FunctionDefinition: public Symbol
{
public:
	FunctionDefinition(FunctionPrototype *prototype, CompoundStatement *body):
		mIdentifier(prototype->GetName()),
		mTypeSpecifier(&INT_TOKEN, &mIdentifier),
		mTypeDescriptor(&mTypeSpecifier, true),
		mPrototype(prototype),
		mBody(body),
		mNextDefinition(0)
	{}

	virtual ~FunctionDefinition() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const { visitor.Visit(this); }

	virtual SymbolType GetSymbolType() const { return TYPE_FUNCTION; }
	virtual const Token *GetName() const { return mIdentifier.GetName(); }

	const TypeDescriptor *GetTypeDescriptor() const { return &mTypeDescriptor; }

	FunctionPrototype *GetPrototype() { return mPrototype; }
	const FunctionPrototype *GetPrototype() const { return mPrototype; }

	CompoundStatement *GetBody() { return mBody; }
	const CompoundStatement *GetBody() const { return mBody; }

	FunctionDefinition *GetNextDefinition() { return mNextDefinition; }
	const FunctionDefinition *GetNextDefinition() const { return mNextDefinition; }
	void AppendDefinition(FunctionDefinition *next) {	next->mNextDefinition = mNextDefinition; mNextDefinition = next; }

	bool IsDeclaration() const { return mBody == 0; }

private:
	QualifiedIdentifier mIdentifier;
	TypeSpecifier mTypeSpecifier;
	TypeDescriptor mTypeDescriptor;
	FunctionPrototype *mPrototype;
	CompoundStatement *mBody;
	FunctionDefinition *mNextDefinition;
};


class Parameter: public ListParseNode
{
public:
	Parameter(const Token *name, TypeDescriptor *typeDescriptor): mName(name), mTypeDescriptor(typeDescriptor) {}

	virtual ~Parameter() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const { visitor.Visit(this); }

	virtual const Token *GetContextToken() const { return mName; }

	const Token *GetName() const { return mName; }

	TypeDescriptor *GetTypeDescriptor() { return mTypeDescriptor; }
	const TypeDescriptor *GetTypeDescriptor() const { return mTypeDescriptor; }

private:
	const Token *mName;
	TypeDescriptor *mTypeDescriptor;
};


class NamedInitializer: public Symbol
{
public:
	explicit NamedInitializer(const Token *name, Initializer *initializer, TypeDescriptor *typeDescriptor):
		mTypeAndValue(typeDescriptor),
		mName(name),
		mInitializer(initializer)
	{}

	virtual ~NamedInitializer() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const { visitor.Visit(this); }

	virtual SymbolType GetSymbolType() const { return TYPE_VALUE; }
	virtual const Token *GetName() const { return mName; }

	virtual TypeAndValue *GetTypeAndValue() { return &mTypeAndValue; }
	virtual const TypeAndValue *GetTypeAndValue() const { return &mTypeAndValue; }

	Initializer *GetInitializer() { return mInitializer; }
	const Initializer *GetInitializer() const { return mInitializer; }

private:
	TypeAndValue mTypeAndValue;
	const Token *mName;
	Initializer *mInitializer;
};


class Initializer: public ListParseNode
{
public:
	explicit Initializer(Expression *expression):
		mExpression(expression),
		mInitializerList(0)
	{}

	explicit Initializer(Initializer *initializerList):
		mExpression(0),
		mInitializerList(initializerList)
	{}

	virtual ~Initializer() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const { visitor.Visit(this); }

	Expression *GetExpression() { return mExpression; }
	const Expression *GetExpression() const { return mExpression; }

	Initializer *GetInitializerList() { return mInitializerList; }
	const Initializer *GetInitializerList() const { return mInitializerList; }

private:
	Expression *mExpression;
	Initializer *mInitializerList;
};


class CompoundStatement: public ListParseNode
{
public:
	explicit CompoundStatement(ListParseNode *statementList): mStatementList(statementList) {}
	virtual ~CompoundStatement() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const { visitor.Visit(this); }

	ListParseNode *GetStatementList() { return mStatementList; }
	const ListParseNode *GetStatementList() const { return mStatementList; }

private:
	ListParseNode *mStatementList;
};


class IfStatement: public ListParseNode
{
public:
	IfStatement(Expression *condition, ParseNode *thenStatement, ParseNode *elseStatement):
		mCondition(condition),
		mThenStatement(thenStatement),
		mElseStatement(elseStatement)
	{}

	virtual ~IfStatement() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const { visitor.Visit(this); }

	Expression *GetCondition() { return mCondition; }
	const Expression *GetCondition() const { return mCondition; }

	ParseNode *GetThenStatement() { return mThenStatement; }
	const ParseNode *GetThenStatement() const { return mThenStatement; }

	ParseNode *GetElseStatement() { return mElseStatement; }
	const ParseNode *GetElseStatement() const { return mElseStatement; }

private:
	Expression *mCondition;
	ParseNode *mThenStatement;
	ParseNode *mElseStatement;
};


class SwitchStatement: public ListParseNode
{
public:
	SwitchStatement(Expression *control, SwitchSection *sectionList):
		mControl(control),
		mSectionList(sectionList)
	{}

	virtual ~SwitchStatement() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const { visitor.Visit(this); }

	Expression *GetControl() { return mControl; }
	const Expression *GetControl() const { return mControl; }

	SwitchSection *GetSectionList() { return mSectionList; }
	const SwitchSection *GetSectionList() const { return mSectionList; }

private:
	Expression *mControl;
	SwitchSection *mSectionList;
};


class SwitchSection: public ListParseNode
{
public:
	SwitchSection(SwitchLabel *labelList, ListParseNode* statementList):
		mLabelList(labelList),
		mStatementList(statementList)
	{}

	virtual ~SwitchSection() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const { visitor.Visit(this); }

	SwitchLabel *GetLabelList() { return mLabelList; }
	const SwitchLabel *GetLabelList() const { return mLabelList; }

	ListParseNode *GetStatementList() { return mStatementList; }
	const ListParseNode *GetStatementList() const { return mStatementList; }

private:
	SwitchLabel *mLabelList;
	ListParseNode *mStatementList;
};


class SwitchLabel: public ListParseNode
{
public:
	enum Variant
	{
		VARIANT_CASE,
		VARIANT_DEFAULT,
	};

	explicit SwitchLabel(const Token *label):
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

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const { visitor.Visit(this); }

	virtual const Token *GetContextToken() const { return mLabel; }

	const Token *GetLabel() const { return mLabel; }

	Expression *GetExpression() { return mExpression; }
	const Expression *GetExpression() const { return mExpression; }

	Variant GetVariant() const { return mVariant; }

private:
	const Token *mLabel;
	Expression *mExpression;
	Variant mVariant;
};


class WhileStatement: public ListParseNode
{
public:
	enum Variant
	{
		VARIANT_WHILE,
		VARIANT_DO_WHILE,
	};

	WhileStatement(Expression *condition, ParseNode *body, Variant variant):
		mCondition(condition),
		mBody(body),
		mVariant(variant)
	{}

	virtual ~WhileStatement() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const { visitor.Visit(this); }

	Expression *GetCondition() { return mCondition; }
	const Expression *GetCondition() const { return mCondition; }

	ParseNode *GetBody() { return mBody; }
	const ParseNode *GetBody() const { return mBody; }

	Variant GetVariant() const { return mVariant; }

private:
	Expression *mCondition;
	ParseNode *mBody;
	Variant mVariant;
};


class ForStatement: public ListParseNode
{
public:
	ForStatement(
			ParseNode *initializer,
			Expression *condition,
			Expression *countingExpression,
			ParseNode *body):
		mInitializer(initializer),
		mCondition(condition),
		mCountingExpression(countingExpression),
		mBody(body)
	{}

	virtual ~ForStatement() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const { visitor.Visit(this); }

	ParseNode *GetInitializer() { return mInitializer; }
	const ParseNode *GetInitializer() const { return mInitializer; }

	Expression *GetCondition() { return mCondition; }
	const Expression *GetCondition() const { return mCondition; }

	Expression *GetCountingExpression() { return mCountingExpression; }
	const Expression *GetCountingExpression() const { return mCountingExpression; }

	ParseNode *GetBody() { return mBody; }
	const ParseNode *GetBody() const { return mBody; }

private:
	ParseNode *mInitializer;
	Expression *mCondition;
	Expression *mCountingExpression;
	ParseNode *mBody;
};


class JumpStatement: public ListParseNode
{
public:
	JumpStatement(const Token *op, Expression *rhs):	mOperator(op), mRhs(rhs) {}
	virtual ~JumpStatement() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const { visitor.Visit(this); }

	virtual const Token *GetContextToken() const { return mOperator; }

	const Token *GetOperator() const { return mOperator; }

	Expression *GetRhs() { return mRhs; }
	const Expression *GetRhs() const { return mRhs; }

private:
	const Token *mOperator;
	Expression *mRhs;
};


class DeclarativeStatement: public ListParseNode
{
public:
	DeclarativeStatement(TypeDescriptor *typeDescriptor, NamedInitializer *initializerList):
		mTypeDescriptor(typeDescriptor),
		mNamedInitializerList(initializerList)
	{}

	virtual ~DeclarativeStatement() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const { visitor.Visit(this); }

	const TypeDescriptor *GetTypeDescriptor() const { return mTypeDescriptor; }
	TypeDescriptor *GetTypeDescriptor() { return mTypeDescriptor; }

	const NamedInitializer *GetNamedInitializerList() const { return mNamedInitializerList; }
	NamedInitializer *GetNamedInitializerList() { return mNamedInitializerList; }

private:
	TypeDescriptor *mTypeDescriptor;
	NamedInitializer *mNamedInitializerList;
};


class ExpressionStatement: public ListParseNode
{
public:
	explicit ExpressionStatement(Expression *expression): mExpression(expression) {}
	virtual ~ExpressionStatement() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const { visitor.Visit(this); }

	//virtual const Token *GetContextToken() const;

	Expression *GetExpression() { return mExpression; }
	const Expression *GetExpression() const { return mExpression; }

private:
	Expression *mExpression;
};


class Expression: public ListParseNode
{
public:
	virtual ~Expression() {}

	TypeAndValue &GetTypeAndValue() { return mTypeAndValue; }
	const TypeAndValue &GetTypeAndValue() const { return mTypeAndValue; }

protected:
	Expression() {}

private:
	TypeAndValue mTypeAndValue;
};


// Expression that results in a temporary value that may differ from its operand type, thus requiring
// the instantiation of a new TypeDescriptor to describe the resulting type.
class TemporaryExpression: public Expression
{
public:
	virtual ~TemporaryExpression() {}

	const TypeDescriptor *GetTypeDescriptor() const { return &mTypeDescriptor; }
	void SetTypeDescriptor(const TypeDescriptor &descriptor) { mTypeDescriptor = descriptor; }

private:
	TypeDescriptor mTypeDescriptor;
};


class ConditionalExpression: public TemporaryExpression
{
public:
	ConditionalExpression(Expression *condition, Expression *trueExpression, Expression *falseExpression):
		mCondition(condition),
		mTrueExpression(trueExpression),
		mFalseExpression(falseExpression)
	{}

	virtual ~ConditionalExpression() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const { visitor.Visit(this); }

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


class BinaryExpression: public TemporaryExpression
{
public:
	BinaryExpression(const Token *op, Expression *lhs, Expression *rhs):
		mOperator(op),
		mLhs(lhs),
		mRhs(rhs)
	{}

	virtual ~BinaryExpression() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const { visitor.Visit(this); }

	virtual const Token *GetContextToken() const { return mOperator; }

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


class UnaryExpression: public TemporaryExpression
{
public:
	UnaryExpression(const Token *op, Expression *rhs): mOperator(op), mRhs(rhs) {}
	virtual ~UnaryExpression() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const { visitor.Visit(this); }

	virtual const Token *GetContextToken() const { return mOperator; }

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

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const { visitor.Visit(this); }

	virtual const Token *GetContextToken() const { return mOperator; }

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

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const { visitor.Visit(this); }

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
	ArraySubscriptExpression(const Token *op, Expression *lhs, Expression *index):
		mOperator(op),
		mLhs(lhs),
		mIndex(index)
	{}

	virtual ~ArraySubscriptExpression() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const { visitor.Visit(this); }

	virtual const Token *GetContextToken() const { return mOperator; }

	const Token *GetOperator() const { return mOperator; }

	Expression *GetLhs() { return mLhs; }
	const Expression *GetLhs() const { return mLhs; }

	Expression *GetIndex() { return mIndex; }
	const Expression *GetIndex() const { return mIndex; }

private:
	const Token *mOperator;
	Expression *mLhs;
	Expression *mIndex;
};


class FunctionCallExpression: public Expression
{
public:
	FunctionCallExpression(Expression *lhs, Expression *argumentList): mLhs(lhs), mArgumentList(argumentList) {}
	virtual ~FunctionCallExpression() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const { visitor.Visit(this); }

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

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const { visitor.Visit(this); }

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
	explicit SizeofExpression(TypeDescriptor *typeDescriptor): mTypeDescriptor(typeDescriptor), mRhs(0) {}
	explicit SizeofExpression(Expression *rhs): mTypeDescriptor(0), mRhs(rhs) {}
	virtual ~SizeofExpression() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const { visitor.Visit(this); }

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

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const { visitor.Visit(this); }

	virtual const Token *GetContextToken() const { return mValue; }

	const Token *GetValueToken() const { return mValue; }

private:
	const Token *mValue;
};


class IdentifierExpression: public Expression
{
public:
	explicit IdentifierExpression(QualifiedIdentifier *identifier): mIdentifier(identifier) {}
	virtual ~IdentifierExpression() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const { visitor.Visit(this); }

	virtual const Token *GetContextToken() const;

	QualifiedIdentifier *GetIdentifier() { return mIdentifier; }
	const QualifiedIdentifier *GetIdentifier() const { return mIdentifier; }

private:
	QualifiedIdentifier *mIdentifier;
};


extern const TypeSpecifier BOOL_TYPE_SPECIFIER;
extern const TypeSpecifier CHAR_TYPE_SPECIFIER;
extern const TypeSpecifier INT_TYPE_SPECIFIER;
extern const TypeSpecifier UINT_TYPE_SPECIFIER;
extern const TypeSpecifier FLOAT_TYPE_SPECIFIER;

extern const TypeDescriptor CONST_BOOL_TYPE_DESCRIPTOR;
extern const TypeDescriptor CONST_CHAR_TYPE_DESCRIPTOR;
extern const TypeDescriptor CONST_INT_TYPE_DESCRIPTOR;
extern const TypeDescriptor CONST_UINT_TYPE_DESCRIPTOR;
extern const TypeDescriptor CONST_FLOAT_TYPE_DESCRIPTOR;
extern const TypeDescriptor CONST_STRING_TYPE_DESCRIPTOR;

}

#endif
