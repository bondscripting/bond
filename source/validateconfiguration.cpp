#include "bond/types/value.h"
#include "bond/systems/assert.h"

namespace Bond
{

void ValidateConfiguration()
{
	static_assert(sizeof(bi8_t) == 1, "sizeof(bi8_t) is not 1");
	static_assert(sizeof(bu8_t) == 1, "sizeof(bu8_t) is not 1");
	static_assert(sizeof(bi16_t) == 2, "sizeof(bi16_t) is not 2");
	static_assert(sizeof(bu16_t) == 2, "sizeof(bu16_t) is not 2");
	static_assert(sizeof(bi32_t) == 4, "sizeof(bi32_t) is not 4");
	static_assert(sizeof(bu32_t) == 4, "sizeof(bu32_t) is not 4");
	static_assert(sizeof(bi64_t) == 8, "sizeof(bi64_t) is not 8");
	static_assert(sizeof(bu64_t) == 8, "sizeof(bu64_t) is not 8");
	static_assert(sizeof(float) == 4, "sizeof(float) is not 4");
	static_assert(sizeof(double) == 8, "sizeof(double) is not 8");
	static_assert(sizeof(Value16) == 2, "sizeof(Value16) is not 2");
	static_assert(sizeof(Value32) == 4, "sizeof(Value32) is not 4");
	static_assert(sizeof(Value64) == 8, "sizeof(Value64) is not 8");

#if defined(BOND_USE_32BIT_POINTERS)
	static_assert(sizeof(void *) == 4, "sizeof(void *) is not 4");
	static_assert(BOND_NATIVE_POINTER_SIZE == POINTER_32BIT, "BOND_NATIVE_POINTER_SIZE is not defined as POINTER_32BIT even though BOND_USE_32BIT_POINTERS is defined");
#elif defined(BOND_USE_64BIT_POINTERS)
	static_assert(sizeof(void *) == 8, "sizeof(void *) is not 8");
	static_assert(BOND_NATIVE_POINTER_SIZE == POINTER_64BIT, "BOND_NATIVE_POINTER_SIZE is not defined as POINTER_64BIT even though BOND_USE_64BIT_POINTERS is defined");
#else
#error Neither BOND_USE_32BIT_POINTERS nor BOND_USE_64BIT_POINTERS is defined.
#endif

	union EndianTest
	{
		EndianTest(): mInt(1) {}
		char mBytes[sizeof(int)];
		int mInt;
	};
	const EndianTest endiantTest;

#if defined(BOND_LITTLE_ENDIAN)
	BOND_ASSERT_MESSAGE(endiantTest.mBytes[0] == 1, "BOND_LITTLE_ENDIAN is defined but system is not little endian.");
#elif defined(BOND_BIG_ENDIAN)
	BOND_ASSERT_MESSAGE(endiantTest.mBytes[0] == 0, "BOND_BIG_ENDIAN is defined but system is not big endian.");
#else
#error Neither BOND_LITTLE_ENDIAN nor BOND_BIG_ENDIAN is defined.
#endif

	const Value32 floatFormatTest(-2.375f);
	BOND_ASSERT_MESSAGE(floatFormatTest.mUInt == bu32_t(0xC0180000), "The representation of type 'float' does not appear to be in IEEE binary32 format.");

	const Value64 doubleFormatTest(-2.375);
	BOND_ASSERT_MESSAGE(doubleFormatTest.mULong == (bu64_t(0xC0030000) << bu64_t(32)), "The representation of type 'double' does not appear to be in IEEE binary64 format.");
}

}
