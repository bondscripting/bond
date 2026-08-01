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

/// \brief Describes where a symbol is declared.
enum Scope
{
	/// \brief Symbol is declared at global scope.
	SCOPE_GLOBAL,

	/// \brief Symbol is declared in a local scope.
	SCOPE_LOCAL,

	/// \brief Symbol is declared as a struct member.
	SCOPE_STRUCT_MEMBER
};


/// \brief Base class for all parse tree nodes.
class ParseNode
{
public:
	virtual ~ParseNode() {}

	/// \brief Dispatches this node to the given visitor.
	/// \param visitor Visitor that handles this node type.
	virtual void Accept(ParseNodeVisitor &visitor) = 0;

	/// \brief Dispatches this node to the given visitor.
	/// \param visitor Visitor that handles this node type.
	virtual void Accept(ParseNodeVisitor &visitor) const = 0;

	/// \brief Returns the token used as source context for diagnostics.
	/// \returns A token associated with this node, or null if no token applies.
	virtual const Token *GetContextToken() const = 0;

protected:
	ParseNode() {}
};


/// \brief Base class for parse nodes that can be linked into singly linked lists.
class ListParseNode: public ParseNode
{
public:
	virtual ~ListParseNode() {}

	/// \brief Returns the next node in the list.
	ListParseNode *GetNextNode() { return mNextNode; }

	/// \brief Returns the next node in the list.
	const ListParseNode *GetNextNode() const { return mNextNode; }

	/// \brief Sets the next node in the list.
	/// \param next Node to link after this node.
	void SetNextNode(ListParseNode *next) { mNextNode = next; }

protected:
	ListParseNode(): mNextNode(nullptr) {}

private:
	ListParseNode *mNextNode;
};


template <typename NodeType>
/// \brief Helper container for singly linked lists of parse nodes.
class ParseNodeList
{
public:
	/// \brief Constructs an empty parse node list.
	ParseNodeList(): mHead(nullptr), mTail(nullptr) {}

	/// \brief Returns true if the list has no nodes.
	bool IsEmpty() const { return mHead == nullptr; }

	/// \brief Returns the first node in the list.
	/// \returns The head node, or null when the list is empty.
	NodeType *GetHead() const { return mHead; }

	/// \brief Appends a node to the end of the list.
	/// \param node Node to append. Null nodes are ignored.
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


/// \brief Base class for named entities that participate in symbol lookup.
class Symbol: public ListParseNode
{
public:
	/// \brief Identifies the concrete symbol category.
	enum SymbolType
	{
		/// \brief Namespace declaration symbol.
		TYPE_NAMESPACE,

		/// \brief Struct declaration symbol.
		TYPE_STRUCT,

		/// \brief Enum declaration symbol.
		TYPE_ENUM,

		/// \brief Local scope symbol.
		TYPE_LOCALSCOPE,

		/// \brief Value symbol, such as a variable or enumerator.
		TYPE_VALUE,

		/// \brief Function symbol.
		TYPE_FUNCTION,
	};

	virtual ~Symbol() {}

	virtual const Token *GetContextToken() const override { return GetName(); }

	/// \brief Returns the concrete symbol type.
	virtual SymbolType GetSymbolType() const = 0;

	/// \brief Returns the symbol name token.
	/// \returns The name token, or null for anonymous symbols.
	virtual const Token *GetName() const { return nullptr; }

	/// \brief Returns the symbol's type-and-value metadata.
	/// \returns Type and value information, or null if not applicable.
	virtual TypeAndValue *GetTypeAndValue() { return nullptr; }

	/// \brief Returns the symbol's type-and-value metadata.
	/// \returns Type and value information, or null if not applicable.
	virtual const TypeAndValue *GetTypeAndValue() const { return nullptr; }

	/// \brief Returns true when symbol resolution has completed for this symbol.
	virtual bool IsResolved() const { return true; }

	/// \brief Returns true if this symbol defines a type.
	bool IsTypeDefinition() const;

	/// \brief Returns the enclosing symbol.
	/// \returns The parent symbol, or null at global scope.
	Symbol *GetParentSymbol() { return mParentSymbol; }

	/// \brief Returns the enclosing symbol.
	/// \returns The parent symbol, or null at global scope.
	const Symbol *GetParentSymbol() const { return mParentSymbol; }

	/// \brief Sets the enclosing symbol.
	/// \param parent Parent symbol to assign.
	void SetParentSymbol(Symbol *parent);

	/// \brief Finds a directly contained symbol by name.
	/// \param name Name to search for.
	/// \returns Matching symbol, or null if not found.
	Symbol *FindSymbol(const StringView &name);

	/// \brief Finds a directly contained symbol by name.
	/// \param name Name to search for.
	/// \returns Matching symbol, or null if not found.
	const Symbol *FindSymbol(const StringView &name) const;

	/// \brief Finds a directly contained symbol by token text.
	/// \param name Name token to search for.
	/// \returns Matching symbol, or null if not found.
	Symbol *FindSymbol(const Token *name) { return FindSymbol(name->GetText()); }

	/// \brief Finds a directly contained symbol by token text.
	/// \param name Name token to search for.
	/// \returns Matching symbol, or null if not found.
	const Symbol *FindSymbol(const Token *name) const { return FindSymbol(name->GetText()); }

	/// \brief Finds a symbol referenced by a qualified identifier.
	/// \param identifier Qualified identifier to resolve.
	/// \returns Matching symbol, or null if not found.
	Symbol *FindSymbol(const QualifiedIdentifier *identifier);

	/// \brief Finds a symbol referenced by a qualified identifier.
	/// \param identifier Qualified identifier to resolve.
	/// \returns Matching symbol, or null if not found.
	const Symbol *FindSymbol(const QualifiedIdentifier *identifier) const;

	/// \brief Inserts a symbol into this symbol's child list.
	/// \param symbol Symbol to insert.
	void InsertSymbol(Symbol *symbol);

	/// \brief Returns true if this symbol has no explicit name.
	bool IsAnonymous() const { return GetName() == nullptr; }

	/// \brief Returns true if this symbol's name matches the given text.
	/// \param name Name to compare against.
	bool Matches(const StringView &name) const;

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


/// \brief Describes a resolved or partially resolved Bond type.
class TypeDescriptor: public ParseNode
{
public:
	/// \brief Constructs an undefined type descriptor.
	TypeDescriptor():
		mTypeSpecifier(nullptr),
		mParent(nullptr),
		mLengthExpressionList(nullptr),
		mFlags(0)
	{}

	/// \brief Constructs a value type descriptor from a type specifier.
	/// \param specifier Type specifier for the value type.
	/// \param isConst True to mark the descriptor as const-qualified.
	TypeDescriptor(const TypeSpecifier *specifier, bool isConst):
		mTypeSpecifier(specifier),
		mParent(nullptr),
		mLengthExpressionList(nullptr),
		mFlags(FLAG_VALUE | (isConst ? FLAG_CONST : 0))
	{}

	/// \brief Constructs a pointer type descriptor from a parent type.
	/// \param parent Pointee type descriptor.
	/// \param isConst True to mark the pointer as const-qualified.
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

	/// \brief Returns the base type specifier for value types.
	TypeSpecifier *GetTypeSpecifier() { return const_cast<TypeSpecifier *>(mTypeSpecifier); }

	/// \brief Returns the base type specifier for value types.
	const TypeSpecifier *GetTypeSpecifier() const { return mTypeSpecifier; }

	/// \brief Returns the parent type for pointers and arrays.
	TypeDescriptor *GetParent() { return mParent; }

	/// \brief Returns the parent type for pointers and arrays.
	const TypeDescriptor *GetParent() const { return mParent; }

	/// \brief Sets the parent type for pointers and arrays.
	/// \param parent Parent type descriptor.
	void SetParent(TypeDescriptor *parent) { mParent = parent; }

	/// \brief Returns array length expressions for array types.
	Expression *GetLengthExpressionList() { return mLengthExpressionList; }

	/// \brief Returns array length expressions for array types.
	const Expression *GetLengthExpressionList() const { return mLengthExpressionList; }

	/// \brief Converts this descriptor to an array type.
	/// \param expressionList List of array length expressions.
	void ConvertToArray(Expression *expressionList);

	/// \brief Converts this descriptor to the intrinsic pointer type.
	void ConvertToPointerIntrinsic();

	/// \brief Returns a descriptor for the dereferenced type.
	TypeDescriptor GetDereferencedType() const;

