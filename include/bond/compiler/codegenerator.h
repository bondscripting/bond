#ifndef BOND_COMPILER_CODEGENERATOR_H
#define BOND_COMPILER_CODEGENERATOR_H

#include "bond/types/types.h"

namespace Bond
{

class Allocator;
class OutputStream;
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

	void Generate(const TranslationUnit *translationUnitList, OutputStream &stream);

private:
	Allocator &mAllocator;
	CompilerErrorBuffer &mErrorBuffer;
	PointerSize mPointerSize;
};

}

#endif
