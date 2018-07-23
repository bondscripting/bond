#ifndef BOND_COMPILER_PARSENODES_H
#define BOND_COMPILER_PARSENODES_H

#include "bond/compiler/parsenodesfwd.h"
#include "bond/compiler/parsenodevisitor.h"
#include "bond/compiler/token.h"
#include "bond/compiler/typeandvalue.h"

namespace Bond
{

/// \addtogroup nodes
/// @{

enum Scope
{
	SCOPE_GLOBAL,
	SCOPE_LOCAL,
	SCOPE_STRUCT_MEMBER
};


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
	ListParseNode(): mNextNode(nullptr) {}

private:
	ListParseNode *mNextNode;
};


template <typename NodeType>
class ParseNodeList
{
public:
	ParseNodeList(): mHead(nullptr), mTail(nullptr) {}

	bool IsEmpty() const { return mHead == nullptr; }

	NodeType *GetHead() const { return mHead; }

	void Append(NodeType *node)
	{
		if (node != nullptr)
		{
			if (mHead == nullptr)
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

	virtual const Token *GetContextToken() const override { return GetName(); }

	virtual SymbolType GetSymbolType() const = 0;
	virtual const Token *GetName() const { return nullptr; }

	virtual TypeAndValue *GetTypeAndValue() { return nullptr; }
	virtual const TypeAndValue *GetTypeAndValue() const { return nullptr; }

	virtual bool IsResolved() const { return true; }

	bool IsTypeDefinition() const;

	Symbol *GetParentSymbol() { return mParentSymbol; }
	const Symbol *GetParentSymbol() const { return mParentSymbol; }
	void SetParentSymbol(Symbol *parent);

	Symbol *FindSymbol(const SimpleString &name);
	const Symbol *FindSymbol(const SimpleString &name) const;

	Symbol *FindSymbol(const Token *name) { return FindSymbol(name->GetText()); }
	const Symbol *FindSymbol(const Token *name) const { return FindSymbol(name->GetText()); }

	Symbol *FindSymbol(const QualifiedIdentifier *identifier);
	const Symbol *FindSymbol(const QualifiedIdentifier *identifier) const;

	void InsertSymbol(Symbol *symbol);

	bool IsAnonymous() const { return GetName() == nullptr; }

	bool Matches(const SimpleString &name) const;

protected:
	Symbol():
		mNextSymbol(nullptr),
		mParentSymbol(nullptr),
		mSymbolList(nullptr)
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
		mTypeSpecifier(nullptr),
		mParent(nullptr),
		mLengthExpressionList(nullptr),
		mFlags(0)
	{}

	TypeDescriptor(const TypeSpecifier *specifier, bool isConst):
		mTypeSpecifier(specifier),
		mParent(nullptr),
		mLengthExpressionList(nullptr),
		mFlags(FLAG_VALUE | (isConst ? FLAG_CONST : 0))
	{}

	TypeDescriptor(TypeDescriptor *parent, bool isConst):
		mTypeSpecifier(nullptr),
		mParent(parent),
		mLengthExpressionList(nullptr),
		mFlags(FLAG_POINTER | (isConst ? FLAG_CONST : 0))
	{}

	virtual ~TypeDescriptor() {}

	virtual void Accept(ParseNodeVisitor &visitor) override { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const override { visitor.Visit(this); }

	virtual const Token *GetContextToken() const override;

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
	bool IsInstantiable() const;

	bool IsConst() const { return (mFlags & FLAG_CONST) != 0; }
	void SetConst() { mFlags |= FLAG_CONST; }
	void ClearConst() { mFlags &= ~FLAG_CONST; }

	bool IsAddressable() const { return (mFlags & FLAG_ADDRESSABLE) != 0; }
	void SetAddressable() { mFlags |= FLAG_ADDRESSABLE; }

	bool IsUnaddressableLValue() const { return (mFlags & FLAG_UNADDRESSABLE_LVALUE) != 0; }
	void SetUnaddressableLValue() { mFlags |= FLAG_UNADDRESSABLE_LVALUE; }

	bool IsLValue() const { return (mFlags & FLAG_ANY_LVALUE) != 0; }
	void ClearLValue() { mFlags &= ~FLAG_ANY_LVALUE; }

	bool IsAssignable() const { return IsLValue() && !IsConst() && !IsArrayType(); }

	uint32_t GetStackSize(PointerSize pointerSize) const;
	uint32_t GetSize(PointerSize pointerSize) const;
	uint32_t GetAlignment(PointerSize pointerSize) const;

	SignatureType GetSignatureType() const;
	Token::TokenType GetPrimitiveType() const;
	bool IsPrimitiveType() const { return GetPrimitiveType() != Token::INVALID; }
	bool IsBooleanType() const;
	bool IsCharType() const;
	bool IsIntegerType() const;
	bool IsLeast32IntegerType() const;
	bool IsMost32IntegerType() const;
	bool IsNumericType() const;
	bool IsValueType() const { return (mFlags & FLAG_VALUE) != 0; }
	bool IsArrayType() const { return (mFlags & FLAG_ARRAY) != 0; }
	bool IsNullType() const { return (mFlags & FLAG_NULL) != 0; }
	bool IsPointerIntrinsicType() const { return (mFlags & FLAG_POINTER) != 0; }
	bool IsPointerType() const { return  (mFlags & FLAG_ANY_POINTER) != 0; }
	bool IsImplicitlyConvertiblePointerType() const { return IsNullType() || IsVoidPointerType(); }
	bool IsVoidPointerType() const;
	bool IsVoidType() const;
	bool IsStructType() const;
	bool IsStringType() const;

	static TypeDescriptor GetVoidType();
	static TypeDescriptor GetBoolType();
	static TypeDescriptor GetCharType();
	static TypeDescriptor GetIntType();
	static TypeDescriptor GetUIntType();
	static TypeDescriptor GetLongType();
	static TypeDescriptor GetULongType();
	static TypeDescriptor GetFloatType();
	static TypeDescriptor GetDoubleType();
	static TypeDescriptor GetStringType();
	static TypeDescriptor GetVoidPointerType();
	static TypeDescriptor GetPointerType(const TypeSpecifier *specifier);
	static TypeDescriptor GetConstPointerType(const TypeSpecifier *specifier);
	static TypeDescriptor GetNullType();

private:
	TypeDescriptor(const TypeSpecifier *specifier, uint32_t flags):
		mTypeSpecifier(specifier),
		mParent(nullptr),
		mLengthExpressionList(nullptr),
		mFlags(flags)
	{}

	bool HasFlattenedParent() const { return ((mFlags >> PARENT_SHIFT) & STORAGE_MASK) != 0; }

	static const uint32_t FLAG_VALUE = 1 << 0;
	static const uint32_t FLAG_POINTER = 1 << 1;
	static const uint32_t FLAG_ARRAY = 1 << 2;
	static const uint32_t FLAG_NULL = 1 << 3;
	static const uint32_t FLAG_CONST = 1 << 4;
	static const uint32_t FLAG_ADDRESSABLE = 1 << 5;
	static const uint32_t FLAG_UNADDRESSABLE_LVALUE = 1 << 6;
	static const uint32_t FLAG_ANY_POINTER = FLAG_POINTER | FLAG_ARRAY | FLAG_NULL;
	static const uint32_t FLAG_ANY_LVALUE = FLAG_ADDRESSABLE | FLAG_UNADDRESSABLE_LVALUE;
	static const uint32_t PARENT_SHIFT = 8;
	static const uint32_t STORAGE_MASK = FLAG_VALUE | FLAG_ANY_POINTER;
	static const uint32_t FLAG_MASK = (1 << PARENT_SHIFT) - 1;

	const TypeSpecifier *mTypeSpecifier;
	TypeDescriptor *mParent;
	Expression *mLengthExpressionList;
	uint32_t mFlags;
};


class TypeSpecifier: public ParseNode
{
public:

	explicit TypeSpecifier(const Token *primitiveType, QualifiedIdentifier *identifier = nullptr):
		mPrimitiveType(primitiveType),
		mIdentifier(identifier),
		mDefinition(nullptr)
	{}

	explicit TypeSpecifier(QualifiedIdentifier *identifier):
		mPrimitiveType(nullptr),
		mIdentifier(identifier),
		mDefinition(nullptr)
	{}

	TypeSpecifier(const Token *primitiveType, QualifiedIdentifier *identifier, const Symbol *definition):
		mPrimitiveType(primitiveType),
		mIdentifier(identifier),
		mDefinition(definition)
	{}

	virtual ~TypeSpecifier() {}

	virtual void Accept(ParseNodeVisitor &visitor) override { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const override { visitor.Visit(this); }

	virtual const Token *GetContextToken() const override;

	const Token *GetPrimitiveTypeToken() const { return mPrimitiveType; }

	QualifiedIdentifier *GetIdentifier() { return mIdentifier; }
	const QualifiedIdentifier *GetIdentifier() const { return mIdentifier; }

	const Symbol *GetDefinition() const { return mDefinition; }
	void SetDefinition(const Symbol *symbol) { mDefinition = symbol; }

	bool IsResolved() const;
	bool IsInstantiable() const;

	uint32_t GetSize(PointerSize pointerSize) const;
	uint32_t GetAlignment() const;

	SignatureType GetSignatureType() const;
	Token::TokenType GetPrimitiveType() const;
	bool IsBooleanType() const;
	bool IsCharType() const;
	bool IsIntegerType() const;
	bool IsLeast32IntegerType() const;
	bool IsMost32IntegerType() const;
	bool IsNumericType() const;
	bool IsVoidType() const;
	bool IsStructType() const;

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

	virtual void Accept(ParseNodeVisitor &visitor) override { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const override { visitor.Visit(this); }

	virtual const Token *GetContextToken() const override { return mName; }

	const Token *GetName() const { return mName; }

	QualifiedIdentifier *GetNextIdentifier() { return static_cast<QualifiedIdentifier *>(GetNextNode()); }
	const QualifiedIdentifier *GetNextIdentifier() const { return static_cast<const QualifiedIdentifier *>(GetNextNode()); }

	bool IsTerminal() const { return GetNextNode() == nullptr; }

private:
	const Token *mName;
};


class TranslationUnit: public ListParseNode
{
public:
	TranslationUnit(IncludeDirective *includeDirectiveList, ListParseNode *declarationList):
		mIncludeDirectiveList(includeDirectiveList),
		mDeclarationList(declarationList),
		mRequiresCodeGeneration(true)
	{}

	virtual ~TranslationUnit() {}

	virtual void Accept(ParseNodeVisitor &visitor) override { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const override { visitor.Visit(this); }

	virtual const Token *GetContextToken() const override { return nullptr; }

	IncludeDirective *GetIncludeDirectiveList() { return mIncludeDirectiveList; }
	const IncludeDirective *GetIncludeDirectiveList() const { return mIncludeDirectiveList; }


	ListParseNode *GetExternalDeclarationList() { return mDeclarationList; }
	const ListParseNode *GetExternalDeclarationList() const { return mDeclarationList; }
	bool RequiresCodeGeneration() const { return mRequiresCodeGeneration; }
	void SetRequiresCodeGeneration(bool required) { mRequiresCodeGeneration = required; }

private:
	IncludeDirective *mIncludeDirectiveList;
	ListParseNode *mDeclarationList;
	bool mRequiresCodeGeneration;
};


class IncludeDirective: public ListParseNode
{
public:
	explicit IncludeDirective(const Token *includePath): mIncludePath(includePath) {}
	virtual ~IncludeDirective() {}

	virtual void Accept(ParseNodeVisitor &visitor) override { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const override { visitor.Visit(this); }

	virtual const Token *GetContextToken() const override { return mIncludePath; }

	const Token *GetIncludePath() const { return mIncludePath; }

private:
	const Token *mIncludePath;
};


class NamespaceDefinition: public Symbol
{
public:
	NamespaceDefinition(const Token *name, ListParseNode *declarationList):
		mName(name),
		mDeclarationList(declarationList)
	{}

	virtual ~NamespaceDefinition() {}

	virtual void Accept(ParseNodeVisitor &visitor) override { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const override { visitor.Visit(this); }

	virtual SymbolType GetSymbolType() const override { return TYPE_NAMESPACE; }
	virtual const Token *GetName() const override { return mName; }

	ListParseNode *GetExternalDeclarationList() { return mDeclarationList; }
	const ListParseNode *GetExternalDeclarationList() const { return mDeclarationList; }

private:
	const Token *mName;
	ListParseNode *mDeclarationList;
};


class NativeBlock: public ListParseNode
{
public:
	NativeBlock(const Token *keyword, ListParseNode *declarationList):
		mKeyword(keyword),
		mDeclarationList(declarationList)
	{}

	virtual ~NativeBlock() {}

	virtual void Accept(ParseNodeVisitor &visitor) override { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const override { visitor.Visit(this); }

	virtual const Token *GetContextToken() const override { return mKeyword; }

	const Token *GetKeyword() const { return mKeyword; }

	ListParseNode *GetNativeDeclarationList() { return mDeclarationList; }
	const ListParseNode *GetNativeDeclarationList() const { return mDeclarationList; }

private:
	const Token *mKeyword;
	ListParseNode *mDeclarationList;
};


class EnumDeclaration: public Symbol
{
public:
	explicit EnumDeclaration(const Token *name):
		mIdentifier(name),
		mTypeSpecifier(&INT_TOKEN, &mIdentifier, this),
		mTypeDescriptor(&mTypeSpecifier, true),
		mEnumeratorList(nullptr)
	{}

	virtual ~EnumDeclaration() {}

	virtual void Accept(ParseNodeVisitor &visitor) override { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const override { visitor.Visit(this); }

	virtual SymbolType GetSymbolType() const override { return TYPE_ENUM; }
	virtual const Token *GetName() const override { return mIdentifier.GetName(); }

	TypeDescriptor &GetTypeDescriptor() { return mTypeDescriptor; }
	const TypeDescriptor &GetTypeDescriptor() const { return mTypeDescriptor; }

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
	Enumerator(const Token *name, TypeDescriptor &typeDescriptor, Expression *value):
		mTypeAndValue(typeDescriptor),
		mName(name),
		mValue(value) {}
	virtual ~Enumerator() {}

	virtual void Accept(ParseNodeVisitor &visitor) override { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const override { visitor.Visit(this); }

	virtual SymbolType GetSymbolType() const override { return TYPE_VALUE; }
	virtual const Token *GetName() const override { return mName; }

	virtual TypeAndValue *GetTypeAndValue() override { return &mTypeAndValue; }
	virtual const TypeAndValue *GetTypeAndValue() const override { return &mTypeAndValue; }

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
		VARIANT_NATIVE_STUB
	};

	StructDeclaration(
			const Token *name,
			const Token *size,
			const Token *alignment,
			FunctionDefinition *memberFunctionList,
			DeclarativeStatement *memberVariableList,
			Variant variant):
		mIdentifier(name),
		mTypeSpecifier(nullptr, &mIdentifier, this),
		mThisTypeDescriptor(TypeDescriptor::GetPointerType(&mTypeSpecifier)),
		mConstThisTypeDescriptor(TypeDescriptor::GetConstPointerType(&mTypeSpecifier)),
		mSizeToken(size),
		mAlignmentToken(alignment),
		mMemberFunctionList(memberFunctionList),
		mMemberVariableList(memberVariableList),
		mSizeSpecifier((size != nullptr) ? this : nullptr),
		mVariant(variant),
		mSize(0),
		mAlignment(BOND_DEFAULT_STRUCT_ALIGN)
	{}

	virtual ~StructDeclaration() {}

	virtual void Accept(ParseNodeVisitor &visitor) override { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const override { visitor.Visit(this); }

	virtual SymbolType GetSymbolType() const override { return TYPE_STRUCT; }
	virtual const Token *GetName() const override { return mIdentifier.GetName(); }

	virtual bool IsResolved() const override;

	TypeDescriptor &GetThisTypeDescriptor() { return mThisTypeDescriptor; }
	const TypeDescriptor &GetThisTypeDescriptor() const { return mThisTypeDescriptor; }

	TypeDescriptor &GetConstThisTypeDescriptor() { return mConstThisTypeDescriptor; }
	const TypeDescriptor &GetConstThisTypeDescriptor() const { return mConstThisTypeDescriptor; }

	const Token *GetSizeToken() const { return mSizeToken; }
	const Token *GetAlignmentToken() const { return mAlignmentToken; }

	FunctionDefinition *GetMemberFunctionList() { return mMemberFunctionList; }
	const FunctionDefinition *GetMemberFunctionList() const { return mMemberFunctionList; }
	void SetMemberFunctionList(FunctionDefinition *memberFunctionList) { mMemberFunctionList = memberFunctionList; }

	DeclarativeStatement *GetMemberVariableList() { return mMemberVariableList; }
	const DeclarativeStatement *GetMemberVariableList() const { return mMemberVariableList; }
	void SetMemberVariableList(DeclarativeStatement *memberVariableList) { mMemberVariableList = memberVariableList; }

	const StructDeclaration *GetSizeSpecifier() const { return mSizeSpecifier; }
	void SetSizeSpecifier(const StructDeclaration *sizeSpecifier) { mSizeSpecifier = sizeSpecifier; }

	Variant GetVariant() const { return mVariant; }
	bool IsNative() const { return mVariant != VARIANT_BOND; }

	uint32_t GetSize() const { return mSize; }
	void SetSize(uint32_t size) { mSize = size; }

	uint32_t GetAlignment() const { return mAlignment; }
	void SetAlignment(uint32_t alignment) { mAlignment = alignment; }

	bool IsInstantiable() const { return (mVariant == VARIANT_BOND) || (mSizeSpecifier != nullptr); }

private:
	QualifiedIdentifier mIdentifier;
	TypeSpecifier mTypeSpecifier;
	TypeDescriptor mThisTypeDescriptor;
	TypeDescriptor mConstThisTypeDescriptor;
	const Token *mSizeToken;
	const Token *mAlignmentToken;
	FunctionDefinition *mMemberFunctionList;
	DeclarativeStatement *mMemberVariableList;
	const StructDeclaration *mSizeSpecifier;
	Variant mVariant;
	uint32_t mSize;
	uint32_t mAlignment;
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

	virtual void Accept(ParseNodeVisitor &visitor) override { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const override { visitor.Visit(this); }

	virtual const Token *GetContextToken() const override { return mName; }

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
	FunctionDefinition(
			FunctionPrototype *prototype,
			CompoundStatement *body,
			TypeDescriptor *thisTypeDescriptor,
			Scope scope):
		mIdentifier(prototype->GetName()),
		mTypeSpecifier(nullptr, &mIdentifier, this),
		mTypeDescriptor(&mTypeSpecifier, false),
		mTypeAndValue(mTypeDescriptor),
		mPrototype(prototype),
		mBody(body),
		mThisTypeDescriptor(thisTypeDescriptor),
		mScope(scope),
		mNumReservedJumpTargetIds(0)
	{}

	virtual ~FunctionDefinition() {}

	virtual void Accept(ParseNodeVisitor &visitor) override { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const override { visitor.Visit(this); }

	virtual SymbolType GetSymbolType() const override { return TYPE_FUNCTION; }
	virtual const Token *GetName() const override { return mIdentifier.GetName(); }

	virtual TypeAndValue *GetTypeAndValue() override { return &mTypeAndValue; }
	virtual const TypeAndValue *GetTypeAndValue() const override { return &mTypeAndValue; }

	FunctionPrototype *GetPrototype() { return mPrototype; }
	const FunctionPrototype *GetPrototype() const { return mPrototype; }

	CompoundStatement *GetBody() { return mBody; }
	const CompoundStatement *GetBody() const { return mBody; }

	TypeDescriptor *GetThisTypeDescriptor() { return mThisTypeDescriptor; }
	const TypeDescriptor *GetThisTypeDescriptor() const { return mThisTypeDescriptor; }

	Scope GetScope() const { return mScope; }

	bool IsNative() const { return mBody == nullptr; }

	size_t GetNumReservedJumpTargetIds() const { return mNumReservedJumpTargetIds; }
	void SetNumReservedJumpTargetIds(size_t numIds) { mNumReservedJumpTargetIds = numIds; }

private:
	QualifiedIdentifier mIdentifier;
	TypeSpecifier mTypeSpecifier;
	TypeDescriptor mTypeDescriptor;
	TypeAndValue mTypeAndValue;
	FunctionPrototype *mPrototype;
	CompoundStatement *mBody;
	TypeDescriptor *mThisTypeDescriptor;
	Scope mScope;
	size_t mNumReservedJumpTargetIds;
};


class Parameter: public Symbol
{
public:
	Parameter(const Token *name, TypeDescriptor *typeDescriptor):
		mTypeAndValue(*typeDescriptor),
		mName(name),
		mTypeDescriptor(typeDescriptor),
		mOffset(0)
	{}

	// Used by the code generator to create a temporary Parameter object when writing
	// the signature for native member accessors.
	Parameter(const TypeDescriptor *typeDescriptor, int32_t offset):
		mTypeAndValue(*const_cast<TypeDescriptor *>(typeDescriptor)),
		mName(nullptr),
		mTypeDescriptor(const_cast<TypeDescriptor *>(typeDescriptor)),
		mOffset(offset)
	{}

	virtual ~Parameter() {}

	virtual void Accept(ParseNodeVisitor &visitor) override { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const override { visitor.Visit(this); }

	virtual SymbolType GetSymbolType() const override { return TYPE_VALUE; }
	virtual const Token *GetName() const override { return mName; }

	virtual TypeAndValue *GetTypeAndValue() override { return &mTypeAndValue; }
	virtual const TypeAndValue *GetTypeAndValue() const override { return &mTypeAndValue; }

	TypeDescriptor *GetTypeDescriptor() { return mTypeDescriptor; }
	const TypeDescriptor *GetTypeDescriptor() const { return mTypeDescriptor; }

	int32_t GetOffset() const { return mOffset; }
	void SetOffset(int32_t offset) const { mOffset = offset; }

private:
	TypeAndValue mTypeAndValue;
	const Token *mName;
	TypeDescriptor *mTypeDescriptor;
	// Ew. Became mutable after population of this field was moved to the code generator.
	mutable int32_t mOffset;
};


class NamedInitializer: public Symbol
{
public:
	NamedInitializer(const Token *name, Initializer *initializer, TypeDescriptor *typeDescriptor, Scope scope, bool isNative):
		mTypeAndValue(*typeDescriptor),
		mName(name),
		mInitializer(initializer),
		mScope(scope),
		mOffset(isNative ? -1 : 0),
		mIsElidable(false)
	{}

	virtual ~NamedInitializer() {}

	virtual void Accept(ParseNodeVisitor &visitor) override { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const override { visitor.Visit(this); }

	virtual SymbolType GetSymbolType() const override { return TYPE_VALUE; }
	virtual const Token *GetName() const override { return mName; }

	virtual TypeAndValue *GetTypeAndValue() override { return &mTypeAndValue; }
	virtual const TypeAndValue *GetTypeAndValue() const override { return &mTypeAndValue; }

	Initializer *GetInitializer() { return mInitializer; }
	const Initializer *GetInitializer() const { return mInitializer; }

	Scope GetScope() const { return mScope; }

	int32_t GetOffset() const { return mOffset; }
	void SetOffset(int32_t offset) const { mOffset = offset; }

	bool IsNativeStructMember() const { return (mScope == SCOPE_STRUCT_MEMBER) && (mOffset < 0); }
	bool IsElidable() const { return mIsElidable; }
	void SetElidable(bool isElidable) { mIsElidable = isElidable; }

private:
	TypeAndValue mTypeAndValue;
	const Token *mName;
	Initializer *mInitializer;
	Scope mScope;
	// Ew. Became mutable after population of this field was moved to the code generator.
	mutable int32_t mOffset;
	bool mIsElidable;
};


class Initializer: public ListParseNode
{
public:
	explicit Initializer(Expression *expression):
		mOpenBrace(nullptr),
		mExpression(expression),
		mInitializerList(nullptr)
	{}

	explicit Initializer(const Token *openBrace, Initializer *initializerList):
		mOpenBrace(openBrace),
		mExpression(nullptr),
		mInitializerList(initializerList)
	{}

	virtual ~Initializer() {}

	virtual void Accept(ParseNodeVisitor &visitor) override { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const override { visitor.Visit(this); }

	virtual const Token *GetContextToken() const override;

	const TypeDescriptor &GetTypeDescriptor() const { return mTypeDescriptor; }
	void SetTypeDescriptor(const TypeDescriptor &descriptor) { mTypeDescriptor = descriptor; }

	Expression *GetExpression() { return mExpression; }
	const Expression *GetExpression() const { return mExpression; }

	Initializer *GetInitializerList() { return mInitializerList; }
	const Initializer *GetInitializerList() const { return mInitializerList; }

	virtual bool IsResolved() const;

private:
	TypeDescriptor mTypeDescriptor;
	const Token *mOpenBrace;
	Expression *mExpression;
	Initializer *mInitializerList;
};


class CompoundStatement: public Symbol
{
public:
	explicit CompoundStatement(ListParseNode *statementList): mStatementList(statementList) {}
	virtual ~CompoundStatement() {}

	virtual void Accept(ParseNodeVisitor &visitor) override { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const override { visitor.Visit(this); }

	virtual SymbolType GetSymbolType() const override { return TYPE_LOCALSCOPE; }

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

	virtual void Accept(ParseNodeVisitor &visitor) override { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const override { visitor.Visit(this); }

	virtual const Token *GetContextToken() const override { return mKeyword; }

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
		mSectionList(sectionList),
		mResolvedLabelList(nullptr),
		mNumMatches(0),
		mMinMatch(0),
		mMaxMatch(0)
	{}

	virtual ~SwitchStatement() {}

	virtual void Accept(ParseNodeVisitor &visitor) override { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const override { visitor.Visit(this); }

	virtual const Token *GetContextToken() const override { return mKeyword; }

	const Token *GetKeyword() const { return mKeyword; }

	Expression *GetControl() { return mControl; }
	const Expression *GetControl() const { return mControl; }

	SwitchSection *GetSectionList() { return mSectionList; }
	const SwitchSection *GetSectionList() const { return mSectionList; }

	const ResolvedSwitchLabel *GetResolvedLabelList() const { return mResolvedLabelList; }
	void SetResolvedLabelList(const ResolvedSwitchLabel *labelList) { mResolvedLabelList = labelList; }

	uint32_t GetNumMatches() const { return mNumMatches; }
	void SetNumMatches(uint32_t numMatches) { mNumMatches = numMatches; }

	int32_t GetMinMatch() const { return mMinMatch; }
	void SetMinMatch(int32_t minMatch) { mMinMatch = minMatch; }

	int32_t GetMaxMatch() const { return mMaxMatch; }
	void SetMaxMatch(int32_t maxMatch) { mMaxMatch = maxMatch; }

private:
	const Token *mKeyword;
	Expression *mControl;
	SwitchSection *mSectionList;
	const ResolvedSwitchLabel *mResolvedLabelList;
	uint32_t mNumMatches;
	int32_t mMinMatch;
	int32_t mMaxMatch;
};


class SwitchSection: public Symbol
{
public:
	SwitchSection(SwitchLabel *labelList, ListParseNode* statementList):
		mLabelList(labelList),
		mStatementList(statementList),
		mJumpTargetId(0)
	{}

	virtual ~SwitchSection() {}

	virtual void Accept(ParseNodeVisitor &visitor) override { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const override { visitor.Visit(this); }

	virtual SymbolType GetSymbolType() const override { return TYPE_LOCALSCOPE; }

	SwitchLabel *GetLabelList() { return mLabelList; }
	const SwitchLabel *GetLabelList() const { return mLabelList; }

	ListParseNode *GetStatementList() { return mStatementList; }
	const ListParseNode *GetStatementList() const { return mStatementList; }

	size_t GetJumpTargetId() const { return mJumpTargetId; }
	void SetJumpTargetId(size_t id) { mJumpTargetId = id; }

private:
	SwitchLabel *mLabelList;
	ListParseNode *mStatementList;
	size_t mJumpTargetId;
};


class ResolvedSwitchLabel: public ListParseNode
{
public:
	ResolvedSwitchLabel(): mMatch(0), mJumpTargetId(0), mIsDefault(false) {}

	virtual void Accept(ParseNodeVisitor &visitor) override {}
	virtual void Accept(ParseNodeVisitor &visitor) const override {}
	virtual const Token *GetContextToken() const override { return nullptr; }

	int32_t GetMatch() const { return mMatch; }
	void SetMatch(int32_t match) { mMatch = match; }

	size_t GetJumpTargetId() const { return mJumpTargetId; }
	void SetJumpTargetId(size_t targetId) { mJumpTargetId = targetId; }

	bool IsDefault() const { return mIsDefault; }
	void SetIsDefault(bool isDefault) { mIsDefault = isDefault; }

	bool operator==(const ResolvedSwitchLabel &other) const { return (mIsDefault == other.mIsDefault) && (mMatch == other.mMatch); }

private:
	int32_t mMatch;
	size_t mJumpTargetId;
	bool mIsDefault;
};


class SwitchLabel: public ListParseNode
{
public:
	explicit SwitchLabel(const Token *label):
		mLabel(label),
		mExpression(nullptr)
	{}

	SwitchLabel(const Token *label, Expression *expression):
		mLabel(label),
		mExpression(expression)
	{}

	virtual ~SwitchLabel() {}

	virtual void Accept(ParseNodeVisitor &visitor) override { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const override { visitor.Visit(this); }

	virtual const Token *GetContextToken() const override { return mLabel; }

	const Token *GetLabel() const { return mLabel; }

	Expression *GetExpression() { return mExpression; }
	const Expression *GetExpression() const { return mExpression; }

	ResolvedSwitchLabel &GetResolvedLabel() { return mResolvedLabel; }
	const ResolvedSwitchLabel &GetResolvedLabel() const { return mResolvedLabel; }

	bool IsDefaultLabel() const { return mLabel->GetTokenType() == Token::KEY_DEFAULT; }

private:
	ResolvedSwitchLabel mResolvedLabel;
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

	virtual void Accept(ParseNodeVisitor &visitor) override { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const override { visitor.Visit(this); }

	virtual const Token *GetContextToken() const override { return mKeyword; }

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

	virtual void Accept(ParseNodeVisitor &visitor) override { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const override { visitor.Visit(this); }

	virtual SymbolType GetSymbolType() const override { return TYPE_LOCALSCOPE; }

	virtual const Token *GetContextToken() const override { return mKeyword; }

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

	virtual void Accept(ParseNodeVisitor &visitor) override { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const override { visitor.Visit(this); }

	virtual const Token *GetContextToken() const override { return mKeyword; }

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

	virtual void Accept(ParseNodeVisitor &visitor) override { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const override { visitor.Visit(this); }

	virtual const Token *GetContextToken() const override { return mTypeDescriptor->GetContextToken(); }

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

	const TypeDescriptor &GetTypeDescriptor() const { return mTypeDescriptor; }
	void SetTypeDescriptor(const TypeDescriptor &descriptor) { mTypeDescriptor = descriptor; }

	const TypeAndValue &GetTypeAndValue() const { return mTypeAndValue; }
	TypeAndValue &GetTypeAndValue() { return mTypeAndValue; }

protected:
	Expression(): mTypeAndValue(mTypeDescriptor) {}

private:
	TypeDescriptor mTypeDescriptor;
	TypeAndValue mTypeAndValue;
};


class ExpressionStatement: public ListParseNode
{
public:
	explicit ExpressionStatement(Expression *expression): mExpression(expression) {}
	virtual ~ExpressionStatement() {}

	virtual void Accept(ParseNodeVisitor &visitor) override { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const override { visitor.Visit(this); }

	virtual const Token *GetContextToken() const override { return (mExpression == nullptr) ? nullptr : mExpression->GetContextToken(); }

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

	virtual void Accept(ParseNodeVisitor &visitor) override { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const override { visitor.Visit(this); }

	virtual const Token *GetContextToken() const override { return mOperator; }

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

	virtual void Accept(ParseNodeVisitor &visitor) override { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const override { visitor.Visit(this); }

	virtual const Token *GetContextToken() const override { return mOperator; }

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

	virtual void Accept(ParseNodeVisitor &visitor) override { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const override { visitor.Visit(this); }

	virtual const Token *GetContextToken() const override { return mOperator; }

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

	virtual void Accept(ParseNodeVisitor &visitor) override { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const override { visitor.Visit(this); }

	virtual const Token *GetContextToken() const override { return mOperator; }

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
		mLhs(lhs),
		mDefinition(nullptr)
	{}

	virtual ~MemberExpression() {}

	virtual void Accept(ParseNodeVisitor &visitor) override { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const override { visitor.Visit(this); }

	virtual const Token *GetContextToken() const override { return mOperator; }

	const Token *GetOperator() const { return mOperator; }
	const Token *GetMemberName() const { return mMemberName; }

	Expression *GetLhs() { return mLhs; }
	const Expression *GetLhs() const { return mLhs; }

	const Symbol *GetDefinition() const { return mDefinition; }
	void SetDefinition(const Symbol *symbol) { mDefinition = symbol; }

private:
	const Token *mOperator;
	const Token *mMemberName;
	Expression *mLhs;
	const Symbol *mDefinition;
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

	virtual void Accept(ParseNodeVisitor &visitor) override { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const override { visitor.Visit(this); }

	virtual const Token *GetContextToken() const override { return mOperator; }

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

	virtual void Accept(ParseNodeVisitor &visitor) override { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const override { visitor.Visit(this); }

	virtual const Token *GetContextToken() const override { return mContext; }

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
	CastExpression(const Token *op, TypeDescriptor *targetTypeDescriptor, Expression *rhs):
		mOperator(op),
		mTargetTypeDescriptor(targetTypeDescriptor),
		mRhs(rhs)
	{}

	virtual ~CastExpression() {}

	virtual void Accept(ParseNodeVisitor &visitor) override { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const override { visitor.Visit(this); }

	virtual const Token *GetContextToken() const override { return mOperator; }

	const TypeDescriptor *GetTargetTypeDescriptor() const { return mTargetTypeDescriptor; }
	TypeDescriptor *GetTargetTypeDescriptor() { return mTargetTypeDescriptor; }

	Expression *GetRhs() { return mRhs; }
	const Expression *GetRhs() const { return mRhs; }

private:
	const Token *mOperator;
	TypeDescriptor *mTargetTypeDescriptor;
	Expression *mRhs;
};


class PropertyofExpression: public Expression
{
public:
	PropertyofExpression(const Token *op, TypeDescriptor *targetTypeDescriptor):
		mOperator(op),
		mTargetTypeDescriptor(targetTypeDescriptor),
		mRhs(nullptr)
	{}

	PropertyofExpression(const Token *op, Expression *rhs):
		mOperator(op),
		mTargetTypeDescriptor(nullptr),
		mRhs(rhs)
	{}

	virtual ~PropertyofExpression() {}

	virtual void Accept(ParseNodeVisitor &visitor) override { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const override { visitor.Visit(this); }

	virtual const Token *GetContextToken() const override { return mOperator; }

	const Token *GetOperator() const { return mOperator; }

	const TypeDescriptor *GetTargetTypeDescriptor() const { return mTargetTypeDescriptor; }
	TypeDescriptor *GetTargetTypeDescriptor() { return mTargetTypeDescriptor; }

	Expression *GetRhs() { return mRhs; }
	const Expression *GetRhs() const { return mRhs; }

	bool IsAlignof() const { return mOperator->GetTokenType() == Token::KEY_ALIGNOF; }
	bool IsSizeof() const { return mOperator->GetTokenType() == Token::KEY_SIZEOF; }

private:
	const Token *mOperator;
	TypeDescriptor *mTargetTypeDescriptor;
	Expression *mRhs;
};


class ConstantLiteralExpression: public Expression
{
public:
	explicit ConstantLiteralExpression(const Token *value): mValue(value) {}
	virtual ~ConstantLiteralExpression() {}

	virtual void Accept(ParseNodeVisitor &visitor) override { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const override { visitor.Visit(this); }

	virtual const Token *GetContextToken() const override { return mValue; }

	const Token *GetValueToken() const { return mValue; }

private:
	const Token *mValue;
};


class IdentifierExpression: public Expression
{
public:
	explicit IdentifierExpression(QualifiedIdentifier *identifier):
		mIdentifier(identifier),
		mDefinition(nullptr)
	{}

	virtual ~IdentifierExpression() {}

	virtual void Accept(ParseNodeVisitor &visitor) override { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const override { visitor.Visit(this); }

	virtual const Token *GetContextToken() const override { return mIdentifier->GetContextToken(); }

	QualifiedIdentifier *GetIdentifier() { return mIdentifier; }
	const QualifiedIdentifier *GetIdentifier() const { return mIdentifier; }

	Symbol *GetDefinition() { return mDefinition; }
	const Symbol *GetDefinition() const { return mDefinition; }
	void SetDefinition(Symbol *symbol) { mDefinition = symbol; }

private:
	QualifiedIdentifier *mIdentifier;
	Symbol *mDefinition;
};


class ThisExpression: public Expression
{
public:
	explicit ThisExpression(const Token *token): mToken(token) {}
	virtual ~ThisExpression() {}

	virtual void Accept(ParseNodeVisitor &visitor) override { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const override { visitor.Visit(this); }

	virtual const Token *GetContextToken() const override { return mToken; }

private:
	const Token *mToken;
};


class EmptyExpression: public Expression
{
public:
	virtual ~EmptyExpression() {}

	virtual void Accept(ParseNodeVisitor &visitor) override { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const override { visitor.Visit(this); }

	virtual const Token *GetContextToken() const override { return nullptr; }
};


extern const TypeSpecifier VOID_TYPE_SPECIFIER;
extern const TypeSpecifier BOOL_TYPE_SPECIFIER;
extern const TypeSpecifier CHAR_TYPE_SPECIFIER;
extern const TypeSpecifier INT_TYPE_SPECIFIER;
extern const TypeSpecifier UINT_TYPE_SPECIFIER;
extern const TypeSpecifier LONG_TYPE_SPECIFIER;
extern const TypeSpecifier ULONG_TYPE_SPECIFIER;
extern const TypeSpecifier FLOAT_TYPE_SPECIFIER;
extern const TypeSpecifier DOUBLE_TYPE_SPECIFIER;

/// @}

}

#endif