	/// \brief Returns a descriptor for one array element.
	TypeDescriptor GetArrayElementType() const;

	/// \brief Returns true if this descriptor has a defined storage category.
	bool IsDefined() const { return (mFlags & STORAGE_MASK) != 0; }

	/// \brief Returns true if all referenced type information is resolved.
	bool IsResolved() const;

	/// \brief Returns true if values of this type can be instantiated.
	bool IsInstantiable() const;

	/// \brief Returns true if this descriptor is const-qualified.
	bool IsConst() const { return (mFlags & FLAG_CONST) != 0; }

	/// \brief Marks this descriptor as const-qualified.
	void SetConst() { mFlags |= FLAG_CONST; }

	/// \brief Clears const qualification from this descriptor.
	void ClearConst() { mFlags &= ~FLAG_CONST; }

	/// \brief Returns true if this descriptor denotes an addressable lvalue.
	bool IsAddressable() const { return (mFlags & FLAG_ADDRESSABLE) != 0; }

	/// \brief Marks this descriptor as an addressable lvalue.
	void SetAddressable() { mFlags |= FLAG_ADDRESSABLE; }

	/// \brief Returns true if this descriptor denotes an unaddressable lvalue.
	bool IsUnaddressableLValue() const { return (mFlags & FLAG_UNADDRESSABLE_LVALUE) != 0; }

	/// \brief Marks this descriptor as an unaddressable lvalue.
	void SetUnaddressableLValue() { mFlags |= FLAG_UNADDRESSABLE_LVALUE; }

	/// \brief Returns true if this descriptor denotes any lvalue.
	bool IsLValue() const { return (mFlags & FLAG_ANY_LVALUE) != 0; }

	/// \brief Clears all lvalue flags.
	void ClearLValue() { mFlags &= ~FLAG_ANY_LVALUE; }

	/// \brief Returns true if assignment into this type is allowed.
	bool IsAssignable() const { return IsLValue() && !IsConst() && !IsArrayType(); }

	/// \brief Returns the stack storage size of this type.
	/// \param pointerSize Target pointer size.
	uint32_t GetStackSize(PointerSize pointerSize) const;

	/// \brief Returns the object size of this type.
	/// \param pointerSize Target pointer size.
	uint32_t GetSize(PointerSize pointerSize) const;

	/// \brief Returns the object alignment of this type.
	/// \param pointerSize Target pointer size.
	uint32_t GetAlignment(PointerSize pointerSize) const;

	/// \brief Returns the signature encoding type for this descriptor.
	SignatureType GetSignatureType() const;

	/// \brief Returns the primitive token type, if applicable.
	Token::TokenType GetPrimitiveType() const;

	/// \brief Returns true if this descriptor is a primitive type.
	bool IsPrimitiveType() const { return GetPrimitiveType() != Token::INVALID; }

	/// \brief Returns true if this descriptor is a boolean type.
	bool IsBooleanType() const;

	/// \brief Returns true if this descriptor is a character type.
	bool IsCharType() const;

	/// \brief Returns true if this descriptor is an integer type.
	bool IsIntegerType() const;

	/// \brief Returns true if this descriptor is at least 32-bit integer type.
	bool IsLeast32IntegerType() const;

	/// \brief Returns true if this descriptor is at most 32-bit integer type.
	bool IsMost32IntegerType() const;

	/// \brief Returns true if this descriptor is numeric.
	bool IsNumericType() const;

	/// \brief Returns true if this descriptor is a value type.
	bool IsValueType() const { return (mFlags & FLAG_VALUE) != 0; }

	/// \brief Returns true if this descriptor is an array type.
	bool IsArrayType() const { return (mFlags & FLAG_ARRAY) != 0; }

	/// \brief Returns true if this descriptor is the null type.
	bool IsNullType() const { return (mFlags & FLAG_NULL) != 0; }

	/// \brief Returns true if this descriptor is the intrinsic pointer type.
	bool IsPointerIntrinsicType() const { return (mFlags & FLAG_POINTER) != 0; }

	/// \brief Returns true if this descriptor is any pointer-like type.
	bool IsPointerType() const { return  (mFlags & FLAG_ANY_POINTER) != 0; }

	/// \brief Returns true if this pointer type can be implicitly converted.
	bool IsImplicitlyConvertiblePointerType() const { return IsNullType() || IsVoidPointerType(); }

	/// \brief Returns true if this descriptor is a void pointer type.
	bool IsVoidPointerType() const;

	/// \brief Returns true if this descriptor is the void type.
	bool IsVoidType() const;

	/// \brief Returns true if this descriptor denotes a struct type.
	bool IsStructType() const;

	/// \brief Returns true if this descriptor denotes the string type.
	bool IsStringType() const;

	/// \brief Returns a descriptor for the void type.
	static TypeDescriptor GetVoidType();

	/// \brief Returns a descriptor for the bool type.
	static TypeDescriptor GetBoolType();

	/// \brief Returns a descriptor for the char type.
	static TypeDescriptor GetCharType();

	/// \brief Returns a descriptor for the int type.
	static TypeDescriptor GetIntType();

	/// \brief Returns a descriptor for the uint type.
	static TypeDescriptor GetUIntType();

	/// \brief Returns a descriptor for the long type.
	static TypeDescriptor GetLongType();

	/// \brief Returns a descriptor for the ulong type.
	static TypeDescriptor GetULongType();

	/// \brief Returns a descriptor for the float type.
	static TypeDescriptor GetFloatType();

	/// \brief Returns a descriptor for the double type.
	static TypeDescriptor GetDoubleType();

	/// \brief Returns a descriptor for the string type.
	static TypeDescriptor GetStringType();

	/// \brief Returns a descriptor for the void pointer type.
	static TypeDescriptor GetVoidPointerType();

	/// \brief Returns a descriptor for a pointer to the given type.
	/// \param specifier Type specifier for the pointee type.
	static TypeDescriptor GetPointerType(const TypeSpecifier *specifier);

	/// \brief Returns a const-qualified descriptor for a pointer to the given type.
	/// \param specifier Type specifier for the pointee type.
	static TypeDescriptor GetConstPointerType(const TypeSpecifier *specifier);

	/// \brief Returns a descriptor for the null type.
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


/// \brief Represents a parsed type specifier.
class TypeSpecifier: public ParseNode
{
public:

	/// \brief Constructs a type specifier from a primitive token and optional identifier.
	/// \param primitiveType Primitive type token, or null for non-primitive types.
	/// \param identifier Optional qualified identifier for user-defined types.
	explicit TypeSpecifier(const Token *primitiveType, QualifiedIdentifier *identifier = nullptr):
		mPrimitiveType(primitiveType),
		mIdentifier(identifier),
		mDefinition(nullptr)
	{}

	/// \brief Constructs a non-primitive type specifier from an identifier.
	/// \param identifier Qualified identifier for the referenced type.
	explicit TypeSpecifier(QualifiedIdentifier *identifier):
		mPrimitiveType(nullptr),
		mIdentifier(identifier),
		mDefinition(nullptr)
	{}

	/// \brief Constructs a fully specified type specifier.
	/// \param primitiveType Primitive type token, or null for non-primitive types.
	/// \param identifier Qualified identifier for user-defined types.
	/// \param definition Resolved symbol definition for this type.
	TypeSpecifier(const Token *primitiveType, QualifiedIdentifier *identifier, const Symbol *definition):
		mPrimitiveType(primitiveType),
		mIdentifier(identifier),
		mDefinition(definition)
	{}

	virtual ~TypeSpecifier() {}

