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
	virtual const Token *GetContextToken() const = 0;

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


template <typename NodeType>
class ParseNodeList
{
public:
	ParseNodeList(): mHead(0), mTail(0) {}

	NodeType *GetHead() const { return mHead; }

	void Append(NodeType *node)
	{
		if (node != 0)
		{
			if (mHead == 0)
			{
				mHead = node;
			}
			else
			{
				mTail->SetNextNode(node);
			}
			mTail = node;
		}
	}

private:
	NodeType *mHead;
	NodeType *mTail;
};


class Symbol: public ListParseNode
{
public:
	enum SymbolType
	{
		TYPE_NAMESPACE,
		TYPE_STRUCT,
		TYPE_ENUM,
		TYPE_LOCALSCOPE,
		TYPE_VALUE,
		TYPE_FUNCTION,
	};

	virtual ~Symbol() {}

	virtual const Token *GetContextToken() const { return GetName(); }

	virtual SymbolType GetSymbolType() const = 0;
	virtual const Token *GetName() const { return 0; }

	virtual TypeAndValue *GetTypeAndValue() { return 0; }
	virtual const TypeAndValue *GetTypeAndValue() const { return 0; }

	virtual bool IsResolved() const { return true; }

	bool IsTypeDefinition() const;

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

protected:
	Symbol():
		mNextSymbol(0),
		mParentSymbol(0),
		mSymbolList(0)
	{}

private:
	Symbol *FindQualifiedSymbol(const QualifiedIdentifier *identifier);
	const Symbol *FindQualifiedSymbol(const QualifiedIdentifier *identifier) const;

	Symbol *mNextSymbol;
	Symbol *mParentSymbol;
	Symbol *mSymbolList;
};


class TypeDescriptor: public ParseNode
{
public:
	TypeDescriptor():
		mTypeSpecifier(0),
		mParent(0),
		mLengthExpressionList(0),
		mFlags(0)
	{}

	TypeDescriptor(const TypeSpecifier *specifier, bool isConst):
		mTypeSpecifier(specifier),
		mParent(0),
		mLengthExpressionList(0),
		mFlags(FLAG_VALUE | (isConst ? FLAG_CONST : 0))
	{}

	TypeDescriptor(TypeDescriptor *parent, bool isConst):
		mTypeSpecifier(0),
		mParent(parent),
		mLengthExpressionList(0),
		mFlags(FLAG_POINTER | (isConst ? FLAG_CONST : 0))
	{}

	virtual ~TypeDescriptor() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const { visitor.Visit(this); }

	virtual const Token *GetContextToken() const;

	TypeSpecifier *GetTypeSpecifier() { return const_cast<TypeSpecifier *>(mTypeSpecifier); }
	const TypeSpecifier *GetTypeSpecifier() const { return mTypeSpecifier; }

	TypeDescriptor *GetParent() { return mParent; }
	const TypeDescriptor *GetParent() const { return mParent; }
	void SetParent(TypeDescriptor *parent) { mParent = parent; }

	Expression *GetLengthExpressionList() { return mLengthExpressionList; }
	const Expression *GetLengthExpressionList() const { return mLengthExpressionList; }

	void ConvertToArray(Expression *expressionList);
	void ConvertToPointerIntrinsic();

	TypeDescriptor GetDereferencedType() const;
	TypeDescriptor GetArrayElementType() const;

	bool IsDefined() const { return (mFlags & STORAGE_MASK) != 0; }
	bool IsResolved() const;

	bool IsConst() const { return (mFlags & FLAG_CONST) != 0; }
	void SetConst() { mFlags |= FLAG_CONST; }

	bool IsLValue() const { return (mFlags & FLAG_LVALUE) != 0; }
	void SetLValue() { mFlags |= FLAG_LVALUE; }

	bool IsRValue() const { return (mFlags & FLAG_LVALUE) == 0; }
	void SetRValue() { mFlags &= ~FLAG_LVALUE; }

	bool IsAssignable() const { return IsLValue() && !IsConst() && !IsArrayType(); }

	bu32_t GetSize(bu32_t pointerSize) const;
	bu32_t GetAlignment() const;

