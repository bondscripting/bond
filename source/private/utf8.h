#ifndef BOND_PRIVATE_UTF8_H
#define BOND_PRIVATE_UTF8_H

#include "bond/types/types.h"

namespace Bond
{
namespace Private
{

// Returns whether c is a UTF-8 continuation byte, i.e. of the form 10xxxxxx.
inline bool IsUtf8ContinuationByte(char c)
{
	return (uint8_t(c) & 0xC0) == 0x80;
}


// Validates and decodes a UTF-8 sequence one byte at a time. The rules follow table 3-7 of the Unicode
// standard: by restricting the range of the second byte, overlong encodings, UTF-16 surrogates and
// values beyond U+10FFFF are all rejected.
class Utf8Validator
{
public:
	enum Result
	{
		NEED_MORE,
		COMPLETE,
		INVALID
	};

	Utf8Validator(): mCodePoint(0), mRemaining(0), mLow(0x80), mHigh(0xBF) {}

	// Starts a new sequence with its first byte.
	Result Start(char c)
	{
		const uint8_t lead = uint8_t(c);
		mLow = 0x80;
		mHigh = 0xBF;

		if (lead < 0x80)
		{
			mCodePoint = lead;
			mRemaining = 0;
		}
		else if ((lead >= 0xC2) && (lead <= 0xDF))
		{
			mCodePoint = lead & 0x1Fu;
			mRemaining = 1;
		}
		else if ((lead >= 0xE0) && (lead <= 0xEF))
		{
			mCodePoint = lead & 0x0Fu;
			mRemaining = 2;
			mLow = (lead == 0xE0) ? 0xA0 : 0x80;
			mHigh = (lead == 0xED) ? 0x9F : 0xBF;
		}
		else if ((lead >= 0xF0) && (lead <= 0xF4))
		{
			mCodePoint = lead & 0x07u;
			mRemaining = 3;
			mLow = (lead == 0xF0) ? 0x90 : 0x80;
			mHigh = (lead == 0xF4) ? 0x8F : 0xBF;
		}
		else
		{
			// A continuation byte, C0, C1 or F5 to FF.
			mRemaining = 0;
			return INVALID;
		}

		return (mRemaining == 0) ? COMPLETE : NEED_MORE;
	}

	// Feeds the next byte of a sequence. When the result is INVALID, the byte is not part of the
	// sequence and should be processed again on its own.
	Result Feed(char c)
	{
		const uint8_t b = uint8_t(c);
		if ((mRemaining == 0) || (b < mLow) || (b > mHigh))
		{
			mRemaining = 0;
			return INVALID;
		}

		mCodePoint = (mCodePoint << 6) | (b & 0x3Fu);
		mLow = 0x80;
		mHigh = 0xBF;
		--mRemaining;
		return (mRemaining == 0) ? COMPLETE : NEED_MORE;
	}

	// Returns the decoded code point once the result is COMPLETE.
	uint32_t GetCodePoint() const { return mCodePoint; }

private:
	uint32_t mCodePoint;
	uint32_t mRemaining;
	uint8_t mLow;
	uint8_t mHigh;
};


struct Utf8Decoded
{
	uint32_t codePoint;
	size_t length;
	bool valid;
};


// Decodes the UTF-8 sequence at the start of str, reading at most available bytes, which must be
// at least 1. If the sequence is invalid, the code point is U+FFFD and the length is that of the
// maximal invalid subpart, which is at least 1, as recommended by the Unicode standard.
inline Utf8Decoded DecodeUtf8(const char *str, size_t available)
{
	const uint32_t REPLACEMENT_CHARACTER = 0xFFFD;
	Utf8Validator validator;
	Utf8Validator::Result result = validator.Start(str[0]);
	size_t length = 1;

	while (result == Utf8Validator::NEED_MORE)
	{
		if (length >= available)
		{
			result = Utf8Validator::INVALID;
			break;
		}

		result = validator.Feed(str[length]);
		if (result != Utf8Validator::INVALID)
		{
			++length;
		}
	}

	Utf8Decoded decoded;
	decoded.valid = result == Utf8Validator::COMPLETE;
	decoded.codePoint = decoded.valid ? validator.GetCodePoint() : REPLACEMENT_CHARACTER;
	decoded.length = length;
	return decoded;
}


// Returns the number of bytes needed to encode the code point in UTF-8, or 0 if it is a UTF-16
// surrogate or is beyond U+10FFFF.
inline size_t GetUtf8EncodedLength(uint32_t codePoint)
{
	if (codePoint < 0x80)
	{
		return 1;
	}
	if (codePoint < 0x800)
	{
		return 2;
	}
	if (codePoint < 0x10000)
	{
		return ((codePoint >= 0xD800) && (codePoint <= 0xDFFF)) ? 0 : 3;
	}
	return (codePoint <= 0x10FFFF) ? 4 : 0;
}


// Encodes the code point in UTF-8, writing between 1 and 4 bytes without a terminator. Returns the
// number of bytes written, or 0 if the code point is a UTF-16 surrogate or is beyond U+10FFFF.
inline size_t EncodeUtf8(uint32_t codePoint, char *buffer)
{
	const size_t length = GetUtf8EncodedLength(codePoint);
	switch (length)
	{
		case 1:
			buffer[0] = char(codePoint);
			break;

		case 2:
			buffer[0] = char(0xC0 | (codePoint >> 6));
			buffer[1] = char(0x80 | (codePoint & 0x3F));
			break;

		case 3:
			buffer[0] = char(0xE0 | (codePoint >> 12));
			buffer[1] = char(0x80 | ((codePoint >> 6) & 0x3F));
			buffer[2] = char(0x80 | (codePoint & 0x3F));
			break;

		case 4:
			buffer[0] = char(0xF0 | (codePoint >> 18));
			buffer[1] = char(0x80 | ((codePoint >> 12) & 0x3F));
			buffer[2] = char(0x80 | ((codePoint >> 6) & 0x3F));
			buffer[3] = char(0x80 | (codePoint & 0x3F));
			break;

		default:
			break;
	}
	return length;
}

}
}

#endif