	virtual void Accept(ParseNodeVisitor &visitor) override { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const override { visitor.Visit(this); }

	virtual const Token *GetContextToken() const override;

	/// \brief Returns the primitive type token.
	/// \returns Primitive type token, or null for non-primitive types.
	const Token *GetPrimitiveTypeToken() const { return mPrimitiveType; }

	/// \brief Returns the qualified identifier for this type.
	QualifiedIdentifier *GetIdentifier() { return mIdentifier; }

	/// \brief Returns the qualified identifier for this type.
	const QualifiedIdentifier *GetIdentifier() const { return mIdentifier; }

	/// \brief Returns the resolved symbol definition for this type.
	const Symbol *GetDefinition() const { return mDefinition; }

	/// \brief Sets the resolved symbol definition for this type.
	/// \param symbol Symbol that defines this type.
	void SetDefinition(const Symbol *symbol) { mDefinition = symbol; }

	/// \brief Returns true if this type specifier has been resolved.
	bool IsResolved() const;

	/// \brief Returns true if values of this type can be instantiated.
	bool IsInstantiable() const;

	/// \brief Returns the object size of this type.
	/// \param pointerSize Target pointer size.
	uint32_t GetSize(PointerSize pointerSize) const;

	/// \brief Returns the alignment of this type.
	uint32_t GetAlignment() const;

	/// \brief Returns the signature encoding type for this type.
	SignatureType GetSignatureType() const;

	/// \brief Returns the primitive token type, if applicable.
	Token::TokenType GetPrimitiveType() const;

	/// \brief Returns true if this type is boolean.
	bool IsBooleanType() const;

	/// \brief Returns true if this type is character.
	bool IsCharType() const;

	/// \brief Returns true if this type is integer.
	bool IsIntegerType() const;

	/// \brief Returns true if this type is at least 32-bit integer.
	bool IsLeast32IntegerType() const;

	/// \brief Returns true if this type is at most 32-bit integer.
	bool IsMost32IntegerType() const;

	/// \brief Returns true if this type is numeric.
	bool IsNumericType() const;

	/// \brief Returns true if this type is void.
	bool IsVoidType() const;

	/// \brief Returns true if this type is a struct type.
	bool IsStructType() const;

private:
	const Token *mPrimitiveType;
	QualifiedIdentifier *mIdentifier;
	const Symbol *mDefinition;
};


/// \brief Represents a potentially qualified identifier.
class QualifiedIdentifier: public ListParseNode
{
public:
	/// \brief Constructs a qualified identifier element.
	/// \param name Token containing one identifier segment.
	explicit QualifiedIdentifier(const Token *name): mName(name) {}
	virtual ~QualifiedIdentifier() {}

	virtual void Accept(ParseNodeVisitor &visitor) override { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const override { visitor.Visit(this); }

	virtual const Token *GetContextToken() const override { return mName; }

	/// \brief Returns this identifier segment token.
	const Token *GetName() const { return mName; }

	/// \brief Returns the next identifier segment.
	/// \returns Next segment, or null if this segment is terminal.
	QualifiedIdentifier *GetNextIdentifier() { return static_cast<QualifiedIdentifier *>(GetNextNode()); }

	/// \brief Returns the next identifier segment.
	/// \returns Next segment, or null if this segment is terminal.
	const QualifiedIdentifier *GetNextIdentifier() const { return static_cast<const QualifiedIdentifier *>(GetNextNode()); }

	/// \brief Returns true if this is the last identifier segment.
	bool IsTerminal() const { return GetNextNode() == nullptr; }

private:
	const Token *mName;
};


/// \brief Represents a complete parsed source file.
class TranslationUnit: public ListParseNode
{
public:
	/// \brief Constructs a translation unit.
	/// \param includeDirectiveList List of include directives.
	/// \param declarationList List of top-level declarations.
	TranslationUnit(IncludeDirective *includeDirectiveList, ListParseNode *declarationList):
		mIncludeDirectiveList(includeDirectiveList),
		mDeclarationList(declarationList),
		mRequiresCodeGeneration(true)
	{}

	virtual ~TranslationUnit() {}

	virtual void Accept(ParseNodeVisitor &visitor) override { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const override { visitor.Visit(this); }

	virtual const Token *GetContextToken() const override { return nullptr; }

	/// \brief Returns the include directive list.
	IncludeDirective *GetIncludeDirectiveList() { return mIncludeDirectiveList; }

	/// \brief Returns the include directive list.
	const IncludeDirective *GetIncludeDirectiveList() const { return mIncludeDirectiveList; }


	/// \brief Returns the list of top-level declarations.
	ListParseNode *GetExternalDeclarationList() { return mDeclarationList; }

	/// \brief Returns the list of top-level declarations.
	const ListParseNode *GetExternalDeclarationList() const { return mDeclarationList; }

	/// \brief Returns true if this unit should produce code.
	bool RequiresCodeGeneration() const { return mRequiresCodeGeneration; }

	/// \brief Sets whether this translation unit should produce code.
	/// \param required True to enable code generation.
	void SetRequiresCodeGeneration(bool required) { mRequiresCodeGeneration = required; }

private:
	IncludeDirective *mIncludeDirectiveList;
	ListParseNode *mDeclarationList;
	bool mRequiresCodeGeneration;
};


/// \brief Represents an include directive.
class IncludeDirective: public ListParseNode
{
public:
	/// \brief Constructs an include directive node.
	/// \param includePath Token containing the include path.
	explicit IncludeDirective(const Token *includePath): mIncludePath(includePath) {}
	virtual ~IncludeDirective() {}

	virtual void Accept(ParseNodeVisitor &visitor) override { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const override { visitor.Visit(this); }

	virtual const Token *GetContextToken() const override { return mIncludePath; }

	/// \brief Returns the include path token.
	const Token *GetIncludePath() const { return mIncludePath; }

private:
	const Token *mIncludePath;
};


/// \brief Represents a namespace definition.
class NamespaceDefinition: public Symbol
{
public:
	/// \brief Constructs a namespace definition.
	/// \param name Namespace name token.
	/// \param declarationList List of declarations in the namespace.
	NamespaceDefinition(const Token *name, ListParseNode *declarationList):
		mName(name),
		mDeclarationList(declarationList)
	{}

	virtual ~NamespaceDefinition() {}

	virtual void Accept(ParseNodeVisitor &visitor) override { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const override { visitor.Visit(this); }

	virtual SymbolType GetSymbolType() const override { return TYPE_NAMESPACE; }
	virtual const Token *GetName() const override { return mName; }

	/// \brief Returns declarations contained in this namespace.
	ListParseNode *GetExternalDeclarationList() { return mDeclarationList; }

	/// \brief Returns declarations contained in this namespace.
	const ListParseNode *GetExternalDeclarationList() const { return mDeclarationList; }

private:
	const Token *mName;
	ListParseNode *mDeclarationList;
};


/// \brief Represents a native declaration block.
class NativeBlock: public ListParseNode
{
public:
	/// \brief Constructs a native block.
	/// \param keyword Native block keyword token.
	/// \param declarationList List of declarations in the block.
	NativeBlock(const Token *keyword, ListParseNode *declarationList):
		mKeyword(keyword),
		mDeclarationList(declarationList)
	{}

	virtual ~NativeBlock() {}

	virtual void Accept(ParseNodeVisitor &visitor) override { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const override { visitor.Visit(this); }

	virtual const Token *GetContextToken() const override { return mKeyword; }

	/// \brief Returns the token introducing this native block.
	const Token *GetKeyword() const { return mKeyword; }

	/// \brief Returns native declarations contained in this block.
	ListParseNode *GetNativeDeclarationList() { return mDeclarationList; }

	/// \brief Returns native declarations contained in this block.
	const ListParseNode *GetNativeDeclarationList() const { return mDeclarationList; }

private:
	const Token *mKeyword;
	ListParseNode *mDeclarationList;
};


/// \brief Represents an enum declaration.
class EnumDeclaration: public Symbol
{
public:
	/// \brief Constructs an enum declaration.
	/// \param name Enum name token.
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

	/// \brief Returns the enum's underlying type descriptor.
	TypeDescriptor &GetTypeDescriptor() { return mTypeDescriptor; }

	/// \brief Returns the enum's underlying type descriptor.
	const TypeDescriptor &GetTypeDescriptor() const { return mTypeDescriptor; }

	/// \brief Returns the list of enumerators.
	Enumerator *GetEnumeratorList() { return mEnumeratorList; }

	/// \brief Returns the list of enumerators.
	const Enumerator *GetEnumeratorList() const { return mEnumeratorList; }

	/// \brief Sets the list of enumerators.
	/// \param enumeratorList Head of the enumerator list.
	void SetEnumeratorList(Enumerator *enumeratorList) { mEnumeratorList = enumeratorList; }

private:
	QualifiedIdentifier mIdentifier;
	TypeSpecifier mTypeSpecifier;
	TypeDescriptor mTypeDescriptor;
	Enumerator *mEnumeratorList;
};


/// \brief Represents a single enum value.
class Enumerator: public Symbol
{
public:
	/// \brief Constructs an enumerator.
	/// \param name Enumerator name token.
	/// \param typeDescriptor Type descriptor associated with the enumerator.
	/// \param value Optional explicit value expression.
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

