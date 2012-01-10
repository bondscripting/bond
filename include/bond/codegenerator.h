#ifndef BOND_CODEGENERATOR_H
#define BOND_CODEGENERATOR_H

#include "bond/allocator.h"
#include "bond/types.h"

namespace Bond
{

class TranslationUnit;

class CodeGenerator
{
public:
	CodeGenerator(Allocator &allocator, bu32_t pointerSize = BOND_NATIVE_POINTER_SIZE):
		mAllocator(allocator),
		mPointerSize(pointerSize)
	{}
	~CodeGenerator() {}

	void Generate(const TranslationUnit *translationUnitList);

private:
	Allocator &mAllocator;
	bu32_t mPointerSize;
};

}

#endif