	Token::TokenType GetPrimitiveType() const;
	bool IsBooleanType() const;
	bool IsIntegerType() const;
	bool IsNumericType() const;
	bool IsValueType() const { return (mFlags & FLAG_VALUE) != 0; }
	bool IsArrayType() const { return (mFlags & FLAG_ARRAY) != 0; }
	bool IsNullType() const { return (mFlags & FLAG_NULL) != 0; }
	bool IsPointerIntrinsicType() const { return (mFlags & FLAG_POINTER) != 0; }
	bool IsPointerType() const { return  (mFlags & FLAG_ANY_POINTER) != 0; }
	bool IsVoidType() const;

	static TypeDescriptor GetBoolType();
	static TypeDescriptor GetCharType();
	static TypeDescriptor GetIntType();
	static TypeDescriptor GetUIntType();
	static TypeDescriptor GetFloatType();
	static TypeDescriptor GetStringType();
	static TypeDescriptor GetPointerType(const TypeSpecifier *specifier);
	static TypeDescriptor GetConstPointerType(const TypeSpecifier *specifier);
	static TypeDescriptor GetNullType();

private:
	TypeDescriptor(const TypeSpecifier *specifier, bu32_t flags):
		mTypeSpecifier(specifier),
		mParent(0),
		mLengthExpressionList(0),
		mFlags(flags)
	{}

	bool HasFlattenedParent() const { return ((mFlags >> PARENT_SHIFT) & STORAGE_MASK) != 0; }

	static const bu32_t FLAG_VALUE = 1 << 0;
	static const bu32_t FLAG_POINTER = 1 << 1;
	static const bu32_t FLAG_ARRAY = 1 << 2;
	static const bu32_t FLAG_NULL = 1 << 3;
	static const bu32_t FLAG_CONST = 1 << 4;
	static const bu32_t FLAG_LVALUE = 1 << 5;
	static const bu32_t FLAG_ANY_POINTER = FLAG_POINTER | FLAG_ARRAY | FLAG_NULL;
	static const bu32_t PARENT_SHIFT = 8;
	static const bu32_t STORAGE_MASK = FLAG_VALUE | FLAG_ANY_POINTER;
	static const bu32_t FLAG_MASK = (1 << PARENT_SHIFT) - 1;

	const TypeSpecifier *mTypeSpecifier;
	TypeDescriptor *mParent;
	Expression *mLengthExpressionList;
	bu32_t mFlags;
};


class TypeSpecifier: public ParseNode
{
public:

	explicit TypeSpecifier(const Token *primitiveType, QualifiedIdentifier *identifier = 0):
		mPrimitiveType(primitiveType),
		mIdentifier(identifier),
		mDefinition(0)
	{}

	explicit TypeSpecifier(QualifiedIdentifier *identifier):
		mPrimitiveType(0),
		mIdentifier(identifier),
		mDefinition(0)
	{}

	TypeSpecifier(const Token *primitiveType, QualifiedIdentifier *identifier, const Symbol *definition):
		mPrimitiveType(primitiveType),
		mIdentifier(identifier),
		mDefinition(definition)
	{}

	virtual ~TypeSpecifier() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const { visitor.Visit(this); }

	virtual const Token *GetContextToken() const;

	const Token *GetPrimitiveTypeToken() const { return mPrimitiveType; }

	QualifiedIdentifier *GetIdentifier() { return mIdentifier; }
	const QualifiedIdentifier *GetIdentifier() const { return mIdentifier; }

	const Symbol *GetDefinition() const { return mDefinition; }
	void SetDefinition(const Symbol *symbol) { mDefinition = symbol; }

	bool IsResolved() const;

	bu32_t GetSize(bu32_t pointerSize) const;
	bu32_t GetAlignment() const;

	Token::TokenType GetPrimitiveType() const;
	bool IsBooleanType() const;
	bool IsIntegerType() const;
	bool IsNumericType() const;
	bool IsVoidType() const;

private:
	const Token *mPrimitiveType;
	QualifiedIdentifier *mIdentifier;
	const Symbol *mDefinition;
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