	/// \brief Returns the enumerator value expression.
	Expression *GetValue() { return mValue; }

	/// \brief Returns the enumerator value expression.
	const Expression *GetValue() const { return mValue; }

private:
	TypeAndValue mTypeAndValue;
	const Token *mName;
	Expression *mValue;
};


/// \brief Represents a struct declaration.
class StructDeclaration: public Symbol
{
public:
	/// \brief Identifies the implementation variant of a struct.
	enum Variant
	{
		/// \brief Bond-defined struct.
		VARIANT_BOND,

		/// \brief Struct mapped to a native type.
		VARIANT_NATIVE,

		/// \brief Stub for a native struct declaration.
		VARIANT_NATIVE_STUB
	};

	/// \brief Constructs a struct declaration.
	/// \param name Struct name token.
	/// \param size Optional token describing explicit size metadata.
	/// \param alignment Optional token describing explicit alignment metadata.
	/// \param memberFunctionList List of member functions.
	/// \param memberVariableList List of member variables.
	/// \param variant Struct implementation variant.
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

	/// \brief Returns true when this struct and its members are resolved.
	virtual bool IsResolved() const override;

	/// \brief Returns the descriptor of this-parameter type.
	TypeDescriptor &GetThisTypeDescriptor() { return mThisTypeDescriptor; }

	/// \brief Returns the descriptor of this-parameter type.
	const TypeDescriptor &GetThisTypeDescriptor() const { return mThisTypeDescriptor; }

	/// \brief Returns the descriptor of const this-parameter type.
	TypeDescriptor &GetConstThisTypeDescriptor() { return mConstThisTypeDescriptor; }

	/// \brief Returns the descriptor of const this-parameter type.
	const TypeDescriptor &GetConstThisTypeDescriptor() const { return mConstThisTypeDescriptor; }

	/// \brief Returns the token specifying struct size metadata.
	const Token *GetSizeToken() const { return mSizeToken; }

	/// \brief Returns the token specifying struct alignment metadata.
	const Token *GetAlignmentToken() const { return mAlignmentToken; }

	/// \brief Returns the list of member functions.
	FunctionDefinition *GetMemberFunctionList() { return mMemberFunctionList; }

	/// \brief Returns the list of member functions.
	const FunctionDefinition *GetMemberFunctionList() const { return mMemberFunctionList; }

	/// \brief Sets the list of member functions.
	/// \param memberFunctionList Head of member function list.
	void SetMemberFunctionList(FunctionDefinition *memberFunctionList) { mMemberFunctionList = memberFunctionList; }

	/// \brief Returns the list of member variables.
	DeclarativeStatement *GetMemberVariableList() { return mMemberVariableList; }

	/// \brief Returns the list of member variables.
	const DeclarativeStatement *GetMemberVariableList() const { return mMemberVariableList; }

	/// \brief Sets the list of member variables.
	/// \param memberVariableList Head of member variable list.
	void SetMemberVariableList(DeclarativeStatement *memberVariableList) { mMemberVariableList = memberVariableList; }

	/// \brief Returns the struct used to determine this struct's size.
	const StructDeclaration *GetSizeSpecifier() const { return mSizeSpecifier; }

	/// \brief Sets the struct used to determine this struct's size.
	/// \param sizeSpecifier Struct whose size metadata should be used.
	void SetSizeSpecifier(const StructDeclaration *sizeSpecifier) { mSizeSpecifier = sizeSpecifier; }

	/// \brief Returns this struct declaration's variant.
	Variant GetVariant() const { return mVariant; }

	/// \brief Returns true if this struct is native-backed.
	bool IsNative() const { return mVariant != VARIANT_BOND; }

	/// \brief Returns the resolved size in bytes.
	uint32_t GetSize() const { return mSize; }

	/// \brief Sets the resolved size in bytes.
	/// \param size Struct size in bytes.
	void SetSize(uint32_t size) { mSize = size; }

	/// \brief Returns the resolved alignment in bytes.
	uint32_t GetAlignment() const { return mAlignment; }

	/// \brief Sets the resolved alignment in bytes.
	/// \param alignment Struct alignment in bytes.
	void SetAlignment(uint32_t alignment) { mAlignment = alignment; }

	/// \brief Returns true if instances of this struct can be created.
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


/// \brief Represents a function signature.
class FunctionPrototype: public ParseNode
{
public:
	/// \brief Constructs a function prototype.
	/// \param name Function name token.
	/// \param returnType Function return type descriptor.
	/// \param parameterList Function parameter list.
	/// \param isConst True if this is a const-qualified member function.
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

	/// \brief Returns the function name token.
	const Token *GetName() const { return mName; }

	/// \brief Returns the return type descriptor.
	TypeDescriptor *GetReturnType() { return mReturnType; }

	/// \brief Returns the return type descriptor.
	const TypeDescriptor *GetReturnType() const { return mReturnType; }

	/// \brief Returns the parameter list.
	Parameter *GetParameterList() { return mParameterList; }

	/// \brief Returns the parameter list.
	const Parameter *GetParameterList() const { return mParameterList; }

	/// \brief Returns true if the function is const-qualified.
	bool IsConst() const { return mIsConst; }

private:
	const Token *mName;
	TypeDescriptor *mReturnType;
	Parameter *mParameterList;
	bool mIsConst;
};


/// \brief Represents a function declaration or definition.
class FunctionDefinition: public Symbol
{
public:
	/// \brief Constructs a function definition node.
	/// \param prototype Function prototype.
	/// \param body Function body, or null for native functions.
	/// \param thisTypeDescriptor This-parameter type for member functions.
	/// \param scope Scope in which the function is declared.
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

	/// \brief Returns the function prototype.
	FunctionPrototype *GetPrototype() { return mPrototype; }

	/// \brief Returns the function prototype.
	const FunctionPrototype *GetPrototype() const { return mPrototype; }

	/// \brief Returns the function body.
	CompoundStatement *GetBody() { return mBody; }

	/// \brief Returns the function body.
	const CompoundStatement *GetBody() const { return mBody; }

	/// \brief Returns the this-parameter type descriptor.
	TypeDescriptor *GetThisTypeDescriptor() { return mThisTypeDescriptor; }

	/// \brief Returns the this-parameter type descriptor.
	const TypeDescriptor *GetThisTypeDescriptor() const { return mThisTypeDescriptor; }

	/// \brief Returns the declaration scope of this function.
	Scope GetScope() const { return mScope; }

	/// \brief Returns true if this function is native (no Bond body).
	bool IsNative() const { return mBody == nullptr; }

	/// \brief Returns the number of reserved jump target IDs used by code generation.
	size_t GetNumReservedJumpTargetIds() const { return mNumReservedJumpTargetIds; }

	/// \brief Sets the number of reserved jump target IDs used by code generation.
	/// \param numIds Number of reserved jump target IDs.
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


/// \brief Represents a function parameter.
class Parameter: public Symbol
{
public:
	/// \brief Constructs a named parameter.
	/// \param name Parameter name token.
	/// \param typeDescriptor Parameter type descriptor.
	Parameter(const Token *name, TypeDescriptor *typeDescriptor):
		mTypeAndValue(*typeDescriptor),
		mName(name),
		mTypeDescriptor(typeDescriptor),
		mOffset(0)
	{}

	/// \brief Constructs a temporary parameter for code generation.
	/// \param typeDescriptor Parameter type descriptor.
	/// \param offset Stack or object offset used by generated accessors.
	///
	/// Used by the code generator to create a temporary Parameter object when writing
	/// the signature for native member accessors.
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

	/// \brief Returns the parameter type descriptor.
	TypeDescriptor *GetTypeDescriptor() { return mTypeDescriptor; }

	/// \brief Returns the parameter type descriptor.
	const TypeDescriptor *GetTypeDescriptor() const { return mTypeDescriptor; }

	/// \brief Returns the generated parameter offset.
	int32_t GetOffset() const { return mOffset; }

	/// \brief Sets the generated parameter offset.
	/// \param offset Stack or object offset for this parameter.
	void SetOffset(int32_t offset) const { mOffset = offset; }

private:
	TypeAndValue mTypeAndValue;
	const Token *mName;
	TypeDescriptor *mTypeDescriptor;
	// Ew. Became mutable after population of this field was moved to the code generator.
	mutable int32_t mOffset;
};


/// \brief Represents a named variable initializer.
class NamedInitializer: public Symbol
{
public:
	/// \brief Constructs a named initializer.
	/// \param name Variable name token.
	/// \param initializer Optional initializer expression or list.
	/// \param typeDescriptor Variable type descriptor.
	/// \param scope Declaration scope.
	/// \param isNative True for native struct members.
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

