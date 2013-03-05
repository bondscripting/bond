#ifndef BOND_CODEGENERATOR_H
#define BOND_CODEGENERATOR_H

#include "bond/types/types.h"

namespace Bond
{

class Allocator;
class BinaryWriter;
class CompilerErrorBuffer;
class TranslationUnit;

class CodeGenerator
{
public:
	CodeGenerator(Allocator &allocator, CompilerErrorBuffer &errorBuffer, PointerSize pointerSize = BOND_NATIVE_POINTER_SIZE):
		mAllocator(allocator),
		mErrorBuffer(errorBuffer),
		mPointerSize(pointerSize)
	{}
	~CodeGenerator() {}

	void Generate(const TranslationUnit *translationUnitList, BinaryWriter &writer);

private:
	Allocator &mAllocator;
	CompilerErrorBuffer &mErrorBuffer;
	PointerSize mPointerSize;
};

}

#endif