	virtual const Token *GetContextToken() const { return 0; }

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
		mTypeSpecifier(&INT_TOKEN, &mIdentifier, this),
		mTypeDescriptor(&mTypeSpecifier, true),
		mEnumeratorList(0)
	{}

	virtual ~EnumDeclaration() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const { visitor.Visit(this); }

	virtual SymbolType GetSymbolType() const { return TYPE_ENUM; }
	virtual const Token *GetName() const { return mIdentifier.GetName(); }

	TypeDescriptor *GetTypeDescriptor() { return &mTypeDescriptor; }
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
	Enumerator(const Token *name, TypeDescriptor *typeDescriptor, Expression *value):
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
	enum Variant
	{
		VARIANT_BOND,
		VARIANT_NATIVE,
		VARIANT_REFERENCE,
	};

	explicit StructDeclaration(
			const Token *name,
			const Token *size,
			const Token *alignment,
			FunctionDefinition *memberFunctionList,
			DeclarativeStatement *memberVariableList,
			Variant variant):
		mIdentifier(name),
		mTypeSpecifier(0, &mIdentifier, this),
		mThisTypeDescriptor(TypeDescriptor::GetPointerType(&mTypeSpecifier)),
		mConstThisTypeDescriptor(TypeDescriptor::GetConstPointerType(&mTypeSpecifier)),
		mSizeToken(size),
		mAlignmentToken(alignment),
		mMemberFunctionList(memberFunctionList),
		mMemberVariableList(memberVariableList),
		mVariant(variant),
		mSize(0),
		mAlignment(BOND_DEFAULT_STRUCT_ALIGN)
	{}

	virtual ~StructDeclaration() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const { visitor.Visit(this); }

	virtual SymbolType GetSymbolType() const { return TYPE_STRUCT; }
	virtual const Token *GetName() const { return mIdentifier.GetName(); }

	virtual bool IsResolved() const;

	TypeDescriptor *GetThisTypeDescriptor() { return &mThisTypeDescriptor; }
	const TypeDescriptor *GetThisTypeDescriptor() const { return &mThisTypeDescriptor; }

	TypeDescriptor *GetConstThisTypeDescriptor() { return &mConstThisTypeDescriptor; }
	const TypeDescriptor *GetConstThisTypeDescriptor() const { return &mConstThisTypeDescriptor; }

	const Token *GetSizeToken() const { return mSizeToken; }
	const Token *GetAlignmentToken() const { return mAlignmentToken; }

	FunctionDefinition *GetMemberFunctionList() { return mMemberFunctionList; }
	const FunctionDefinition *GetMemberFunctionList() const { return mMemberFunctionList; }

	DeclarativeStatement *GetMemberVariableList() { return mMemberVariableList; }
	const DeclarativeStatement *GetMemberVariableList() const { return mMemberVariableList; }
	void SetMemberVariableList(DeclarativeStatement *memberVariableList) { mMemberVariableList = memberVariableList; }

	Variant GetVariant() const { return mVariant; }

	bu32_t GetSize() const { return mSize; }
	void SetSize(bu32_t size) { mSize = size; }

	bu32_t GetAlignment() const { return mAlignment; }
	void SetAlignment(bu32_t alignment) { mAlignment = alignment; }

private:
	QualifiedIdentifier mIdentifier;
	TypeSpecifier mTypeSpecifier;
	TypeDescriptor mThisTypeDescriptor;
	TypeDescriptor mConstThisTypeDescriptor;
	const Token *mSizeToken;
	const Token *mAlignmentToken;
	FunctionDefinition *mMemberFunctionList;
	DeclarativeStatement *mMemberVariableList;
	Variant mVariant;
	bu32_t mSize;
	bu32_t mAlignment;
};


class FunctionPrototype: public ParseNode
{
public:
	FunctionPrototype(const Token *name, TypeDescriptor *returnType, Parameter *parameterList, bool isConst):
		mName(name),
		mReturnType(returnType),
		mParameterList(parameterList),
		mIsConst(isConst)
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