	/// \brief Returns the initializer expression or initializer list.
	Initializer *GetInitializer() { return mInitializer; }

	/// \brief Returns the initializer expression or initializer list.
	const Initializer *GetInitializer() const { return mInitializer; }

	/// \brief Returns the declaration scope for this initializer.
	Scope GetScope() const { return mScope; }

	/// \brief Returns the generated offset for this variable.
	int32_t GetOffset() const { return mOffset; }

	/// \brief Sets the generated offset for this variable.
	/// \param offset Stack or object offset.
	void SetOffset(int32_t offset) const { mOffset = offset; }

	/// \brief Returns true if this initializer represents a native struct member.
	bool IsNativeStructMember() const { return (mScope == SCOPE_STRUCT_MEMBER) && (mOffset < 0); }

	/// \brief Returns true if this initializer can be elided by code generation.
	bool IsElidable() const { return mIsElidable; }

	/// \brief Sets whether this initializer can be elided by code generation.
	/// \param isElidable True to mark this initializer as elidable.
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


/// \brief Represents a scalar or aggregate initializer.
class Initializer: public ListParseNode
{
public:
	/// \brief Constructs a scalar initializer.
	/// \param expression Initializer expression.
	explicit Initializer(Expression *expression):
		mOpenBrace(nullptr),
		mExpression(expression),
		mInitializerList(nullptr)
	{}

	/// \brief Constructs an aggregate initializer.
	/// \param openBrace Token that opens the initializer list.
	/// \param initializerList Nested initializer list.
	explicit Initializer(const Token *openBrace, Initializer *initializerList):
		mOpenBrace(openBrace),
		mExpression(nullptr),
		mInitializerList(initializerList)
	{}

	virtual ~Initializer() {}

	virtual void Accept(ParseNodeVisitor &visitor) override { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const override { visitor.Visit(this); }

	virtual const Token *GetContextToken() const override;

	/// \brief Returns the evaluated type descriptor for this initializer.
	const TypeDescriptor &GetTypeDescriptor() const { return mTypeDescriptor; }

	/// \brief Sets the evaluated type descriptor for this initializer.
	/// \param descriptor Type descriptor to assign.
	void SetTypeDescriptor(const TypeDescriptor &descriptor) { mTypeDescriptor = descriptor; }

	/// \brief Returns the scalar initializer expression.
	Expression *GetExpression() { return mExpression; }

	/// \brief Returns the scalar initializer expression.
	const Expression *GetExpression() const { return mExpression; }

	/// \brief Returns nested initializer list for aggregates.
	Initializer *GetInitializerList() { return mInitializerList; }

	/// \brief Returns nested initializer list for aggregates.
	const Initializer *GetInitializerList() const { return mInitializerList; }

	/// \brief Returns true if this initializer and sub-initializers are resolved.
	virtual bool IsResolved() const;

private:
	TypeDescriptor mTypeDescriptor;
	const Token *mOpenBrace;
	Expression *mExpression;
	Initializer *mInitializerList;
};


/// \brief Represents a block statement and its local scope.
class CompoundStatement: public Symbol
{
public:
	/// \brief Constructs a compound statement.
	/// \param statementList List of statements in the block.
	explicit CompoundStatement(ListParseNode *statementList): mStatementList(statementList) {}
	virtual ~CompoundStatement() {}

	virtual void Accept(ParseNodeVisitor &visitor) override { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const override { visitor.Visit(this); }

	virtual SymbolType GetSymbolType() const override { return TYPE_LOCALSCOPE; }

	/// \brief Returns the statement list in this block.
	ListParseNode *GetStatementList() { return mStatementList; }

	/// \brief Returns the statement list in this block.
	const ListParseNode *GetStatementList() const { return mStatementList; }

private:
	ListParseNode *mStatementList;
};


/// \brief Represents an if/else statement.
class IfStatement: public ListParseNode
{
public:
	/// \brief Constructs an if statement.
	/// \param keyword If keyword token.
	/// \param condition Condition expression.
	/// \param thenStatement Statement executed when condition is true.
	/// \param elseStatement Optional statement executed when condition is false.
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

	/// \brief Returns the if keyword token.
	const Token *GetKeyword() const { return mKeyword; }

	/// \brief Returns the condition expression.
	Expression *GetCondition() { return mCondition; }

	/// \brief Returns the condition expression.
	const Expression *GetCondition() const { return mCondition; }

	/// \brief Returns the then-branch statement.
	ParseNode *GetThenStatement() { return mThenStatement; }

	/// \brief Returns the then-branch statement.
	const ParseNode *GetThenStatement() const { return mThenStatement; }

	/// \brief Returns the else-branch statement.
	ParseNode *GetElseStatement() { return mElseStatement; }

	/// \brief Returns the else-branch statement.
	const ParseNode *GetElseStatement() const { return mElseStatement; }

private:
	const Token *mKeyword;
	Expression *mCondition;
	ParseNode *mThenStatement;
	ParseNode *mElseStatement;
};


/// \brief Represents a switch statement.
class SwitchStatement: public ListParseNode
{
public:
	/// \brief Constructs a switch statement.
	/// \param keyword Switch keyword token.
	/// \param control Switch control expression.
	/// \param sectionList List of switch sections.
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

	/// \brief Returns the switch keyword token.
	const Token *GetKeyword() const { return mKeyword; }

	/// \brief Returns the switch control expression.
	Expression *GetControl() { return mControl; }

	/// \brief Returns the switch control expression.
	const Expression *GetControl() const { return mControl; }

	/// \brief Returns the list of switch sections.
	SwitchSection *GetSectionList() { return mSectionList; }

	/// \brief Returns the list of switch sections.
	const SwitchSection *GetSectionList() const { return mSectionList; }

	/// \brief Returns the resolved linearized label list.
	const ResolvedSwitchLabel *GetResolvedLabelList() const { return mResolvedLabelList; }

	/// \brief Sets the resolved linearized label list.
	/// \param labelList Head of resolved switch labels.
	void SetResolvedLabelList(const ResolvedSwitchLabel *labelList) { mResolvedLabelList = labelList; }

	/// \brief Returns number of non-default case matches.
	uint32_t GetNumMatches() const { return mNumMatches; }

	/// \brief Sets number of non-default case matches.
	/// \param numMatches Number of case matches.
	void SetNumMatches(uint32_t numMatches) { mNumMatches = numMatches; }

	/// \brief Returns smallest case match value.
	int32_t GetMinMatch() const { return mMinMatch; }

	/// \brief Sets smallest case match value.
	/// \param minMatch Smallest match value.
	void SetMinMatch(int32_t minMatch) { mMinMatch = minMatch; }

	/// \brief Returns largest case match value.
	int32_t GetMaxMatch() const { return mMaxMatch; }

	/// \brief Sets largest case match value.
	/// \param maxMatch Largest match value.
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


/// \brief Represents one labeled section inside a switch statement.
class SwitchSection: public Symbol
{
public:
	/// \brief Constructs a switch section.
	/// \param labelList List of case/default labels.
	/// \param statementList List of statements for this section.
	SwitchSection(SwitchLabel *labelList, ListParseNode* statementList):
		mLabelList(labelList),
		mStatementList(statementList),
		mJumpTargetId(0)
	{}

	virtual ~SwitchSection() {}

	virtual void Accept(ParseNodeVisitor &visitor) override { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const override { visitor.Visit(this); }

	virtual SymbolType GetSymbolType() const override { return TYPE_LOCALSCOPE; }

	/// \brief Returns the label list for this section.
	SwitchLabel *GetLabelList() { return mLabelList; }

	/// \brief Returns the label list for this section.
	const SwitchLabel *GetLabelList() const { return mLabelList; }

	/// \brief Returns the statement list for this section.
	ListParseNode *GetStatementList() { return mStatementList; }

	/// \brief Returns the statement list for this section.
	const ListParseNode *GetStatementList() const { return mStatementList; }

	/// \brief Returns the jump target ID assigned to this section.
	size_t GetJumpTargetId() const { return mJumpTargetId; }

	/// \brief Sets the jump target ID assigned to this section.
	/// \param id Jump target identifier.
	void SetJumpTargetId(size_t id) { mJumpTargetId = id; }

private:
	SwitchLabel *mLabelList;
	ListParseNode *mStatementList;
	size_t mJumpTargetId;
};


/// \brief Represents a resolved switch label used for code generation.
class ResolvedSwitchLabel: public ListParseNode
{
public:
	/// \brief Constructs a resolved switch label with default values.
	ResolvedSwitchLabel(): mMatch(0), mJumpTargetId(0), mIsDefault(false) {}

