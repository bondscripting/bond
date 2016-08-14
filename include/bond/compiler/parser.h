#ifndef BOND_COMPILER_PARSER_H
#define BOND_COMPILER_PARSER_H

#include "bond/compiler/parsenodes.h"
#include "bond/stl/vector.h"

namespace Bond
{

class CompilerErrorBuffer;
class Token;
class TokenCollection;
class TokenStream;

/// \addtogroup compiler
/// @{

/// \brief An owning pointer to a dynamically allocated ParseNode.
typedef Allocator::ObjectHandle<ParseNode> ParseNodeHandle;

/// \brief A collection of dynamically allocated ParseNodes.
typedef Vector<ParseNodeHandle> ParseNodeStore;


/// \brief A parser for the Bond scripting language.
///
/// The Parser parses the contents of a TokenCollection generated from a Lexer that has scanned
/// Bond source code. Its output is a parse tree of objects that derive from ParseNode. The
/// concrete type of the root node is TranslationUnit. The parse tree can then be used as input
/// to a SemanticAnalyzer and CodeGenerator. A single Parser can be used to parse several
/// collections of Tokens.
///
/// When writing a tool that requires a compiler front end, the interactions between the front end
/// components, namely a Lexer, a Parser and a SemanticAnalyzer, can be managed by a FrontEnd.
///
/// \sa CompilerErrorBuffer, FrontEnd, Parser, SemanticAnalyzer, TokenCollection, TokenStream,
/// TranslationUnit
class Parser
{
public:
	/// \brief Constructs a Parser object.
	/// \param allocator The memory allocator from which ParseNodes are allocated.
	/// \param errorBuffer Buffer where error messages are pushed when syntax errors are encountered.
	/// \param store Store of owning pointers to the ParseNodes created by this Parser.
	Parser(Allocator &allocator, CompilerErrorBuffer &errorBuffer, ParseNodeStore &store):
		mAllocator(allocator),
		mErrorBuffer(errorBuffer),
		mStore(store)
	{}

	Parser(const Parser &other) = delete;
	Parser &operator=(const Parser &other) = delete;

	/// \brief Parses a sequence of Tokens and generates a parse tree.
	/// \param collection The collection of Tokens to be parsed.
	/// \returns A parse tree of ParseNodes rooted with a TranslationUnit.
	TranslationUnit *Parse(TokenCollection &collection);

	/// \brief Parses a sequence of Tokens and generates a parse tree.
	/// \param stream The stream of Tokens to be parsed.
	/// \returns A parse tree of ParseNodes rooted with a Translationunit.
	TranslationUnit *Parse(TokenStream &stream);

	/// \brief Returns the buffer where error messages are pushed.
	const CompilerErrorBuffer &GetErrorBuffer() const { return mErrorBuffer; }

	/// \brief Returns the store of owning pointers to the ParseNodes created by this Parser.
	ParseNodeStore &GetParseNodeStore() { return mStore; }

private:
	Allocator &mAllocator;
	CompilerErrorBuffer &mErrorBuffer;
	ParseNodeStore &mStore;
};

/// @}

}

#endif
