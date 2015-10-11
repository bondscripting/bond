#ifndef BOND_COMPILER_PARSER_H
#define BOND_COMPILER_PARSER_H

#include "bond/compiler/parsenodefactory.h"
#include "bond/compiler/parsenodes.h"

namespace Bond
{

class CompilerErrorBuffer;
class Token;
class TokenCollection;
class TokenStream;

/// \brief A parser for the Bond scripting language.
///
/// The Parser parses the contents of a TokenCollection generated from a Lexer that has scanned
/// Bond source code. Its output is a parse tree of objects that derive from ParseNode. The
/// concrete type of the root node is TranslationUnit. The parse tree can then be used as input
/// to a SemanticAnalyzer and CodeGenerator.
///
/// A single Parser can be used to parse several collections of Tokens; each invocation generates
/// a new parse tree rooted with a TranslationUnit. The Parser retains an owning pointer to all
/// of the generated parse trees, all of which are destroyed when the Parser is destroyed, or
/// when its Dispose method is called.
///
/// When writing a tool that requires a compiler front end, the interactions between the front end
/// components, namely a Lexer, a Parser and a SemanticAnalyzer, can be managed by a FrontEnd.
///
/// \sa CompilerErrorBuffer, FrontEnd, Parser, SemanticAnalyzer, TokenCollection, TokenStream
class Parser
{
public:
	/// \brief Constructs a Parser object.
	/// \param allocator The memory allocator from which ParseNodes are allocated.
	/// \param errorBuffer Buffer where error messages are pushed when syntax errors are encountered.
	Parser(Allocator &allocator, CompilerErrorBuffer &errorBuffer);

	~Parser();

	/// \brief Destroys all generated parse trees.
	void Dispose();

	/// \brief Parses a sequence of Tokens and generates a parse tree.
	/// \param collection The collection of Tokens to be parsed.
	/// \returns A parse tree of ParseNodes rooted with a Translationunit.
	TranslationUnit *Parse(TokenCollection &collection);

	/// \brief Parses a sequence of Tokens and generates a parse tree.
	/// \param stream The stream of Tokens to be parsed.
	/// \returns A parse tree of ParseNodes rooted with a Translationunit.
	TranslationUnit *Parse(TokenStream &stream);

	/// \brief Returns a linked list of all generated parse trees.
	TranslationUnit *GetTranslationUnitList() { return mTranslationUnitList; }

	/// \brief Returns the buffer where error messages are pushed.
	const CompilerErrorBuffer &GetErrorBuffer() const { return mErrorBuffer; }

private:
	// Copying disallowed.
	Parser(const Parser &other) = delete;
	Parser &operator=(const Parser &other) = delete;

	ParseNodeFactory mFactory;
	CompilerErrorBuffer &mErrorBuffer;
	TranslationUnit *mTranslationUnitList;
};

}

#endif