	bool IsConst() const { return mIsConst; }

private:
	const Token *mName;
	TypeDescriptor *mReturnType;
	Parameter *mParameterList;
	bool mIsConst;
};


class FunctionDefinition: public Symbol
{
public:
	FunctionDefinition(FunctionPrototype *prototype, CompoundStatement *body):
		mIdentifier(prototype->GetName()),
		mTypeSpecifier(0, &mIdentifier, this),
		mTypeDescriptor(&mTypeSpecifier, true),
		mTypeAndValue(&mTypeDescriptor),
		mPrototype(prototype),
		mBody(body)
	{}

	virtual ~FunctionDefinition() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const { visitor.Visit(this); }

	virtual SymbolType GetSymbolType() const { return TYPE_FUNCTION; }
	virtual const Token *GetName() const { return mIdentifier.GetName(); }

	virtual TypeAndValue *GetTypeAndValue() { return &mTypeAndValue; }
	virtual const TypeAndValue *GetTypeAndValue() const { return &mTypeAndValue; }

	FunctionPrototype *GetPrototype() { return mPrototype; }
	const FunctionPrototype *GetPrototype() const { return mPrototype; }

	CompoundStatement *GetBody() { return mBody; }
	const CompoundStatement *GetBody() const { return mBody; }

	bool IsDeclaration() const { return mBody == 0; }

private:
	QualifiedIdentifier mIdentifier;
	TypeSpecifier mTypeSpecifier;
	TypeDescriptor mTypeDescriptor;
	TypeAndValue mTypeAndValue;
	FunctionPrototype *mPrototype;
	CompoundStatement *mBody;
};


class Parameter: public Symbol
{
public:
	Parameter(const Token *name, TypeDescriptor *typeDescriptor):
		mTypeAndValue(typeDescriptor),
		mName(name),
		mTypeDescriptor(typeDescriptor),
		mOffset(0)
	{}

	virtual ~Parameter() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const { visitor.Visit(this); }

	virtual SymbolType GetSymbolType() const { return TYPE_VALUE; }
	virtual const Token *GetName() const { return mName; }

	virtual TypeAndValue *GetTypeAndValue() { return &mTypeAndValue; }
	virtual const TypeAndValue *GetTypeAndValue() const { return &mTypeAndValue; }

	TypeDescriptor *GetTypeDescriptor() { return mTypeDescriptor; }
	const TypeDescriptor *GetTypeDescriptor() const { return mTypeDescriptor; }

	bi32_t GetOffset() const { return mOffset; }
	void SetOffset(bi32_t offset) { mOffset = offset; }

private:
	TypeAndValue mTypeAndValue;
	const Token *mName;
	TypeDescriptor *mTypeDescriptor;
	bi32_t mOffset;
};


class NamedInitializer: public Symbol
{
public:
	NamedInitializer(const Token *name, Initializer *initializer, TypeDescriptor *typeDescriptor):
		mTypeAndValue(typeDescriptor),
		mName(name),
		mInitializer(initializer),
		mOffset(0)
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

	bi32_t GetOffset() const { return mOffset; }
	void SetOffset(bi32_t offset) { mOffset = offset; }

private:
	TypeAndValue mTypeAndValue;
	const Token *mName;
	Initializer *mInitializer;
	bi32_t mOffset;
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

	virtual const Token *GetContextToken() const;

	Expression *GetExpression() { return mExpression; }
	const Expression *GetExpression() const { return mExpression; }

	Initializer *GetInitializerList() { return mInitializerList; }
	const Initializer *GetInitializerList() const { return mInitializerList; }

	bool IsResolved() const;

private:
	Expression *mExpression;
	Initializer *mInitializerList;
};


class CompoundStatement: public Symbol
{
public:
	explicit CompoundStatement(ListParseNode *statementList): mStatementList(statementList) {}
	virtual ~CompoundStatement() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const { visitor.Visit(this); }

	virtual SymbolType GetSymbolType() const { return TYPE_LOCALSCOPE; }

	ListParseNode *GetStatementList() { return mStatementList; }
	const ListParseNode *GetStatementList() const { return mStatementList; }

private:
	ListParseNode *mStatementList;
};


