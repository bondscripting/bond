#ifndef BOND_TYPEUTIL_H
#define BOND_TYPEUTIL_H

#include "bond/math.h"

namespace Bond
{

template<typename T>
bool ValidateArgType(size_t size, SignatureType signatureType)
{
	return
		(AlignUp(sizeof(T), size_t(BOND_SLOT_SIZE)) == size) &&
		((signatureType == SIG_POINTER) || (signatureType == SIG_STRUCT));
}


template<>
inline bool ValidateArgType<bool>(size_t size, SignatureType signatureType)
{
	return
		(signatureType == SIG_BOOL) ||
		(signatureType == SIG_CHAR) ||
		(signatureType == SIG_UCHAR);
}


template<>
inline bool ValidateArgType<bi8_t>(size_t size, SignatureType signatureType)
{
	return
		(signatureType == SIG_BOOL) ||
		(signatureType == SIG_CHAR) ||
		(signatureType == SIG_UCHAR);
}


template<>
inline bool ValidateArgType<bu8_t>(size_t size, SignatureType signatureType)
{
	return
		(signatureType == SIG_BOOL) ||
		(signatureType == SIG_CHAR) ||
		(signatureType == SIG_UCHAR);
}


template<>
inline bool ValidateArgType<bi16_t>(size_t size, SignatureType signatureType)
{
	return
		(signatureType == SIG_SHORT) ||
		(signatureType == SIG_USHORT);
}


template<>
inline bool ValidateArgType<bu16_t>(size_t size, SignatureType signatureType)
{
	return
		(signatureType == SIG_SHORT) ||
		(signatureType == SIG_USHORT);
}


template<>
inline bool ValidateArgType<bi32_t>(size_t size, SignatureType signatureType)
{
	return
		(signatureType == SIG_INT) ||
		(signatureType == SIG_UINT);
}


template<>
inline bool ValidateArgType<bu32_t>(size_t size, SignatureType signatureType)
{
	return
		(signatureType == SIG_INT) ||
		(signatureType == SIG_UINT);
}


template<>
inline bool ValidateArgType<bi64_t>(size_t size, SignatureType signatureType)
{
	return
		(signatureType == SIG_LONG) ||
		(signatureType == SIG_ULONG);
}


template<>
inline bool ValidateArgType<bu64_t>(size_t size, SignatureType signatureType)
{
	return
		(signatureType == SIG_LONG) ||
		(signatureType == SIG_ULONG);
}


template<>
inline bool ValidateArgType<bf32_t>(size_t size, SignatureType signatureType)
{
	return signatureType == SIG_FLOAT;
}


template<>
inline bool ValidateArgType<bf64_t>(size_t size, SignatureType signatureType)
{
	return signatureType == SIG_DOUBLE;
}

}

#endif
