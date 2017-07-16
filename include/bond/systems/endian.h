#ifndef BOND_SYSTEMS_ENDIAN_H
#define BOND_SYSTEMS_ENDIAN_H

#include "bond/types/value.h"

namespace Bond
{

/// \addtogroup endian
/// @{

/// \brief Swaps the value of two bytes.
inline void SwapBytes(uint8_t &a, uint8_t &b)
{
	const uint8_t temp = a;
	a = b;
	b = temp;
}


/// \brief Reverses the order of two bytes in place.
inline void ReverseByteOrder16(uint8_t value[2])
{
	SwapBytes(value[0], value[1]);
}


/// \brief Reverses the order of four bytes in place.
inline void ReverseByteOrder32(uint8_t value[4])
{
	SwapBytes(value[0], value[3]);
	SwapBytes(value[1], value[2]);
}


/// \brief Reverses the order of eight bytes in place.
inline void ReverseByteOrder64(uint8_t value[8])
{
	SwapBytes(value[0], value[7]);
	SwapBytes(value[1], value[6]);
	SwapBytes(value[2], value[5]);
	SwapBytes(value[3], value[4]);
}


/// \brief Returns a 16 bit value with its byte order reversed.
inline Value16 ReverseByteOrder16(Value16 value)
{
	ReverseByteOrder16(value.mBytes);
	return value;
}


/// \brief Returns a 32 bit value with its byte order reversed.
inline Value32 ReverseByteOrder32(Value32 value)
{
	ReverseByteOrder32(value.mBytes);
	return value;
}


/// \brief Returns a 64 bit value with its byte order reversed.
inline Value64 ReverseByteOrder64(Value64 value)
{
	ReverseByteOrder64(value.mBytes);
	return value;
}

#if defined(BOND_BIG_ENDIAN)

inline void ConvertBigEndian16(void *value) {}
inline Value16 ConvertBigEndian16(Value16 value) { return value; }
inline int16_t ConvertBigEndianI16(int16_t value) { return value; }
inline uint16_t ConvertBigEndianU16(uint16_t value) { return value; }
inline void ConvertBigEndian32(void *value) {}
inline Value32 ConvertBigEndian32(Value32 value) { return value; }
inline int32_t ConvertBigEndianI32(int32_t value) { return value; }
inline uint32_t ConvertBigEndianU32(uint32_t value) { return value; }
inline float ConvertBigEndianF32(float value) { return value; }
inline void ConvertBigEndian64(void *value) {}
inline Value64 ConvertBigEndian64(Value64 value) { return value; }
inline int64_t ConvertBigEndianI64(int64_t value) { return value; }
inline uint64_t ConvertBigEndianU64(uint64_t value) { return value; }
inline double ConvertBigEndianF64(double value) { return value; }

#else

/// \brief Converts the byte order of a 16 bit value from native byte order to big endian or vice versa
/// in place.
inline void ConvertBigEndian16(void *value)
{
	ReverseByteOrder16(static_cast<uint8_t *>(value));
}


/// \brief Converts the byte order of a 16 bit value from native byte order to big endian or vice versa
/// and returns the new value.
inline Value16 ConvertBigEndian16(Value16 value)
{
	return ReverseByteOrder16(value);
}


/// \brief Converts the byte order of an int16_t from native byte order to big endian or vice versa
/// and returns the new value.
inline int16_t ConvertBigEndianI16(int16_t value)
{
	return ReverseByteOrder16(Value16(value)).mShort;
}


/// \brief Converts the byte order of an uint16_t from native byte order to big endian or vice versa
/// and returns the new value.
inline uint16_t ConvertBigEndianU16(uint16_t value)
{
	return ReverseByteOrder16(Value16(value)).mUShort;
}


/// \brief Converts the byte order of a 32 bit value from native byte order to big endian or vice versa
/// in place.
inline void ConvertBigEndian32(void *value)
{
	ReverseByteOrder32(static_cast<uint8_t *>(value));
}


/// \brief Converts the byte order of a 32 bit value from native byte order to big endian or vice versa
/// and returns the new value.
inline Value32 ConvertBigEndian32(Value32 value)
{
	return ReverseByteOrder32(value);
}


/// \brief Converts the byte order of an int32_t from native byte order to big endian or vice versa
/// and returns the new value.
inline int32_t ConvertBigEndianI32(int32_t value)
{
	return ReverseByteOrder32(Value32(value)).mInt;
}


/// \brief Converts the byte order of an uint32_t from native byte order to big endian or vice versa
/// and returns the new value.
inline uint32_t ConvertBigEndianU32(uint32_t value)
{
	return ReverseByteOrder32(Value32(value)).mUInt;
}


/// \brief Converts the byte order of a float from native byte order to big endian or vice versa
/// and returns the new value.
inline float ConvertBigEndianF32(float value)
{
	return ReverseByteOrder32(Value32(value)).mFloat;
}


/// \brief Converts the byte order of a 64 bit value from native byte order to big endian or vice versa
/// and returns the new value.
inline void ConvertBigEndian64(void *value)
{
	ReverseByteOrder64(static_cast<uint8_t *>(value));
}


/// \brief Converts the byte order of a 64 bit value from native byte order to big endian or vice versa
/// and returns the new value.
inline Value64 ConvertBigEndian64(Value64 value)
{
	return ReverseByteOrder64(value);
}


/// \brief Converts the byte order of an int64_t from native byte order to big endian or vice versa
/// and returns the new value.
inline int64_t ConvertBigEndianI64(int64_t value)
{
	return ReverseByteOrder64(Value64(value)).mLong;
}


/// \brief Converts the byte order of an uint64_t from native byte order to big endian or vice versa
/// and returns the new value.
inline uint64_t ConvertBigEndianU64(uint64_t value)
{
	return ReverseByteOrder64(Value64(value)).mULong;
}


/// \brief Converts the byte order of a double from native byte order to big endian or vice versa
/// and returns the new value.
inline double ConvertBigEndianF64(double value)
{
	return ReverseByteOrder64(Value64(value)).mDouble;
}

#endif

/// @}

}

#endif