class IfStatement: public ListParseNode
{
public:
	IfStatement(const Token *keyword, Expression *condition, ParseNode *thenStatement, ParseNode *elseStatement):
		mKeyword(keyword),
		mCondition(condition),
		mThenStatement(thenStatement),
		mElseStatement(elseStatement)
	{}

	virtual ~IfStatement() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const { visitor.Visit(this); }

	virtual const Token *GetContextToken() const { return mKeyword; }

	const Token *GetKeyword() const { return mKeyword; }

	Expression *GetCondition() { return mCondition; }
	const Expression *GetCondition() const { return mCondition; }

	ParseNode *GetThenStatement() { return mThenStatement; }
	const ParseNode *GetThenStatement() const { return mThenStatement; }

	ParseNode *GetElseStatement() { return mElseStatement; }
	const ParseNode *GetElseStatement() const { return mElseStatement; }

private:
	const Token *mKeyword;
	Expression *mCondition;
	ParseNode *mThenStatement;
	ParseNode *mElseStatement;
};


class SwitchStatement: public ListParseNode
{
public:
	SwitchStatement(const Token *keyword, Expression *control, SwitchSection *sectionList):
		mKeyword(keyword),
		mControl(control),
		mSectionList(sectionList)
	{}

	virtual ~SwitchStatement() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const { visitor.Visit(this); }

	virtual const Token *GetContextToken() const { return mKeyword; }

	const Token *GetKeyword() const { return mKeyword; }

	Expression *GetControl() { return mControl; }
	const Expression *GetControl() const { return mControl; }

	SwitchSection *GetSectionList() { return mSectionList; }
	const SwitchSection *GetSectionList() const { return mSectionList; }

private:
	const Token *mKeyword;
	Expression *mControl;
	SwitchSection *mSectionList;
};


class SwitchSection: public Symbol
{
public:
	SwitchSection(SwitchLabel *labelList, ListParseNode* statementList):
		mLabelList(labelList),
		mStatementList(statementList)
	{}

	virtual ~SwitchSection() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const { visitor.Visit(this); }

	virtual SymbolType GetSymbolType() const { return TYPE_LOCALSCOPE; }

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
	explicit SwitchLabel(const Token *label):
		mLabel(label),
		mExpression(0)
	{}

	SwitchLabel(const Token *label, Expression *expression):
		mLabel(label),
		mExpression(expression)
	{}

	virtual ~SwitchLabel() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const { visitor.Visit(this); }

	virtual const Token *GetContextToken() const { return mLabel; }

	const Token *GetLabel() const { return mLabel; }

	Expression *GetExpression() { return mExpression; }
	const Expression *GetExpression() const { return mExpression; }

private:
	const Token *mLabel;
	Expression *mExpression;
};


class WhileStatement: public ListParseNode
{
public:
	WhileStatement(const Token *keyword, Expression *condition, ParseNode *body):
		mKeyword(keyword),
		mCondition(condition),
		mBody(body)
	{}

	virtual ~WhileStatement() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const { visitor.Visit(this); }

	virtual const Token *GetContextToken() const { return mKeyword; }

	const Token *GetKeyword() const { return mKeyword; }

	Expression *GetCondition() { return mCondition; }
	const Expression *GetCondition() const { return mCondition; }

	ParseNode *GetBody() { return mBody; }
	const ParseNode *GetBody() const { return mBody; }

	bool IsDoLoop() const { return mKeyword->GetTokenType() == Token::KEY_DO; }

private:
	const Token *mKeyword;
	Expression *mCondition;
	ParseNode *mBody;
};


class ForStatement: public Symbol
{
public:
	ForStatement(
			const Token *keyword,
			ParseNode *initializer,
			Expression *condition,
			Expression *countingExpression,
			ParseNode *body):
		mKeyword(keyword),
		mInitializer(initializer),
		mCondition(condition),
		mCountingExpression(countingExpression),
		mBody(body)
	{}

	virtual ~ForStatement() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const { visitor.Visit(this); }

	virtual SymbolType GetSymbolType() const { return TYPE_LOCALSCOPE; }