	virtual void Accept(ParseNodeVisitor &visitor) override {}
	virtual void Accept(ParseNodeVisitor &visitor) const override {}
	virtual const Token *GetContextToken() const override { return nullptr; }

	/// \brief Returns the resolved match value.
	int32_t GetMatch() const { return mMatch; }

	/// \brief Sets the resolved match value.
	/// \param match Case match value.
	void SetMatch(int32_t match) { mMatch = match; }

	/// \brief Returns the jump target ID associated with this label.
	size_t GetJumpTargetId() const { return mJumpTargetId; }

	/// \brief Sets the jump target ID associated with this label.
	/// \param targetId Jump target identifier.
	void SetJumpTargetId(size_t targetId) { mJumpTargetId = targetId; }

	/// \brief Returns true if this is the default label.
	bool IsDefault() const { return mIsDefault; }

	/// \brief Sets whether this is the default label.
	/// \param isDefault True to mark this label as default.
	void SetIsDefault(bool isDefault) { mIsDefault = isDefault; }

	/// \brief Compares two resolved labels by default flag and match value.
	/// \param other Label to compare against.
	/// \returns True if labels represent the same match semantics.
	bool operator==(const ResolvedSwitchLabel &other) const { return (mIsDefault == other.mIsDefault) && (mMatch == other.mMatch); }

private:
	int32_t mMatch;
	size_t mJumpTargetId;
	bool mIsDefault;
};


/// \brief Represents a case or default label in a switch section.
class SwitchLabel: public ListParseNode
{
public:
	/// \brief Constructs a default-style switch label with no expression.
	/// \param label Label token.
	explicit SwitchLabel(const Token *label):
		mLabel(label),
		mExpression(nullptr)
	{}

	/// \brief Constructs a case label with an expression.
	/// \param label Label token.
	/// \param expression Case match expression.
	SwitchLabel(const Token *label, Expression *expression):
		mLabel(label),
		mExpression(expression)
	{}

	virtual ~SwitchLabel() {}

	virtual void Accept(ParseNodeVisitor &visitor) override { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const override { visitor.Visit(this); }

	virtual const Token *GetContextToken() const override { return mLabel; }

	/// \brief Returns the label token.
	const Token *GetLabel() const { return mLabel; }

	/// \brief Returns the case expression.
	Expression *GetExpression() { return mExpression; }

	/// \brief Returns the case expression.
	const Expression *GetExpression() const { return mExpression; }

	/// \brief Returns resolved label metadata.
	ResolvedSwitchLabel &GetResolvedLabel() { return mResolvedLabel; }

	/// \brief Returns resolved label metadata.
	const ResolvedSwitchLabel &GetResolvedLabel() const { return mResolvedLabel; }

	/// \brief Returns true if this label is a default label.
	bool IsDefaultLabel() const { return mLabel->GetTokenType() == Token::KEY_DEFAULT; }

private:
	ResolvedSwitchLabel mResolvedLabel;
	const Token *mLabel;
	Expression *mExpression;
};


/// \brief Represents a while or do-while loop.
class WhileStatement: public ListParseNode
{
public:
	/// \brief Constructs a while/do-while statement.
	/// \param keyword Loop keyword token.
	/// \param condition Loop condition expression.
	/// \param body Loop body.
	WhileStatement(const Token *keyword, Expression *condition, ParseNode *body):
		mKeyword(keyword),
		mCondition(condition),
		mBody(body)
	{}

	virtual ~WhileStatement() {}

	virtual void Accept(ParseNodeVisitor &visitor) override { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const override { visitor.Visit(this); }

	virtual const Token *GetContextToken() const override { return mKeyword; }

	/// \brief Returns the loop keyword token.
	const Token *GetKeyword() const { return mKeyword; }

	/// \brief Returns the loop condition expression.
	Expression *GetCondition() { return mCondition; }

	/// \brief Returns the loop condition expression.
	const Expression *GetCondition() const { return mCondition; }

	/// \brief Returns the loop body.
	ParseNode *GetBody() { return mBody; }

	/// \brief Returns the loop body.
	const ParseNode *GetBody() const { return mBody; }

	/// \brief Returns true when this node represents a do-while loop.
	bool IsDoLoop() const { return mKeyword->GetTokenType() == Token::KEY_DO; }

private:
	const Token *mKeyword;
	Expression *mCondition;
	ParseNode *mBody;
};


/// \brief Represents a for loop and its local scope.
class ForStatement: public Symbol
{
public:
	/// \brief Constructs a for statement.
	/// \param keyword For keyword token.
	/// \param initializer Optional initializer statement.
	/// \param condition Optional condition expression.
	/// \param countingExpression Optional counting expression.
	/// \param body Loop body.
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

	/// \brief Returns the for keyword token.
	const Token *GetKeyword() const { return mKeyword; }

	/// \brief Returns the loop initializer.
	ParseNode *GetInitializer() { return mInitializer; }

	/// \brief Returns the loop initializer.
	const ParseNode *GetInitializer() const { return mInitializer; }

	/// \brief Returns the loop condition expression.
	Expression *GetCondition() { return mCondition; }

	/// \brief Returns the loop condition expression.
	const Expression *GetCondition() const { return mCondition; }

	/// \brief Returns the loop counting expression.
	Expression *GetCountingExpression() { return mCountingExpression; }

	/// \brief Returns the loop counting expression.
	const Expression *GetCountingExpression() const { return mCountingExpression; }

	/// \brief Returns the loop body.
	ParseNode *GetBody() { return mBody; }

	/// \brief Returns the loop body.
	const ParseNode *GetBody() const { return mBody; }

private:
	const Token *mKeyword;
	ParseNode *mInitializer;
	Expression *mCondition;
	Expression *mCountingExpression;
	ParseNode *mBody;
};


/// \brief Represents a break, continue, or return statement.
class JumpStatement: public ListParseNode
{
public:
	/// \brief Constructs a jump statement.
	/// \param keyword Jump keyword token.
	/// \param rhs Optional return expression.
	JumpStatement(const Token *keyword, Expression *rhs): mKeyword(keyword), mRhs(rhs) {}
	virtual ~JumpStatement() {}

	virtual void Accept(ParseNodeVisitor &visitor) override { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const override { visitor.Visit(this); }

	virtual const Token *GetContextToken() const override { return mKeyword; }

	/// \brief Returns the jump keyword token.
	const Token *GetKeyword() const { return mKeyword; }

	/// \brief Returns the right-hand-side expression.
	Expression *GetRhs() { return mRhs; }

	/// \brief Returns the right-hand-side expression.
	const Expression *GetRhs() const { return mRhs; }

	/// \brief Returns true if this is a break statement.
	bool IsBreak() const { return mKeyword->GetTokenType() == Token::KEY_BREAK; }

	/// \brief Returns true if this is a continue statement.
	bool IsContinue() const { return mKeyword->GetTokenType() == Token::KEY_CONTINUE; }

	/// \brief Returns true if this is a return statement.
	bool IsReturn() const { return mKeyword->GetTokenType() == Token::KEY_RETURN; }

private:
	const Token *mKeyword;
	Expression *mRhs;
};


/// \brief Represents a declarative statement with one or more named initializers.
class DeclarativeStatement: public ListParseNode
{
public:
	/// \brief Constructs a declarative statement.
	/// \param typeDescriptor Common type descriptor for declared names.
	/// \param initializerList List of declared names and initializers.
	DeclarativeStatement(TypeDescriptor *typeDescriptor, NamedInitializer *initializerList):
		mTypeDescriptor(typeDescriptor),
		mNamedInitializerList(initializerList)
	{}

	virtual ~DeclarativeStatement() {}

