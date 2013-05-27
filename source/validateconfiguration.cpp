#include "bond/types/value.h"
#include "bond/systems/assert.h"

namespace Bond
{

void ValidateConfiguration()
{
	BOND_STATIC_ASSERT(sizeof(bi8_t) == 1, sizeof_bi8_t_is_1);
	BOND_STATIC_ASSERT(sizeof(bu8_t) == 1, sizeof_bu8_t_is_1);
	BOND_STATIC_ASSERT(sizeof(bi16_t) == 2, sizeof_bi16_t_is_2);
	BOND_STATIC_ASSERT(sizeof(bu16_t) == 2, sizeof_bu16_t_is_2);
	BOND_STATIC_ASSERT(sizeof(bi32_t) == 4, sizeof_bi32_t_is_4);
	BOND_STATIC_ASSERT(sizeof(bu32_t) == 4, sizeof_bu32_t_is_4);
	BOND_STATIC_ASSERT(sizeof(bi64_t) == 8, sizeof_bi64_t_is_8);
	BOND_STATIC_ASSERT(sizeof(bu64_t) == 8, sizeof_bu64_t_is_8);
	BOND_STATIC_ASSERT(sizeof(bf32_t) == 4, sizeof_bf32_t_is_4);
	BOND_STATIC_ASSERT(sizeof(bf64_t) == 8, sizeof_bf64_t_is_8);
	BOND_STATIC_ASSERT(sizeof(Value16) == 2, sizeof_value16_is_2);
	BOND_STATIC_ASSERT(sizeof(Value32) == 4, sizeof_value32_is_4);
	BOND_STATIC_ASSERT(sizeof(Value64) == 8, sizeof_value64_is_8);

#if defined(BOND_USE_32BIT_POINTERS)
	BOND_STATIC_ASSERT(sizeof(void *) == 4, sizeof_pointer_is_4);
	BOND_STATIC_ASSERT(BOND_NATIVE_POINTER_SIZE == POINTER_32BIT, native_pointer_size_is_32bit);
#elif defined(BOND_USE_64BIT_POINTERS)
	BOND_STATIC_ASSERT(sizeof(void *) == 8, sizeof_pointer_is_8);
	BOND_STATIC_ASSERT(BOND_NATIVE_POINTER_SIZE == POINTER_64BIT, native_pointer_size_is_64bit);
#else
#error Neither BOND_USE_32BIT_POINTERS nor BOND_USE_64BIT_POINTERS is defined.
#endif

	// No known way of validating endianness at compile time.
	union EndianTest
	{
		EndianTest(): mInt(1) {}
		char mBytes[sizeof(int)];
		int mInt;
	};
	EndianTest endiantTest;

#if defined(BOND_LITTLE_ENDIAN)
	BOND_ASSERT_MESSAGE(endiantTest.mBytes[0] == 1, "BOND_LITTLE_ENDIAN is defined but system is not little endian.");
#elif defined(BOND_BIG_ENDIAN)
	BOND_ASSERT_MESSAGE(endiantTest.mBytes[0] == 0, "BOND_BIG_ENDIAN is defined but system is not big endian.");
#else
#error Neither BOND_LITTLE_ENDIAN nor BOND_BIG_ENDIAN is defined.
#endif
}

}