	virtual const Token *GetContextToken() const { return mKeyword; }

	const Token *GetKeyword() const { return mKeyword; }

	ParseNode *GetInitializer() { return mInitializer; }
	const ParseNode *GetInitializer() const { return mInitializer; }

	Expression *GetCondition() { return mCondition; }
	const Expression *GetCondition() const { return mCondition; }

	Expression *GetCountingExpression() { return mCountingExpression; }
	const Expression *GetCountingExpression() const { return mCountingExpression; }

	ParseNode *GetBody() { return mBody; }
	const ParseNode *GetBody() const { return mBody; }

private:
	const Token *mKeyword;
	ParseNode *mInitializer;
	Expression *mCondition;
	Expression *mCountingExpression;
	ParseNode *mBody;
};


class JumpStatement: public ListParseNode
{
public:
	JumpStatement(const Token *keyword, Expression *rhs): mKeyword(keyword), mRhs(rhs) {}
	virtual ~JumpStatement() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const { visitor.Visit(this); }

	virtual const Token *GetContextToken() const { return mKeyword; }

	const Token *GetKeyword() const { return mKeyword; }

	Expression *GetRhs() { return mRhs; }
	const Expression *GetRhs() const { return mRhs; }

	bool IsBreak() const { return mKeyword->GetTokenType() == Token::KEY_BREAK; }
	bool IsContinue() const { return mKeyword->GetTokenType() == Token::KEY_CONTINUE; }
	bool IsReturn() const { return mKeyword->GetTokenType() == Token::KEY_RETURN; }

private:
	const Token *mKeyword;
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

	virtual const Token *GetContextToken() const { return mTypeDescriptor->GetContextToken(); }

	const TypeDescriptor *GetTypeDescriptor() const { return mTypeDescriptor; }
	TypeDescriptor *GetTypeDescriptor() { return mTypeDescriptor; }

	const NamedInitializer *GetNamedInitializerList() const { return mNamedInitializerList; }
	NamedInitializer *GetNamedInitializerList() { return mNamedInitializerList; }

private:
	TypeDescriptor *mTypeDescriptor;
	NamedInitializer *mNamedInitializerList;
};


class Expression: public ListParseNode
{
public:
	virtual ~Expression() {}

	const TypeDescriptor *GetTypeDescriptor() const { return &mTypeDescriptor; }
	void SetTypeDescriptor(const TypeDescriptor &descriptor) { mTypeDescriptor = descriptor; }

	TypeAndValue &GetTypeAndValue() { return mTypeAndValue; }
	const TypeAndValue &GetTypeAndValue() const { return mTypeAndValue; }

protected:
	Expression(): mTypeAndValue(&mTypeDescriptor) {}

private:
	TypeDescriptor mTypeDescriptor;
	TypeAndValue mTypeAndValue;
};


class ExpressionStatement: public ListParseNode
{
public:
	explicit ExpressionStatement(Expression *expression): mExpression(expression) {}
	virtual ~ExpressionStatement() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const { visitor.Visit(this); }

	virtual const Token *GetContextToken() const { return (mExpression == 0) ? 0 : mExpression->GetContextToken(); }

	Expression *GetExpression() { return mExpression; }
	const Expression *GetExpression() const { return mExpression; }

private:
	Expression *mExpression;
};


class ConditionalExpression: public Expression
{
public:
	ConditionalExpression(
			const Token *op,
			Expression *condition,
			Expression *trueExpression,
			Expression *falseExpression):
		mOperator(op),
		mCondition(condition),
		mTrueExpression(trueExpression),
		mFalseExpression(falseExpression)
	{}

	virtual ~ConditionalExpression() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const { visitor.Visit(this); }

	virtual const Token *GetContextToken() const { return mOperator; }

	Expression *GetCondition() { return mCondition; }
	const Expression *GetCondition() const { return mCondition; }

	Expression *GetTrueExpression() { return mTrueExpression; }
	const Expression *GetTrueExpression() const { return mTrueExpression; }

	Expression *GetFalseExpression() { return mFalseExpression; }
	const Expression *GetFalseExpression() const { return mFalseExpression; }

private:
	const Token *mOperator;
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


class UnaryExpression: public Expression
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

