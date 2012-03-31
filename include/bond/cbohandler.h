#ifndef BOND_CBOHANDLER_H
#define BOND_CBOHANDLER_H

#include "bond/value.h"

namespace Bond
{

class HashedString;

class QualifiedId
{
public:
	QualifiedId():
		mNumElements(0),
		mElementIndices(NULL)
	{}

	QualifiedId(int numElements, const bu16_t *elementIndices):
		mNumElements(numElements),
		mElementIndices(elementIndices)
	{}

	int GetNumElements() const { return mNumElements; }
	int GetElementIndex(int element) const { return static_cast<int>(mElementIndices[element]); }

private:
	int mNumElements;
	const bu16_t *mElementIndices;
};

class CboHandler
{
public:
	virtual ~CboHandler();

	virtual void ReceiveVersionInfo(int majorVersion, int minorVersion) = 0;

	virtual void ReceivePointerSize(PointerSize size) = 0;

	virtual void ReceiveValue32Table(size_t numValues, const Value32 *values) = 0;
	virtual const Value32 &GetValue32(size_t index) const = 0;
	virtual const size_t &GetValue32TableSize() const = 0;

	virtual void ReceiveValue64Table(size_t numValues, const Value64 *values) = 0;
	virtual const Value64 &GetValue64(size_t index) const = 0;
	virtual const size_t &GetValue64TableSize() const = 0;

	virtual void ReceiveStringTableSize(size_t numStrings) = 0;
	virtual void ReceiveString(const HashedString &str, size_t index) = 0;
	virtual const HashedString &GetString(size_t index) const = 0;
	virtual const size_t &GetStringTableSize() const = 0;

	virtual void BeginList(size_t numElements) = 0;
	virtual void EndList() = 0;

	virtual void ReceiveFunctionBlob(const QualifiedId &name, bu32_t hash, size_t codeSize, unsigned char *byteCode) = 0;
	virtual void EndFunctionBlob() = 0;

	virtual void ReceiveBlob(unsigned char *blob, size_t blobSize) = 0;

	virtual void CboIsCorrupt() = 0;
	virtual void InvalidMagicNumber(bu32_t number) = 0;
	virtual void InvalidByteCode() = 0;
};

}

#endif
