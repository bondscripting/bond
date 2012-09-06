#include "framework/testvmframework.h"

DEFINE_VM_TEST(BinaryOperators, "scripts/vm_BinaryOperators.bond")
{
	VALIDATE_INT_CALL("::IntAdd", 65, 13, 65 + 13);
	VALIDATE_INT_CALL("::IntSub", 43, 12, 43 - 12);

	return true;
}


#define TEST_ITEMS                              \
  TEST_ITEM(BinaryOperators)                    \

RUN_TESTS(VM, TEST_ITEMS)
