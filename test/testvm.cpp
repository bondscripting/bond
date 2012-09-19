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
	VALIDATE_FUNCTION_CALL_5(UINT, "::PushucPopc", bu8_t(234), bu8_t(0), bu8_t(0), bu8_t(0), bu8_t(0), bu8_t(234));
	VALIDATE_FUNCTION_CALL_5(INT, "::PushsPops", bi16_t(-1234), bi16_t(0), bi16_t(0), bi16_t(0), bi16_t(0), bi16_t(-1234));
	VALIDATE_FUNCTION_CALL_5(UINT, "::PushusPops", bu16_t(60000), bu16_t(0), bu16_t(0), bu16_t(0), bu16_t(0), bu16_t(60000));
	VALIDATE_FUNCTION_CALL_5(INT, "::Push32Pop32", bi32_t(-56789), bi32_t(0), bi32_t(0), bi32_t(0), bi32_t(0), bi32_t(-56789));
	VALIDATE_FUNCTION_CALL_5(LONG, "::Push64Pop64", bi64_t(-567890), bi64_t(0), bi64_t(0), bi64_t(0), bi64_t(0), bi64_t(-567890));

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


DEFINE_VM_TEST(TypeConversions, "scripts/vm_TypeConversions.bond")
{
	using namespace Bond;
	VALIDATE_FUNCTION_CALL_1(INT, "::Itoc", bi32_t(bi8_t(-17)), bi32_t(-17));
	VALIDATE_FUNCTION_CALL_1(INT, "::Itoc", bi32_t(bi8_t(0xabc)), bi32_t(0xabc));
	VALIDATE_FUNCTION_CALL_1(INT, "::Itoc", bi32_t(bi8_t(0x864)), bi32_t(0x864));

	VALIDATE_FUNCTION_CALL_1(UINT, "::Uitouc", bu32_t(bu8_t(17)), bu32_t(17));
	VALIDATE_FUNCTION_CALL_1(UINT, "::Uitouc", bu32_t(bu8_t(0xabc)), bu32_t(0xabc));
	VALIDATE_FUNCTION_CALL_1(UINT, "::Uitouc", bu32_t(bu8_t(0x864)), bu32_t(0x864));

	VALIDATE_FUNCTION_CALL_1(INT, "::Itos", bi32_t(bi16_t(-17)), bi32_t(-17));
	VALIDATE_FUNCTION_CALL_1(INT, "::Itos", bi32_t(bi16_t(0xabc)), bi32_t(0xabc));
	VALIDATE_FUNCTION_CALL_1(INT, "::Itos", bi32_t(bi16_t(0xabcde)), bi32_t(0xabcde));
	VALIDATE_FUNCTION_CALL_1(INT, "::Itos", bi32_t(bi16_t(0x987654)), bi32_t(0x987654));

	VALIDATE_FUNCTION_CALL_1(UINT, "::Uitous", bu32_t(bu16_t(17)), bu32_t(17));
	VALIDATE_FUNCTION_CALL_1(UINT, "::Uitous", bu32_t(bu16_t(0xabc)), bu32_t(0xabc));
	VALIDATE_FUNCTION_CALL_1(UINT, "::Uitous", bu32_t(bu16_t(0xabcde)), bu32_t(0xabcde));
	VALIDATE_FUNCTION_CALL_1(UINT, "::Uitous", bu32_t(bu16_t(0x987654)), bu32_t(0x987654));

	VALIDATE_FUNCTION_CALL_1(LONG, "::Itol", bi64_t(bi32_t(-4567)), bi32_t(-4567));
	VALIDATE_FUNCTION_CALL_1(LONG, "::Itol", bi64_t(bi32_t(4567)), bi32_t(4567));

	VALIDATE_FUNCTION_CALL_1(ULONG, "::Uitoul", bu64_t(bu32_t(7654)), bu32_t(7654));

	VALIDATE_FUNCTION_CALL_1(FLOAT, "::Itof", bf32_t(bi32_t(0)), bi32_t(0));
	VALIDATE_FUNCTION_CALL_1(FLOAT, "::Itof", bf32_t(bi32_t(1)), bi32_t(1));
	VALIDATE_FUNCTION_CALL_1(FLOAT, "::Itof", bf32_t(bi32_t(2)), bi32_t(2));
	VALIDATE_FUNCTION_CALL_1(FLOAT, "::Itof", bf32_t(bi32_t(-1)), bi32_t(-1));
	VALIDATE_FUNCTION_CALL_1(FLOAT, "::Itof", bf32_t(bi32_t(-2)), bi32_t(-2));
	VALIDATE_FUNCTION_CALL_1(FLOAT, "::Itof", bf32_t(bi32_t(-3000)), bi32_t(-3000));

	VALIDATE_FUNCTION_CALL_1(FLOAT, "::Uitof", bf32_t(bu32_t(0)), bu32_t(0));
	VALIDATE_FUNCTION_CALL_1(FLOAT, "::Uitof", bf32_t(bu32_t(1)), bu32_t(1));
	VALIDATE_FUNCTION_CALL_1(FLOAT, "::Uitof", bf32_t(bu32_t(2)), bu32_t(2));
	VALIDATE_FUNCTION_CALL_1(FLOAT, "::Uitof", bf32_t(bu32_t(3000)), bu32_t(3000));

	VALIDATE_FUNCTION_CALL_1(DOUBLE, "::Itod", bf64_t(bi32_t(0)), bi32_t(0));
	VALIDATE_FUNCTION_CALL_1(DOUBLE, "::Itod", bf64_t(bi32_t(1)), bi32_t(1));
	VALIDATE_FUNCTION_CALL_1(DOUBLE, "::Itod", bf64_t(bi32_t(2)), bi32_t(2));
	VALIDATE_FUNCTION_CALL_1(DOUBLE, "::Itod", bf64_t(bi32_t(-1)), bi32_t(-1));
	VALIDATE_FUNCTION_CALL_1(DOUBLE, "::Itod", bf64_t(bi32_t(-2)), bi32_t(-2));
	VALIDATE_FUNCTION_CALL_1(DOUBLE, "::Itod", bf64_t(bi32_t(-3000)), bi32_t(-3000));

	VALIDATE_FUNCTION_CALL_1(DOUBLE, "::Uitod", bf64_t(bu32_t(0)), bu32_t(0));
	VALIDATE_FUNCTION_CALL_1(DOUBLE, "::Uitod", bf64_t(bu32_t(1)), bu32_t(1));
	VALIDATE_FUNCTION_CALL_1(DOUBLE, "::Uitod", bf64_t(bu32_t(2)), bu32_t(2));
	VALIDATE_FUNCTION_CALL_1(DOUBLE, "::Uitod", bf64_t(bu32_t(3000)), bu32_t(3000));

	VALIDATE_FUNCTION_CALL_1(INT, "::Ltoi", bi32_t(bi64_t(-8745)), bi64_t(-8745));
	VALIDATE_FUNCTION_CALL_1(INT, "::Ltoi", bi32_t(bi64_t(0xabcdef) << 16), bi64_t(0xabcdef) << 16);
	VALIDATE_FUNCTION_CALL_1(INT, "::Ltoi", bi32_t(bi64_t(8745)), bi64_t(8745));

	VALIDATE_FUNCTION_CALL_1(FLOAT, "::Ltof", bf32_t(bi64_t(0)), bi64_t(0));
	VALIDATE_FUNCTION_CALL_1(FLOAT, "::Ltof", bf32_t(bi64_t(1)), bi64_t(1));
	VALIDATE_FUNCTION_CALL_1(FLOAT, "::Ltof", bf32_t(bi64_t(2)), bi64_t(2));
	VALIDATE_FUNCTION_CALL_1(FLOAT, "::Ltof", bf32_t(bi64_t(-1)), bi64_t(-1));
	VALIDATE_FUNCTION_CALL_1(FLOAT, "::Ltof", bf32_t(bi64_t(-2)), bi64_t(-2));
	VALIDATE_FUNCTION_CALL_1(FLOAT, "::Ltof", bf32_t(bi64_t(-3000)), bi64_t(-3000));

	VALIDATE_FUNCTION_CALL_1(FLOAT, "::Ultof", bf32_t(bu64_t(0)), bu64_t(0));
	VALIDATE_FUNCTION_CALL_1(FLOAT, "::Ultof", bf32_t(bu64_t(1)), bu64_t(1));
	VALIDATE_FUNCTION_CALL_1(FLOAT, "::Ultof", bf32_t(bu64_t(2)), bu64_t(2));
	VALIDATE_FUNCTION_CALL_1(FLOAT, "::Ultof", bf32_t(bu64_t(3000)), bu64_t(3000));

	VALIDATE_FUNCTION_CALL_1(DOUBLE, "::Ltod", bf64_t(bi64_t(0)), bi64_t(0));
	VALIDATE_FUNCTION_CALL_1(DOUBLE, "::Ltod", bf64_t(bi64_t(1)), bi64_t(1));
	VALIDATE_FUNCTION_CALL_1(DOUBLE, "::Ltod", bf64_t(bi64_t(2)), bi64_t(2));
	VALIDATE_FUNCTION_CALL_1(DOUBLE, "::Ltod", bf64_t(bi64_t(-1)), bi64_t(-1));
	VALIDATE_FUNCTION_CALL_1(DOUBLE, "::Ltod", bf64_t(bi64_t(-2)), bi64_t(-2));
	VALIDATE_FUNCTION_CALL_1(DOUBLE, "::Ltod", bf64_t(bi64_t(-3000)), bi64_t(-3000));

	VALIDATE_FUNCTION_CALL_1(DOUBLE, "::Ultod", bf64_t(bu64_t(0)), bu64_t(0));
	VALIDATE_FUNCTION_CALL_1(DOUBLE, "::Ultod", bf64_t(bu64_t(1)), bu64_t(1));
	VALIDATE_FUNCTION_CALL_1(DOUBLE, "::Ultod", bf64_t(bu64_t(2)), bu64_t(2));
	VALIDATE_FUNCTION_CALL_1(DOUBLE, "::Ultod", bf64_t(bu64_t(3000)), bu64_t(3000));

	VALIDATE_FUNCTION_CALL_1(INT, "::Ftoi", bi32_t(bf32_t(0)), bf32_t(0));
	VALIDATE_FUNCTION_CALL_1(INT, "::Ftoi", bi32_t(bf32_t(0.1)), bf32_t(0.1));
	VALIDATE_FUNCTION_CALL_1(INT, "::Ftoi", bi32_t(bf32_t(1.1)), bf32_t(1.1));
	VALIDATE_FUNCTION_CALL_1(INT, "::Ftoi", bi32_t(bf32_t(-0.1)), bf32_t(-0.1));
	VALIDATE_FUNCTION_CALL_1(INT, "::Ftoi", bi32_t(bf32_t(-1.1)), bf32_t(-1.1));
	VALIDATE_FUNCTION_CALL_1(INT, "::Ftoi", bi32_t(bf32_t(1.23e9)), bf32_t(1.23e9));

	VALIDATE_FUNCTION_CALL_1(UINT, "::Ftoui", bu32_t(bf32_t(0)), bf32_t(0));
	VALIDATE_FUNCTION_CALL_1(UINT, "::Ftoui", bu32_t(bf32_t(0.1)), bf32_t(0.1));
	VALIDATE_FUNCTION_CALL_1(UINT, "::Ftoui", bu32_t(bf32_t(1.1)), bf32_t(1.1));
	VALIDATE_FUNCTION_CALL_1(UINT, "::Ftoui", bu32_t(bf32_t(1.23e9)), bf32_t(1.23e9));

	VALIDATE_FUNCTION_CALL_1(LONG, "::Ftol", bi64_t(bf32_t(0)), bf32_t(0));
	VALIDATE_FUNCTION_CALL_1(LONG, "::Ftol", bi64_t(bf32_t(0.1)), bf32_t(0.1));
	VALIDATE_FUNCTION_CALL_1(LONG, "::Ftol", bi64_t(bf32_t(1.1)), bf32_t(1.1));
	VALIDATE_FUNCTION_CALL_1(LONG, "::Ftol", bi64_t(bf32_t(-0.1)), bf32_t(-0.1));
	VALIDATE_FUNCTION_CALL_1(LONG, "::Ftol", bi64_t(bf32_t(-1.1)), bf32_t(-1.1));
	VALIDATE_FUNCTION_CALL_1(LONG, "::Ftol", bi64_t(bf32_t(1.23e10)), bf32_t(1.23e10));

	VALIDATE_FUNCTION_CALL_1(ULONG, "::Ftoul", bu64_t(bf32_t(0)), bf32_t(0));
	VALIDATE_FUNCTION_CALL_1(ULONG, "::Ftoul", bu64_t(bf32_t(0.1)), bf32_t(0.1));
	VALIDATE_FUNCTION_CALL_1(ULONG, "::Ftoul", bu64_t(bf32_t(1.1)), bf32_t(1.1));
	VALIDATE_FUNCTION_CALL_1(ULONG, "::Ftoul", bu64_t(bf32_t(1.23e10)), bf32_t(1.23e10));

	VALIDATE_FUNCTION_CALL_1(DOUBLE, "::Ftod", bf64_t(0), bf32_t(0));
	VALIDATE_FUNCTION_CALL_1(DOUBLE, "::Ftod", bf64_t(0.1), bf32_t(0.1));
	VALIDATE_FUNCTION_CALL_1(DOUBLE, "::Ftod", bf64_t(1.1), bf32_t(1.1));
	VALIDATE_FUNCTION_CALL_1(DOUBLE, "::Ftod", bf64_t(-0.1), bf32_t(-0.1));
	VALIDATE_FUNCTION_CALL_1(DOUBLE, "::Ftod", bf64_t(-1.1), bf32_t(-1.1));
	VALIDATE_FUNCTION_CALL_1(DOUBLE, "::Ftod", bf64_t(1.23e10), bf32_t(1.23e10));

	VALIDATE_FUNCTION_CALL_1(INT, "::Dtoi", bi32_t(bf64_t(0)), bf64_t(0));
	VALIDATE_FUNCTION_CALL_1(INT, "::Dtoi", bi32_t(bf64_t(0.1)), bf64_t(0.1));
	VALIDATE_FUNCTION_CALL_1(INT, "::Dtoi", bi32_t(bf64_t(1.1)), bf64_t(1.1));
	VALIDATE_FUNCTION_CALL_1(INT, "::Dtoi", bi32_t(bf64_t(-0.1)), bf64_t(-0.1));
	VALIDATE_FUNCTION_CALL_1(INT, "::Dtoi", bi32_t(bf64_t(-1.1)), bf64_t(-1.1));
	VALIDATE_FUNCTION_CALL_1(INT, "::Dtoi", bi32_t(bf64_t(1.23e9)), bf64_t(1.23e9));

	VALIDATE_FUNCTION_CALL_1(UINT, "::Dtoui", bu32_t(bf64_t(0)), bf64_t(0));
	VALIDATE_FUNCTION_CALL_1(UINT, "::Dtoui", bu32_t(bf64_t(0.1)), bf64_t(0.1));
	VALIDATE_FUNCTION_CALL_1(UINT, "::Dtoui", bu32_t(bf64_t(1.1)), bf64_t(1.1));
	VALIDATE_FUNCTION_CALL_1(UINT, "::Dtoui", bu32_t(bf64_t(1.23e9)), bf64_t(1.23e9));

	VALIDATE_FUNCTION_CALL_1(LONG, "::Dtol", bi64_t(bf64_t(0)), bf64_t(0));
	VALIDATE_FUNCTION_CALL_1(LONG, "::Dtol", bi64_t(bf64_t(0.1)), bf64_t(0.1));
	VALIDATE_FUNCTION_CALL_1(LONG, "::Dtol", bi64_t(bf64_t(1.1)), bf64_t(1.1));
	VALIDATE_FUNCTION_CALL_1(LONG, "::Dtol", bi64_t(bf64_t(-0.1)), bf64_t(-0.1));
	VALIDATE_FUNCTION_CALL_1(LONG, "::Dtol", bi64_t(bf64_t(-1.1)), bf64_t(-1.1));
	VALIDATE_FUNCTION_CALL_1(LONG, "::Dtol", bi64_t(bf64_t(1.23e10)), bf64_t(1.23e10));

	VALIDATE_FUNCTION_CALL_1(ULONG, "::Dtoul", bu64_t(bf64_t(0)), bf64_t(0));
	VALIDATE_FUNCTION_CALL_1(ULONG, "::Dtoul", bu64_t(bf64_t(0.1)), bf64_t(0.1));
	VALIDATE_FUNCTION_CALL_1(ULONG, "::Dtoul", bu64_t(bf64_t(1.1)), bf64_t(1.1));
	VALIDATE_FUNCTION_CALL_1(ULONG, "::Dtoul", bu64_t(bf64_t(1.23e10)), bf64_t(1.23e10));

	VALIDATE_FUNCTION_CALL_1(FLOAT, "::Dtof", bf32_t(0), bf64_t(0));
	VALIDATE_FUNCTION_CALL_1(FLOAT, "::Dtof", bf32_t(0.1), bf64_t(0.1));
	VALIDATE_FUNCTION_CALL_1(FLOAT, "::Dtof", bf32_t(1.1), bf64_t(1.1));
	VALIDATE_FUNCTION_CALL_1(FLOAT, "::Dtof", bf32_t(-0.1), bf64_t(-0.1));
	VALIDATE_FUNCTION_CALL_1(FLOAT, "::Dtof", bf32_t(-1.1), bf64_t(-1.1));
	VALIDATE_FUNCTION_CALL_1(FLOAT, "::Dtof", bf32_t(1.23e10), bf64_t(1.23e10));

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
  TEST_ITEM(TypeConversions)                    \
  TEST_ITEM(BinaryOperators)                    \

RUN_TESTS(VM, TEST_ITEMS)
