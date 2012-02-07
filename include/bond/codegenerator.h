#ifndef BOND_CODEGENERATOR_H
#define BOND_CODEGENERATOR_H

#include "bond/types.h"

namespace Bond
{

class Allocator;
class BinaryWriter;
class TranslationUnit;

class CodeGenerator
{
public:
	CodeGenerator(Allocator &allocator, bu32_t pointerSize = BOND_NATIVE_POINTER_SIZE):
		mAllocator(allocator),
		mPointerSize(pointerSize)
	{}
	~CodeGenerator() {}

	void Generate(const TranslationUnit *translationUnitList, BinaryWriter &writer);

private:
	Allocator &mAllocator;
	bu32_t mPointerSize;
};

}

#endif
