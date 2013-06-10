#ifndef BOND_TYPES_TYPEUTIL_H
#define BOND_TYPES_TYPEUTIL_H

#include "bond/systems/math.h"

namespace Bond
{

template<typename T>
bool ValidateSignatureType(size_t size, SignatureType signatureType)
{
	return
		(AlignUp(sizeof(T), size_t(BOND_SLOT_SIZE)) == size) &&
		((signatureType == SIG_POINTER) || (signatureType == SIG_STRUCT));
}


template<>
inline bool ValidateSignatureType<bool>(size_t size, SignatureType signatureType)
{
	return
		(signatureType == SIG_BOOL) ||
		(signatureType == SIG_CHAR) ||
		(signatureType == SIG_UCHAR);
}


template<>
inline bool ValidateSignatureType<bi8_t>(size_t size, SignatureType signatureType)
{
	return
		(signatureType == SIG_BOOL) ||
		(signatureType == SIG_CHAR) ||
		(signatureType == SIG_UCHAR);
}


template<>
inline bool ValidateSignatureType<bu8_t>(size_t size, SignatureType signatureType)
{
	return
		(signatureType == SIG_BOOL) ||
		(signatureType == SIG_CHAR) ||
		(signatureType == SIG_UCHAR);
}


template<>
inline bool ValidateSignatureType<bi16_t>(size_t size, SignatureType signatureType)
{
	return
		(signatureType == SIG_SHORT) ||
		(signatureType == SIG_USHORT);
}


template<>
inline bool ValidateSignatureType<bu16_t>(size_t size, SignatureType signatureType)
{
	return
		(signatureType == SIG_SHORT) ||
		(signatureType == SIG_USHORT);
}


template<>
inline bool ValidateSignatureType<bi32_t>(size_t size, SignatureType signatureType)
{
	return
		(signatureType == SIG_INT) ||
		(signatureType == SIG_UINT);
}


template<>
inline bool ValidateSignatureType<bu32_t>(size_t size, SignatureType signatureType)
{
	return
		(signatureType == SIG_INT) ||
		(signatureType == SIG_UINT);
}


template<>
inline bool ValidateSignatureType<bi64_t>(size_t size, SignatureType signatureType)
{
	return
		(signatureType == SIG_LONG) ||
		(signatureType == SIG_ULONG);
}


template<>
inline bool ValidateSignatureType<bu64_t>(size_t size, SignatureType signatureType)
{
	return
		(signatureType == SIG_LONG) ||
		(signatureType == SIG_ULONG);
}


template<>
inline bool ValidateSignatureType<bf32_t>(size_t size, SignatureType signatureType)
{
	return signatureType == SIG_FLOAT;
}


template<>
inline bool ValidateSignatureType<bf64_t>(size_t size, SignatureType signatureType)
{
	return signatureType == SIG_DOUBLE;
}

}

#endif
