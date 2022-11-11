#ifndef BOND_TOOLS_CBOUTIL_H
#define BOND_TOOLS_CBOUTIL_H

#include "bond/stl/stringview.h"
#include "bond/types/types.h"
#include "bond/version.h"

namespace Bond
{

constexpr StringView BOND_LIST_BLOB_ID {"List"};
constexpr StringView BOND_FUNCTION_BLOB_ID {"Func"};
constexpr StringView BOND_DATA_BLOB_ID {"Data"};
constexpr size_t BOND_BLOB_ID_LENGTH = 4;


inline uint32_t EncodeSizeAndType(uint32_t size, SignatureType type)
{
	return (size << 4) | (type & 0xf);
}


inline void DecodeSizeAndType(uint32_t sizeAndType, uint32_t &size, SignatureType &type)
{
	size = sizeAndType >> 4;
	type = SignatureType(sizeAndType & 0xf);
}


inline PointerSize DecodePointerSize(uint16_t flags)
{
	return ((flags & 1) != 0) ? POINTER_64BIT : POINTER_32BIT;
}


inline uint16_t EncodePointerSize(uint16_t flags, PointerSize pointerSize)
{
	return flags | ((pointerSize == POINTER_64BIT) ? 1 : 0);
}


inline bool IsCBOFormatLoadable(uint32_t majorVersion, uint32_t minorVersion)
{
	return
		(majorVersion >= CBO_MIN_SUPPORTED_MAJOR_VERSION) &&
		(majorVersion <= CBO_MAX_SUPPORTED_MAJOR_VERSION);
}


inline bool IsCBOFormatSupported(uint32_t majorVersion, uint32_t minorVersion)
{
	return
		((majorVersion > CBO_MIN_SUPPORTED_MAJOR_VERSION) ||
		 ((majorVersion == CBO_MIN_SUPPORTED_MAJOR_VERSION) &&
			(minorVersion >= CBO_MIN_SUPPORTED_MINOR_VERSION))) &&
		((majorVersion < CBO_MAX_SUPPORTED_MAJOR_VERSION) ||
		 ((majorVersion == CBO_MAX_SUPPORTED_MAJOR_VERSION) &&
			(minorVersion <= CBO_MAX_SUPPORTED_MINOR_VERSION)));
}

}

#endif
