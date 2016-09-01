#ifndef BOND_COMPILER_CHARSTREAM_H
#define BOND_COMPILER_CHARSTREAM_H

#include "bond/compiler/streampos.h"
#include "bond/io/inputstream.h"

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
	/// \brief Constructs a CharStream object
	/// \param buffer A pointer to a buffer of characters over which the stream iterates.
	/// \param length The number of characters contained in the buffer.
	CharStream(InputStream &stream):
		mStream(stream),
		mStartPos(size_t(stream.GetPosition())),
		mEndPos(size_t(stream.GetEndPosition()))
	{}

	/// \brief Resets the position of the stream back to the beginning.
	void Reset() { Reset(StreamPos(mStartPos)); }

	/// \brief Resets the position of the stream back to the specified position.
	void Reset(const StreamPos &pos);

	/// \brief Ensures that the underlying InputStream is seeked to the same position as this stream.
	void Sync() { mStream.SetPosition(Stream::pos_t(mPos.index)); }

	/// \brief Returns whether any characters remain in the stream.
	/// \returns false when the end of the stream is reached and true otherwise.
	bool HasNext() const;

	/// \brief Returns the next character in the stream and advances the stream's position.
	char Next();

	/// \brief Returns the streams current position.
	const StreamPos &GetStreamPos() const { return mPos; }

private:
	StreamPos mPos;
	InputStream &mStream;
	size_t mStartPos;
	size_t mEndPos;
};

}

#endif
