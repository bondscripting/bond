#ifndef BOND_COMPILER_CHARSTREAM_H
#define BOND_COMPILER_CHARSTREAM_H

#include "bond/conf.h"
#include "bond/compiler/streampos.h"
#include "bond/io/inputstream.h"

namespace Bond
{

/// \brief An iterator over the contents of a stream of text.
///
/// CharStream is used internally by Lexer; it is unlikely that someone integrating Bond into
/// their application will have to make direct use of it.
///
/// A single space is appended after the last character of the underlying stream, so that the end
/// of the last token can be identified. The stream position tracks lines and columns as described
/// by StreamPos.
///
/// \ingroup compiler
class BOND_API CharStream
{
public:
	/// \brief Constructs a CharStream object that iterates from the current position to the end
	/// of the given stream.
	/// \param stream The underlying stream of characters over which this stream iterates. It is
	///        not copied, so it must outlive the CharStream.
	explicit CharStream(InputStream &stream):
		mPos(size_t(stream.GetPosition())),
		mStream(stream),
		mStartPos(size_t(stream.GetPosition())),
		mEndPos(size_t(stream.GetEndPosition()))
	{}

	/// \brief Resets the position of the stream back to the beginning.
	void Reset() { Reset(StreamPos(mStartPos)); }

	/// \brief Resets the position of the stream back to the specified position.
	/// \param pos A position previously returned by GetStreamPos().
	void Reset(const StreamPos &pos);

	/// \brief Ensures that the underlying InputStream is seeked to the same position as this stream.
	void Sync() { mStream.SetPosition(Stream::pos_t(mPos.index)); }

	/// \brief Returns whether any characters remain in the stream, including the appended space.
	bool HasNext() const;

	/// \brief Returns the next character in the stream and advances the stream's position.
	///
	/// Returns a space once the end of the underlying stream is reached, without advancing past
	/// the appended space.
	char Next();

	/// \brief Returns the stream's current position.
	const StreamPos &GetStreamPos() const { return mPos; }

private:
	StreamPos mPos;
	InputStream &mStream;
	size_t mStartPos;
	size_t mEndPos;
};

}

#endif