	virtual const Token *GetContextToken() const { return mOperator; }

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
	FunctionCallExpression(const Token *context, Expression *lhs, Expression *argumentList):
		mContext(context),
		mLhs(lhs),
		mArgumentList(argumentList)
	{}

	virtual ~FunctionCallExpression() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const { visitor.Visit(this); }

	virtual const Token *GetContextToken() const { return mContext; }

	Expression *GetLhs() { return mLhs; }
	const Expression *GetLhs() const { return mLhs; }

	Expression *GetArgumentList() { return mArgumentList; }
	const Expression *GetArgumentList() const { return mArgumentList; }

private:
	const Token *mContext;
	Expression *mLhs;
	Expression *mArgumentList;
};


class CastExpression: public Expression
{
public:
	CastExpression(const Token *op, TypeDescriptor *typeDescriptor, Expression *rhs):
		mOperator(op),
		mTypeDescriptor(typeDescriptor),
		mRhs(rhs)
	{}

	virtual ~CastExpression() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const { visitor.Visit(this); }

	virtual const Token *GetContextToken() const { return mOperator; }

	const TypeDescriptor *GetTypeDescriptor() const { return mTypeDescriptor; }
	TypeDescriptor *GetTypeDescriptor() { return mTypeDescriptor; }

	Expression *GetRhs() { return mRhs; }
	const Expression *GetRhs() const { return mRhs; }

private:
	const Token *mOperator;
	TypeDescriptor *mTypeDescriptor;
	Expression *mRhs;
};


class SizeofExpression: public Expression
{
public:
	SizeofExpression(const Token *op, TypeDescriptor *typeDescriptor):
		mOperator(op),
		mTypeDescriptor(typeDescriptor),
		mRhs(0)
	{}

	SizeofExpression(const Token *op, Expression *rhs):
		mOperator(op),
		mTypeDescriptor(0),
		mRhs(rhs)
	{}

	virtual ~SizeofExpression() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const { visitor.Visit(this); }

	virtual const Token *GetContextToken() const { return mOperator; }

	const TypeDescriptor *GetTypeDescriptor() const { return mTypeDescriptor; }
	TypeDescriptor *GetTypeDescriptor() { return mTypeDescriptor; }

	Expression *GetRhs() { return mRhs; }
	const Expression *GetRhs() const { return mRhs; }

private:
	const Token *mOperator;
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
	explicit IdentifierExpression(QualifiedIdentifier *identifier):
		mIdentifier(identifier),
		mDefinition(0)
	{}

	virtual ~IdentifierExpression() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const { visitor.Visit(this); }

	virtual const Token *GetContextToken() const { return mIdentifier->GetContextToken(); }

	QualifiedIdentifier *GetIdentifier() { return mIdentifier; }
	const QualifiedIdentifier *GetIdentifier() const { return mIdentifier; }

	const Symbol *GetDefinition() const { return mDefinition; }
	void SetDefinition(const Symbol *symbol) { mDefinition = symbol; }

private:
	QualifiedIdentifier *mIdentifier;
	const Symbol *mDefinition;
};


class ThisExpression: public Expression
{
public:
	ThisExpression(const Token *token): mToken(token) {}
	virtual ~ThisExpression() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const { visitor.Visit(this); }

	virtual const Token *GetContextToken() const { return mToken; }

private:
	const Token *mToken;
};


class EmptyExpression: public Expression
{
public:
	virtual ~EmptyExpression() {}

	virtual void Accept(ParseNodeVisitor &visitor) { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const { visitor.Visit(this); }

	virtual const Token *GetContextToken() const { return 0; }
};


extern const TypeSpecifier BOOL_TYPE_SPECIFIER;
extern const TypeSpecifier CHAR_TYPE_SPECIFIER;
extern const TypeSpecifier INT_TYPE_SPECIFIER;
extern const TypeSpecifier UINT_TYPE_SPECIFIER;
extern const TypeSpecifier FLOAT_TYPE_SPECIFIER;

}

#endif
