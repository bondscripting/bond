#ifndef BOND_COMPILER_CHARSTREAM_H
#define BOND_COMPILER_CHARSTREAM_H

#include "bond/compiler/streampos.h"

namespace Bond
{

/// \brief An iterator over the contents of a string of text.
///
/// CharStream has methods to return the next available character, peek at the next character,
/// push characters back into the stream and to query the position of the stream.
///
/// CharStream is used internally by Lexer; it is unlikely that someone integrating Bond into
/// their application will have to make direct use of it.
/// \ingroup compiler
class CharStream
{
public:
	/// \brief Default constructs a CharStream object.
	CharStream():
		mBuffer(0),
		mLength(0)
	{
	}

	/// \brief Constructs a CharStream object
	/// \param buffer A pointer to a buffer of characters over which the stream iterates.
	/// \param length The number of characters contained in the buffer.
	CharStream(const char *buffer, size_t length):
		mBuffer(buffer),
		mLength(length)
	{
	}

	/// \brief Resets the position of the stream back to the begining.
	void Reset();

	/// \brief Returns whether any characters remain in the stream.
	/// \returns false when the end of the stream is reached and true otherwise.
	bool HasNext() const;

	/// \brief Returns the next character in the stream and advances the stream's position.
	char Next();

	/// \brief Returns the next character in the stream without advancing the stream's position.
	char Peek() const { return Peek(mPos.index); }

	/// \brief Rewinds the stream's position by one character.
	void Unget() { Unget(1); }

	/// \brief Rewinds the stream's position by the specified number of characters.
	/// \param numChars The number of characters by which the stream's position is rewound.
	void Unget(size_t numChars);

	/// \brief Returns the streams current position.
	const StreamPos &GetStreamPos() const { return mPos; }

	/// \brief Returns the buffer of characters over which the stream iterates.
	const char *GetBuffer() const { return mBuffer; }

	/// \brief Returns the total number of characters in the stream.
	size_t GetLength() const { return mLength; }

private:
	char Peek(size_t index) const;

	StreamPos mPos;
	const char *mBuffer;
	size_t mLength;
};

}

#endif
