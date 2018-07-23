#ifndef BOND_COMPILER_TOKENSTREAM_H
#define BOND_COMPILER_TOKENSTREAM_H

#include "bond/compiler/token.h"

namespace Bond
{

/// \addtogroup compiler
/// @{

/// \brief An iterator over the contents of a TokenCollection.
///
/// TokenStream has methods to return the next available Token, test whether the next Token is of
/// a certain type, peek at the next Token and manipulate the position in the stream.
class TokenStream
{
public:
	/// Constructs an empty TokenStream object.
	TokenStream():
		mBuffer(nullptr),
		mLength(-1),
		mIndex(0)
	{
	}

	/// \brief Constructs a TokenStream object.
	/// \param buffer A pointer to a buffer of Token objects over which the stream iterates..
	/// \param length The number of Tokens contained in the buffer.
	TokenStream(const Token *buffer, int length):
		mBuffer(buffer),
		mLength(length),
		mIndex(0)
	{
	}


	/// \brief Sets the underlying buffer of Token objects and resets the position of the stream.
	/// \param buffer A pointer to a buffer of Token objects that the stream iterates over.
	/// \param length The number of Tokens contained in the buffer.
	void SetBuffer(const Token *buffer, int length);

	/// \brief Resets the position of the stream.
	void Reset() { mIndex = 0; }

	/// \brief Increments the position of the stream by one.
	void Advance() { SetPosition(mIndex + 1); }

	/// \brief Returns the next token in the stream and advances the position.
	const Token *Next();

	/// \brief Returns the next token in the stream if it matches the given type, otherwise returns nullptr.
	/// \param type The type against which the next Token is compared.
	const Token *NextIf(Token::TokenType type);

	/// \brief Returns the next token in the stream if it matches any of the given types, otherwise returns nullptr.
	/// \param typeSet The set of types against which the next Token is compared.
	const Token *NextIf(const TokenTypeSet &typeSet);

	/// \brief Returns the next token in the stream without advancing the position.
	const Token *Peek() const { return mBuffer + mIndex; }

	/// \brief Returns the next token in the stream if it matches the given type, otherwise returns nullptr.
	/// \param type The type against which the next Token is compared.
	const Token *PeekIf(Token::TokenType type) const;

	/// \brief Returns the next token in the stream if it matches any of the given types, otherwise returns nullptr.
	/// \param typeSet The set of types against which the next Token is compared.
	const Token *PeekIf(const TokenTypeSet &typeSet) const;

	/// \brief Advances the position of the stream until a Token of the given type is encountered.
	/// If no such Token is encountered, the stream is advanced to the end.
	/// \param type The type against which the Tokens are compared.
	void SkipTo(Token::TokenType type);

	/// \brief Advances the position of the stream until a Token of one of the given types is
	/// encountered. If no such Token is encountered, the stream is advanced to the end.
	/// \param typeSet The set of types against which the Tokens are compared.
	void SkipTo(const TokenTypeSet &typeSet);

	/// \brief Returns the current position of the stream.
	int GetPosition() const { return mIndex; }

	/// \brief Set the current position of the stream.
	/// \param index The position that the stream is set to.
	void SetPosition(int index) { mIndex = ValidIndex(index); }

	/// \brief Returns the total number of tokens in the stream.
	int GetLength() const { return mLength; }

private:
	int ValidIndex(int index) const { return (index < 0) ? 0 : (index >= mLength) ? (mLength - 1) : index; }

	const Token *mBuffer;
	int mLength;
	int mIndex;
};


/// \brief An ordered immutable collection of Tokens.
///
/// A TokenCollection is the output from a Lexer and can be used as the input to a Parser. Several
/// TokenCollections can be daisy-chained in a linked list.
class TokenCollection
{
public:
	/// \brief Constructs a TokenCollection object.
	/// \param tokens A pointer to an array of Token objects that form the contents of the collection.
	/// \param numTokens The number of Tokens contained in the collection.
	TokenCollection(const Token *tokens, int numTokens):
		mTokens(tokens),
		mNumTokens(numTokens)
	{}

	/// Returns a TokenStream object to iterate over the contents of the collection.
	TokenStream GetTokenStream() const { return TokenStream(mTokens, mNumTokens); }

private:
	const Token *mTokens;
	int mNumTokens;
};

/// @}

}

#endif