	virtual void Accept(ParseNodeVisitor &visitor) override { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const override { visitor.Visit(this); }

	virtual const Token *GetContextToken() const override { return mTypeDescriptor->GetContextToken(); }

	/// \brief Returns the declaration type descriptor.
	const TypeDescriptor *GetTypeDescriptor() const { return mTypeDescriptor; }

	/// \brief Returns the declaration type descriptor.
	TypeDescriptor *GetTypeDescriptor() { return mTypeDescriptor; }

	/// \brief Returns list of named initializers.
	const NamedInitializer *GetNamedInitializerList() const { return mNamedInitializerList; }

	/// \brief Returns list of named initializers.
	NamedInitializer *GetNamedInitializerList() { return mNamedInitializerList; }

private:
	TypeDescriptor *mTypeDescriptor;
	NamedInitializer *mNamedInitializerList;
};


/// \brief Base class for expression nodes.
class Expression: public ListParseNode
{
public:
	virtual ~Expression() {}

	/// \brief Returns the expression's type descriptor.
	const TypeDescriptor &GetTypeDescriptor() const { return mTypeDescriptor; }

	/// \brief Sets the expression's type descriptor.
	/// \param descriptor Type descriptor to assign.
	void SetTypeDescriptor(const TypeDescriptor &descriptor) { mTypeDescriptor = descriptor; }

	/// \brief Returns type and value metadata for the expression.
	const TypeAndValue &GetTypeAndValue() const { return mTypeAndValue; }

	/// \brief Returns mutable type and value metadata for the expression.
	TypeAndValue &GetTypeAndValue() { return mTypeAndValue; }

protected:
	Expression(): mTypeAndValue(mTypeDescriptor) {}

private:
	TypeDescriptor mTypeDescriptor;
	TypeAndValue mTypeAndValue;
};


/// \brief Represents a statement that evaluates an expression.
class ExpressionStatement: public ListParseNode
{
public:
	/// \brief Constructs an expression statement.
	/// \param expression Expression to evaluate, or null for empty statements.
	explicit ExpressionStatement(Expression *expression): mExpression(expression) {}
	virtual ~ExpressionStatement() {}

	virtual void Accept(ParseNodeVisitor &visitor) override { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const override { visitor.Visit(this); }

	virtual const Token *GetContextToken() const override { return (mExpression == nullptr) ? nullptr : mExpression->GetContextToken(); }

	/// \brief Returns the statement expression.
	Expression *GetExpression() { return mExpression; }

	/// \brief Returns the statement expression.
	const Expression *GetExpression() const { return mExpression; }

private:
	Expression *mExpression;
};


/// \brief Represents a ternary conditional expression.
class ConditionalExpression: public Expression
{
public:
	/// \brief Constructs a conditional expression.
	/// \param op Conditional operator token.
	/// \param condition Condition expression.
	/// \param trueExpression Expression evaluated when condition is true.
	/// \param falseExpression Expression evaluated when condition is false.
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

	/// \brief Returns the condition expression.
	Expression *GetCondition() { return mCondition; }

	/// \brief Returns the condition expression.
	const Expression *GetCondition() const { return mCondition; }

	/// \brief Returns the true branch expression.
	Expression *GetTrueExpression() { return mTrueExpression; }

	/// \brief Returns the true branch expression.
	const Expression *GetTrueExpression() const { return mTrueExpression; }

	/// \brief Returns the false branch expression.
	Expression *GetFalseExpression() { return mFalseExpression; }

	/// \brief Returns the false branch expression.
	const Expression *GetFalseExpression() const { return mFalseExpression; }

private:
	const Token *mOperator;
	Expression *mCondition;
	Expression *mTrueExpression;
	Expression *mFalseExpression;
};


/// \brief Represents a binary operator expression.
class BinaryExpression: public Expression
{
public:
	/// \brief Constructs a binary expression.
	/// \param op Binary operator token.
	/// \param lhs Left-hand-side expression.
	/// \param rhs Right-hand-side expression.
	BinaryExpression(const Token *op, Expression *lhs, Expression *rhs):
		mOperator(op),
		mLhs(lhs),
		mRhs(rhs)
	{}

	virtual ~BinaryExpression() {}

	virtual void Accept(ParseNodeVisitor &visitor) override { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const override { visitor.Visit(this); }

	virtual const Token *GetContextToken() const override { return mOperator; }

	/// \brief Returns the binary operator token.
	const Token *GetOperator() const { return mOperator; }

	/// \brief Returns the left-hand-side expression.
	Expression *GetLhs() { return mLhs; }

	/// \brief Returns the left-hand-side expression.
	const Expression *GetLhs() const { return mLhs; }

	/// \brief Returns the right-hand-side expression.
	Expression *GetRhs() { return mRhs; }

	/// \brief Returns the right-hand-side expression.
	const Expression *GetRhs() const { return mRhs; }

private:
	const Token *mOperator;
	Expression *mLhs;
	Expression *mRhs;
};


/// \brief Represents a unary operator expression.
class UnaryExpression: public Expression
{
public:
	/// \brief Constructs a unary expression.
	/// \param op Unary operator token.
	/// \param rhs Operand expression.
	UnaryExpression(const Token *op, Expression *rhs): mOperator(op), mRhs(rhs) {}
	virtual ~UnaryExpression() {}

	virtual void Accept(ParseNodeVisitor &visitor) override { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const override { visitor.Visit(this); }

	virtual const Token *GetContextToken() const override { return mOperator; }

	/// \brief Returns the unary operator token.
	const Token *GetOperator() const { return mOperator; }

	/// \brief Returns the operand expression.
	Expression *GetRhs() { return mRhs; }

	/// \brief Returns the operand expression.
	const Expression *GetRhs() const { return mRhs; }

private:
	const Token *mOperator;
	Expression *mRhs;
};


/// \brief Represents a postfix operator expression.
class PostfixExpression: public Expression
{
public:
	/// \brief Constructs a postfix expression.
	/// \param op Postfix operator token.
	/// \param lhs Operand expression.
	PostfixExpression(const Token *op, Expression *lhs): mOperator(op), mLhs(lhs) {}
	virtual ~PostfixExpression() {}

	virtual void Accept(ParseNodeVisitor &visitor) override { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const override { visitor.Visit(this); }

	virtual const Token *GetContextToken() const override { return mOperator; }

	/// \brief Returns the postfix operator token.
	const Token *GetOperator() const { return mOperator; }

	/// \brief Returns the operand expression.
	Expression *GetLhs() { return mLhs; }

	/// \brief Returns the operand expression.
	const Expression *GetLhs() const { return mLhs; }

private:
	const Token *mOperator;
	Expression *mLhs;
};


/// \brief Represents member access on a struct-like value.
class MemberExpression: public Expression
{
public:
	/// \brief Constructs a member access expression.
	/// \param op Member access operator token.
	/// \param memberName Member name token.
	/// \param lhs Left-hand-side object expression.
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

	/// \brief Returns the member access operator token.
	const Token *GetOperator() const { return mOperator; }

	/// \brief Returns the member name token.
	const Token *GetMemberName() const { return mMemberName; }

	/// \brief Returns the left-hand-side object expression.
	Expression *GetLhs() { return mLhs; }

	/// \brief Returns the left-hand-side object expression.
	const Expression *GetLhs() const { return mLhs; }

	/// \brief Returns resolved symbol definition for the member.
	const Symbol *GetDefinition() const { return mDefinition; }

	/// \brief Sets resolved symbol definition for the member.
	/// \param symbol Symbol resolved for the member access.
	void SetDefinition(const Symbol *symbol) { mDefinition = symbol; }

private:
	const Token *mOperator;
	const Token *mMemberName;
	Expression *mLhs;
	const Symbol *mDefinition;
};


/// \brief Represents an array subscript expression.
class ArraySubscriptExpression: public Expression
{
public:
	/// \brief Constructs an array subscript expression.
	/// \param op Subscript operator context token.
	/// \param lhs Array or pointer expression.
	/// \param index Index expression.
	ArraySubscriptExpression(const Token *op, Expression *lhs, Expression *index):
		mOperator(op),
		mLhs(lhs),
		mIndex(index)
	{}

	virtual ~ArraySubscriptExpression() {}

	virtual void Accept(ParseNodeVisitor &visitor) override { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const override { visitor.Visit(this); }

	virtual const Token *GetContextToken() const override { return mOperator; }

	/// \brief Returns the subscript operator context token.
	const Token *GetOperator() const { return mOperator; }

	/// \brief Returns the array or pointer expression.
	Expression *GetLhs() { return mLhs; }

	/// \brief Returns the array or pointer expression.
	const Expression *GetLhs() const { return mLhs; }

	/// \brief Returns the index expression.
	Expression *GetIndex() { return mIndex; }

	/// \brief Returns the index expression.
	const Expression *GetIndex() const { return mIndex; }

private:
	const Token *mOperator;
	Expression *mLhs;
	Expression *mIndex;
};


/// \brief Represents a function call expression.
class FunctionCallExpression: public Expression
{
public:
	/// \brief Constructs a function call expression.
	/// \param context Token used for diagnostics.
	/// \param lhs Callable expression.
	/// \param argumentList Argument expression list.
	FunctionCallExpression(const Token *context, Expression *lhs, Expression *argumentList):
		mContext(context),
		mLhs(lhs),
		mArgumentList(argumentList)
	{}

