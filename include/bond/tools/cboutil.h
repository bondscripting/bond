#ifndef BOND_TOOLS_CBOUTIL_H
#define BOND_TOOLS_CBOUTIL_H

#include "bond/stl/stringview.h"
#include "bond/types/types.h"
#include "bond/version.h"

namespace Bond
{

/// \addtogroup tools
/// @{

/// \brief Four-character blob identifier used for list blobs in CBO data.
constexpr StringView BOND_LIST_BLOB_ID {"List"};
/// \brief Four-character blob identifier used for function blobs in CBO data.
constexpr StringView BOND_FUNCTION_BLOB_ID {"Func"};
/// \brief Four-character blob identifier used for data blobs in CBO data.
constexpr StringView BOND_DATA_BLOB_ID {"Data"};
/// \brief Number of bytes in a CBO blob identifier.
constexpr size_t BOND_BLOB_ID_LENGTH = 4;


/// \brief Packs a size and SignatureType into a single encoded integer.
/// \param size The size value to encode.
/// \param type The signature type to encode.
/// \returns A packed integer containing both values.
inline uint32_t EncodeSizeAndType(uint32_t size, SignatureType type)
{
	return (size << 4) | (type & 0xf);
}


/// \brief Unpacks an encoded size-and-type value.
/// \param sizeAndType Packed value previously produced by EncodeSizeAndType().
/// \param size Output parameter receiving the decoded size.
/// \param type Output parameter receiving the decoded signature type.
inline void DecodeSizeAndType(uint32_t sizeAndType, uint32_t &size, SignatureType &type)
{
	size = sizeAndType >> 4;
	type = SignatureType(sizeAndType & 0xf);
}


/// \brief Decodes pointer size information from CBO flags.
/// \param flags CBO header flags containing encoded pointer size information.
/// \returns The decoded pointer size.
inline PointerSize DecodePointerSize(uint16_t flags)
{
	return ((flags & 1) != 0) ? POINTER_64BIT : POINTER_32BIT;
}


/// \brief Encodes pointer size information into CBO flags.
/// \param flags Existing CBO header flags.
/// \param pointerSize Pointer size value to encode.
/// \returns Updated flags containing the encoded pointer size bit.
inline uint16_t EncodePointerSize(uint16_t flags, PointerSize pointerSize)
{
	return flags | ((pointerSize == POINTER_64BIT) ? 1 : 0);
}


/// \brief Determines whether a CBO format version can be loaded.
/// \param majorVersion Major version number of the CBO format.
/// \param minorVersion Minor version number of the CBO format.
/// \returns True if the major version is within the supported loadable range.
inline bool IsCBOFormatLoadable(uint32_t majorVersion, uint32_t minorVersion)
{
	return
		(majorVersion >= CBO_MIN_SUPPORTED_MAJOR_VERSION) &&
		(majorVersion <= CBO_MAX_SUPPORTED_MAJOR_VERSION);
}


/// \brief Determines whether a CBO format version is fully supported.
/// \param majorVersion Major version number of the CBO format.
/// \param minorVersion Minor version number of the CBO format.
/// \returns True if the major/minor version pair is within the supported range.
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

/// @}

}

#endif
