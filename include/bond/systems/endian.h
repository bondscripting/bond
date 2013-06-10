#ifndef BOND_SYSTEMS_ENDIAN_H
#define BOND_SYSTEMS_ENDIAN_H

#include "bond/types/value.h"

namespace Bond
{

inline void SwapBytes(bu8_t &a, bu8_t &b)
{
	const bu8_t temp = a;
	a = b;
	b = temp;
}


inline void ReverseByteOrder16(bu8_t value[2])
{
	SwapBytes(value[0], value[1]);
}


inline void ReverseByteOrder32(bu8_t value[4])
{
	SwapBytes(value[0], value[3]);
	SwapBytes(value[1], value[2]);
}


inline void ReverseByteOrder64(bu8_t value[8])
{
	SwapBytes(value[0], value[7]);
	SwapBytes(value[1], value[6]);
	SwapBytes(value[2], value[5]);
	SwapBytes(value[3], value[4]);
}


inline Value16 ReverseByteOrder16(Value16 value)
{
	ReverseByteOrder16(value.mBytes);
	return value;
}


inline Value32 ReverseByteOrder32(Value32 value)
{
	ReverseByteOrder32(value.mBytes);
	return value;
}


inline Value64 ReverseByteOrder64(Value64 value)
{
	ReverseByteOrder64(value.mBytes);
	return value;
}

#if defined(BOND_BIG_ENDIAN)

inline void ConvertBigEndian16(void *value) {}
inline Value16 ConvertBigEndian16(Value16 value) { return value; }
inline bi16_t ConvertBigEndianI16(bi16_t value) { return value; }
inline bu16_t ConvertBigEndianU16(bu16_t value) { return value; }
inline void ConvertBigEndian32(void *value) {}
inline Value32 ConvertBigEndian32(Value32 value) { return value; }
inline bi32_t ConvertBigEndianI32(bi32_t value) { return value; }
inline bu32_t ConvertBigEndianU32(bu32_t value) { return value; }
inline bf32_t ConvertBigEndianF32(bf32_t value) { return value; }
inline void ConvertBigEndian64(void *value) {}
inline Value64 ConvertBigEndian64(Value64 value) { return value; }
inline bi64_t ConvertBigEndianI64(bi64_t value) { return value; }
inline bu64_t ConvertBigEndianU64(bu64_t value) { return value; }
inline bf64_t ConvertBigEndianF64(bf64_t value) { return value; }

#else

inline void ConvertBigEndian16(void *value)
{
	return ReverseByteOrder16(static_cast<bu8_t *>(value));
}


inline Value16 ConvertBigEndian16(Value16 value)
{
	return ReverseByteOrder16(value);
}


inline bi16_t ConvertBigEndianI16(bi16_t value)
{
	return ReverseByteOrder16(Value16(value)).mShort;
}


inline bu16_t ConvertBigEndianU16(bu16_t value)
{
	return ReverseByteOrder16(Value16(value)).mUShort;
}


inline void ConvertBigEndian32(void *value)
{
	return ReverseByteOrder32(static_cast<bu8_t *>(value));
}


inline Value32 ConvertBigEndian32(Value32 value)
{
	return ReverseByteOrder32(value);
}


inline bi32_t ConvertBigEndianI32(bi32_t value)
{
	return ReverseByteOrder32(Value32(value)).mInt;
}


inline bu32_t ConvertBigEndianU32(bu32_t value)
{
	return ReverseByteOrder32(Value32(value)).mUInt;
}


inline bf32_t ConvertBigEndianF32(bf32_t value)
{
	return ReverseByteOrder32(Value32(value)).mFloat;
}


inline void ConvertBigEndian64(void *value)
{
	return ReverseByteOrder64(static_cast<bu8_t *>(value));
}


inline Value64 ConvertBigEndian64(Value64 value)
{
	return ReverseByteOrder64(value);
}


inline bi64_t ConvertBigEndianI64(bi64_t value)
{
	return ReverseByteOrder64(Value64(value)).mLong;
}


inline bu64_t ConvertBigEndianU64(bu64_t value)
{
	return ReverseByteOrder64(Value64(value)).mULong;
}


inline bf64_t ConvertBigEndianF64(bf64_t value)
{
	return ReverseByteOrder64(Value64(value)).mDouble;
}

#endif

}

#endif