	virtual ~FunctionCallExpression() {}

	virtual void Accept(ParseNodeVisitor &visitor) override { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const override { visitor.Visit(this); }

	virtual const Token *GetContextToken() const override { return mContext; }

	/// \brief Returns the callable expression.
	Expression *GetLhs() { return mLhs; }

	/// \brief Returns the callable expression.
	const Expression *GetLhs() const { return mLhs; }

	/// \brief Returns the argument expression list.
	Expression *GetArgumentList() { return mArgumentList; }

	/// \brief Returns the argument expression list.
	const Expression *GetArgumentList() const { return mArgumentList; }

private:
	const Token *mContext;
	Expression *mLhs;
	Expression *mArgumentList;
};


/// \brief Represents an explicit cast expression.
class CastExpression: public Expression
{
public:
	/// \brief Constructs a cast expression.
	/// \param op Cast operator token.
	/// \param targetTypeDescriptor Target type descriptor.
	/// \param rhs Source expression.
	CastExpression(const Token *op, TypeDescriptor *targetTypeDescriptor, Expression *rhs):
		mOperator(op),
		mTargetTypeDescriptor(targetTypeDescriptor),
		mRhs(rhs)
	{}

	virtual ~CastExpression() {}

	virtual void Accept(ParseNodeVisitor &visitor) override { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const override { visitor.Visit(this); }

	virtual const Token *GetContextToken() const override { return mOperator; }

	/// \brief Returns the cast target type descriptor.
	const TypeDescriptor *GetTargetTypeDescriptor() const { return mTargetTypeDescriptor; }

	/// \brief Returns the cast target type descriptor.
	TypeDescriptor *GetTargetTypeDescriptor() { return mTargetTypeDescriptor; }

	/// \brief Returns the source expression.
	Expression *GetRhs() { return mRhs; }

	/// \brief Returns the source expression.
	const Expression *GetRhs() const { return mRhs; }

private:
	const Token *mOperator;
	TypeDescriptor *mTargetTypeDescriptor;
	Expression *mRhs;
};


/// \brief Represents sizeof/alignof style property queries.
class PropertyofExpression: public Expression
{
public:
	/// \brief Constructs a property query on a type.
	/// \param op Property operator token.
	/// \param targetTypeDescriptor Type descriptor to inspect.
	PropertyofExpression(const Token *op, TypeDescriptor *targetTypeDescriptor):
		mOperator(op),
		mTargetTypeDescriptor(targetTypeDescriptor),
		mRhs(nullptr)
	{}

	/// \brief Constructs a property query on an expression.
	/// \param op Property operator token.
	/// \param rhs Expression to inspect.
	PropertyofExpression(const Token *op, Expression *rhs):
		mOperator(op),
		mTargetTypeDescriptor(nullptr),
		mRhs(rhs)
	{}

	virtual ~PropertyofExpression() {}

	virtual void Accept(ParseNodeVisitor &visitor) override { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const override { visitor.Visit(this); }

	virtual const Token *GetContextToken() const override { return mOperator; }

	/// \brief Returns the property operator token.
	const Token *GetOperator() const { return mOperator; }

	/// \brief Returns the target type descriptor.
	const TypeDescriptor *GetTargetTypeDescriptor() const { return mTargetTypeDescriptor; }

	/// \brief Returns the target type descriptor.
	TypeDescriptor *GetTargetTypeDescriptor() { return mTargetTypeDescriptor; }

	/// \brief Returns the target expression.
	Expression *GetRhs() { return mRhs; }

	/// \brief Returns the target expression.
	const Expression *GetRhs() const { return mRhs; }

	/// \brief Returns true if this is an alignof query.
	bool IsAlignof() const { return mOperator->GetTokenType() == Token::KEY_ALIGNOF; }

	/// \brief Returns true if this is a sizeof query.
	bool IsSizeof() const { return mOperator->GetTokenType() == Token::KEY_SIZEOF; }

private:
	const Token *mOperator;
	TypeDescriptor *mTargetTypeDescriptor;
	Expression *mRhs;
};


/// \brief Represents a literal constant expression.
class ConstantLiteralExpression: public Expression
{
public:
	/// \brief Constructs a literal expression.
	/// \param value Literal token.
	explicit ConstantLiteralExpression(const Token *value): mValue(value) {}
	virtual ~ConstantLiteralExpression() {}

	virtual void Accept(ParseNodeVisitor &visitor) override { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const override { visitor.Visit(this); }

	virtual const Token *GetContextToken() const override { return mValue; }

	/// \brief Returns the literal token.
	const Token *GetValueToken() const { return mValue; }

private:
	const Token *mValue;
};


/// \brief Represents an identifier expression.
class IdentifierExpression: public Expression
{
public:
	/// \brief Constructs an identifier expression.
	/// \param identifier Qualified identifier.
	explicit IdentifierExpression(QualifiedIdentifier *identifier):
		mIdentifier(identifier),
		mDefinition(nullptr)
	{}

	virtual ~IdentifierExpression() {}

	virtual void Accept(ParseNodeVisitor &visitor) override { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const override { visitor.Visit(this); }

	virtual const Token *GetContextToken() const override { return mIdentifier->GetContextToken(); }

	/// \brief Returns the qualified identifier.
	QualifiedIdentifier *GetIdentifier() { return mIdentifier; }

	/// \brief Returns the qualified identifier.
	const QualifiedIdentifier *GetIdentifier() const { return mIdentifier; }

	/// \brief Returns resolved symbol definition for the identifier.
	Symbol *GetDefinition() { return mDefinition; }

	/// \brief Returns resolved symbol definition for the identifier.
	const Symbol *GetDefinition() const { return mDefinition; }

	/// \brief Sets resolved symbol definition for the identifier.
	/// \param symbol Resolved symbol definition.
	void SetDefinition(Symbol *symbol) { mDefinition = symbol; }

private:
	QualifiedIdentifier *mIdentifier;
	Symbol *mDefinition;
};


/// \brief Represents a this expression.
class ThisExpression: public Expression
{
public:
	/// \brief Constructs a this expression.
	/// \param token This keyword token.
	explicit ThisExpression(const Token *token): mToken(token) {}
	virtual ~ThisExpression() {}

	virtual void Accept(ParseNodeVisitor &visitor) override { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const override { visitor.Visit(this); }

	virtual const Token *GetContextToken() const override { return mToken; }

private:
	const Token *mToken;
};


/// \brief Represents an intentionally empty expression.
class EmptyExpression: public Expression
{
public:
	virtual ~EmptyExpression() {}

	virtual void Accept(ParseNodeVisitor &visitor) override { visitor.Visit(this); }
	virtual void Accept(ParseNodeVisitor &visitor) const override { visitor.Visit(this); }

	virtual const Token *GetContextToken() const override { return nullptr; }
};


/// \brief Canonical void type specifier instance.
extern const TypeSpecifier VOID_TYPE_SPECIFIER;

/// \brief Canonical bool type specifier instance.
extern const TypeSpecifier BOOL_TYPE_SPECIFIER;

/// \brief Canonical char type specifier instance.
extern const TypeSpecifier CHAR_TYPE_SPECIFIER;

/// \brief Canonical int type specifier instance.
extern const TypeSpecifier INT_TYPE_SPECIFIER;

/// \brief Canonical uint type specifier instance.
extern const TypeSpecifier UINT_TYPE_SPECIFIER;

/// \brief Canonical long type specifier instance.
extern const TypeSpecifier LONG_TYPE_SPECIFIER;

/// \brief Canonical ulong type specifier instance.
extern const TypeSpecifier ULONG_TYPE_SPECIFIER;

/// \brief Canonical float type specifier instance.
extern const TypeSpecifier FLOAT_TYPE_SPECIFIER;

/// \brief Canonical double type specifier instance.
extern const TypeSpecifier DOUBLE_TYPE_SPECIFIER;

/// @}

}

#endif
