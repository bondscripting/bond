#include "framework/testvmframework.h"

DEFINE_VM_TEST(Constants, "scripts/vm_Constants.bond")
{
	using namespace Bond;
	VALIDATE_FUNCTION_CALL_0(INT, "::Constc", bi32_t(-7));
	VALIDATE_FUNCTION_CALL_0(UINT, "::Constuc", bi32_t(130));
	VALIDATE_FUNCTION_CALL_0(INT, "::Consts", bi32_t(-345));
	VALIDATE_FUNCTION_CALL_0(UINT, "::Constus", bi32_t(40000));
	VALIDATE_FUNCTION_CALL_0(INT, "::Const32", bi32_t(-34567));
	VALIDATE_FUNCTION_CALL_0(LONG, "::Const64", bi64_t(-34567));
	VALIDATE_FUNCTION_CALL_0(INT, "::Consti_n2", bi32_t(-2));
	VALIDATE_FUNCTION_CALL_0(INT, "::Consti_n1", bi32_t(-1));
	VALIDATE_FUNCTION_CALL_0(INT, "::Consti_0", bi32_t(0));
	VALIDATE_FUNCTION_CALL_0(INT, "::Consti_1", bi32_t(1));
	VALIDATE_FUNCTION_CALL_0(INT, "::Consti_2", bi32_t(2));
	VALIDATE_FUNCTION_CALL_0(INT, "::Consti_3", bi32_t(3));
	VALIDATE_FUNCTION_CALL_0(INT, "::Consti_4", bi32_t(4));
	VALIDATE_FUNCTION_CALL_0(INT, "::Consti_8", bi32_t(8));
	VALIDATE_FUNCTION_CALL_0(LONG, "::Constl_n2", bi64_t(-2));
	VALIDATE_FUNCTION_CALL_0(LONG, "::Constl_n1", bi64_t(-1));
	VALIDATE_FUNCTION_CALL_0(LONG, "::Constl_0", bi64_t(0));
	VALIDATE_FUNCTION_CALL_0(LONG, "::Constl_1", bi64_t(1));
	VALIDATE_FUNCTION_CALL_0(LONG, "::Constl_2", bi64_t(2));
	VALIDATE_FUNCTION_CALL_0(LONG, "::Constl_3", bi64_t(3));
	VALIDATE_FUNCTION_CALL_0(LONG, "::Constl_4", bi64_t(4));
	VALIDATE_FUNCTION_CALL_0(LONG, "::Constl_8", bi64_t(8));
	VALIDATE_FUNCTION_CALL_0(FLOAT, "::Constf_n2", bf32_t(-2.0f));
	VALIDATE_FUNCTION_CALL_0(FLOAT, "::Constf_n1", bf32_t(-1.0f));
	VALIDATE_FUNCTION_CALL_0(FLOAT, "::Constf_nh", bf32_t(-0.5f));
	VALIDATE_FUNCTION_CALL_0(FLOAT, "::Constf_0", bf32_t(0.0f));
	VALIDATE_FUNCTION_CALL_0(FLOAT, "::Constf_h", bf32_t( 0.5f));
	VALIDATE_FUNCTION_CALL_0(FLOAT, "::Constf_1", bf32_t(1.0f));
	VALIDATE_FUNCTION_CALL_0(FLOAT, "::Constf_2", bf32_t(2.0f));
	VALIDATE_FUNCTION_CALL_0(DOUBLE, "::Constd_n2", bf64_t(-2.0));
	VALIDATE_FUNCTION_CALL_0(DOUBLE, "::Constd_n1", bf64_t(-1.0));
	VALIDATE_FUNCTION_CALL_0(DOUBLE, "::Constd_nh", bf64_t(-0.5));
	VALIDATE_FUNCTION_CALL_0(DOUBLE, "::Constd_0", bf64_t(0.0));
	VALIDATE_FUNCTION_CALL_0(DOUBLE, "::Constd_h", bf64_t( 0.5));
	VALIDATE_FUNCTION_CALL_0(DOUBLE, "::Constd_1", bf64_t(1.0));
	VALIDATE_FUNCTION_CALL_0(DOUBLE, "::Constd_2", bf64_t(2.0));

	return true;
}


DEFINE_VM_TEST(StackOperations, "scripts/vm_StackOperations.bond")
{
	using namespace Bond;
	VALIDATE_FUNCTION_CALL_5(INT, "::PushcPopc", bi8_t(-123), bi8_t(0), bi8_t(0), bi8_t(0), bi8_t(0), bi8_t(-123));
	VALIDATE_FUNCTION_CALL_5(INT, "::PushsPops", bi16_t(-1234), bi16_t(0), bi16_t(0), bi16_t(0), bi16_t(0), bi16_t(-1234));
	VALIDATE_FUNCTION_CALL_5(INT, "::Push32Pop32", bi32_t(-56789), bi32_t(0), bi32_t(0), bi32_t(0), bi32_t(0), bi32_t(-56789));
	VALIDATE_FUNCTION_CALL_5(INT, "::Push64Pop64", bi64_t(-567890), bi64_t(0), bi64_t(0), bi64_t(0), bi64_t(0), bi64_t(-567890));

	return true;
}


DEFINE_VM_TEST(MemoryOperations, "scripts/vm_MemoryOperations.bond")
{
	/*
	using namespace Bond;
	bi8_t charValue = 123;
	bu8_t ucharValue = 234;
	bi16_t shortValue = -345;
	bu16_t ushortValue = 40000;

	VALIDATE_FUNCTION_CALL_1(INT, "::Loadc", &charValue, bi32_t(charValue));

	*/
	return true;
}


DEFINE_VM_TEST(BinaryOperators, "scripts/vm_BinaryOperators.bond")
{
	using namespace Bond;
	VALIDATE_FUNCTION_CALL_2(INT, "::IntAdd", bi32_t(65 + 13), bi32_t(65), bi32_t(13));
	VALIDATE_FUNCTION_CALL_2(INT, "::IntSub", bi32_t(43 - 12), bi32_t(43), bi32_t(12));

	return true;
}

#define TEST_ITEMS                              \
  TEST_ITEM(Constants)                          \
  TEST_ITEM(StackOperations)                    \
  TEST_ITEM(BinaryOperators)                    \

RUN_TESTS(VM, TEST_ITEMS)
