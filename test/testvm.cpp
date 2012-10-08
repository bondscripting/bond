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
	using namespace Bond;
	bi8_t chars[] = {0, -123, 0, 0, 0};
	VALIDATE_FUNCTION_CALL_2(INT, "::LoadcStorec", bi32_t(-123), chars + 1, chars + 3);
	ASSERT_FORMAT(chars[1] == chars[3],
		("Expected %" BOND_PRId32 ", but was %" BOND_PRId32 ".", chars[1], chars[3]));
	ASSERT_MESSAGE((chars[0] == 0) && (chars[2] == 0) && (chars[4] == 0),
		"Memory stomp by LoadcStorec() detected.");

	bu8_t uchars[] = {0, 0xff, 0, 0, 0};
	VALIDATE_FUNCTION_CALL_2(UINT, "::LoaducStorec", bu32_t(0xff), uchars + 1, uchars + 3);
	ASSERT_FORMAT(uchars[1] == uchars[3],
		("Expected %" BOND_PRIu32 ", but was %" BOND_PRIu32 ".", uchars[1], uchars[3]));
	ASSERT_MESSAGE((uchars[0] == 0) && (uchars[2] == 0) && (uchars[4] == 0),
		"Memory stomp by LoaducStorec() detected.");

	bi16_t shorts[] = {0, -234, 0, 0, 0};
	VALIDATE_FUNCTION_CALL_2(INT, "::LoadsStores", bi32_t(-234), shorts + 1, shorts + 3);
	ASSERT_FORMAT(shorts[1] == shorts[3],
		("Expected %" BOND_PRId32 ", but was %" BOND_PRId32 ".", shorts[1], shorts[3]));
	ASSERT_MESSAGE((shorts[0] == 0) && (shorts[2] == 0) && (shorts[4] == 0),
		"Memory stomp by LoadsStores() detected.");

	bu16_t ushorts[] = {0, 0xffff, 0, 0, 0};
	VALIDATE_FUNCTION_CALL_2(UINT, "::LoadusStores", bi32_t(0xffff), ushorts + 1, ushorts + 3);
	ASSERT_FORMAT(ushorts[1] == ushorts[3],
		("Expected %" BOND_PRIu32 ", but was %" BOND_PRIu32 ".", ushorts[1], ushorts[3]));
	ASSERT_MESSAGE((ushorts[0] == 0) && (ushorts[2] == 0) && (ushorts[4] == 0),
		"Memory stomp by LoadusStores() detected.");

	bi32_t ints[] = {0, -345, 0, 0, 0};
	VALIDATE_FUNCTION_CALL_2(INT, "::Load32Store32", bi32_t(-345), ints + 1, ints + 3);
	ASSERT_FORMAT(ints[1] == ints[3],
		("Expected %" BOND_PRId32 ", but was %" BOND_PRId32 ".", ints[1], ints[3]));
	ASSERT_MESSAGE((ints[0] == 0) && (ints[2] == 0) && (ints[4] == 0),
		"Memory stomp by Load32Store32() detected.");

	bi64_t longs[] = {0, -456, 0, 0, 0};
	VALIDATE_FUNCTION_CALL_2(LONG, "::Load64Store64", bi64_t(-456), longs + 1, longs + 3);
	ASSERT_FORMAT(longs[1] == longs[3],
		("Expected %" BOND_PRId64 ", but was %" BOND_PRId64 ".", longs[1], longs[3]));
	ASSERT_MESSAGE((longs[0] == 0) && (longs[2] == 0) && (longs[4] == 0),
		"Memory stomp by Load64Store64() detected.");

	VALIDATE_FUNCTION_CALL_1(INT, "::Loadfp", bi32_t(-567), bi32_t(-567));

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
	VALIDATE_FUNCTION_CALL_2(INT, "::Addi", bi32_t(-65) + bi32_t(13), bi32_t(-65), bi32_t(13));
	VALIDATE_FUNCTION_CALL_2(UINT, "::Addi", bu32_t(65) + bi32_t(13), bu32_t(65), bu32_t(13));
	VALIDATE_FUNCTION_CALL_2(LONG, "::Addl", (bi64_t(-65) << 40) + (bi64_t(13) << 40), (bi64_t(-65) << 40), (bi64_t(13) << 40));
	VALIDATE_FUNCTION_CALL_2(ULONG, "::Addl", (bu64_t(65) << 40) + (bu64_t(13) << 40), (bu64_t(65) << 40), (bu64_t(13) << 40));
	VALIDATE_FUNCTION_CALL_2(FLOAT, "::Addf", bf32_t(-65.0f) + bf32_t(13.0f), bf32_t(-65.0f), bf32_t(13.0f));
	VALIDATE_FUNCTION_CALL_2(FLOAT, "::Addf", bf32_t(1e20f) + bf32_t(0.5e20f), bf32_t(1e20f), bf32_t(0.5e20f));
	VALIDATE_FUNCTION_CALL_2(DOUBLE, "::Addd", bf64_t(-65.0) + bf64_t(13.0), bf64_t(-65.0), bf64_t(13.0));
	VALIDATE_FUNCTION_CALL_2(DOUBLE, "::Addd", bf64_t(1e40) + bf64_t(0.5e40), bf64_t(1e40), bf64_t(0.5e40));

	VALIDATE_FUNCTION_CALL_2(INT, "::Subi", bi32_t(-43) - bi32_t(12), bi32_t(-43), bi32_t(12));
	VALIDATE_FUNCTION_CALL_2(UINT, "::Subi", bu32_t(43) - bi32_t(12), bu32_t(43), bu32_t(12));
	VALIDATE_FUNCTION_CALL_2(LONG, "::Subl", (bi64_t(-43) << 40) - (bi64_t(12) << 40), (bi64_t(-43) << 40), (bi64_t(12) << 40));
	VALIDATE_FUNCTION_CALL_2(ULONG, "::Subl", (bu64_t(43) << 40) - (bu64_t(12) << 40), (bu64_t(43) << 40), (bu64_t(12) << 40));
	VALIDATE_FUNCTION_CALL_2(FLOAT, "::Subf", bf32_t(-43.0f) - bf32_t(12.0f), bf32_t(-43.0f), bf32_t(12.0f));
	VALIDATE_FUNCTION_CALL_2(FLOAT, "::Subf", bf32_t(1e20f) - bf32_t(0.5e20f), bf32_t(1e20f), bf32_t(0.5e20f));
	VALIDATE_FUNCTION_CALL_2(DOUBLE, "::Subd", bf64_t(-43.0) - bf64_t(12.0), bf64_t(-43.0), bf64_t(12.0));
	VALIDATE_FUNCTION_CALL_2(DOUBLE, "::Subd", bf64_t(1e40) - bf64_t(0.5e40), bf64_t(1e40), bf64_t(0.5e40));

	VALIDATE_FUNCTION_CALL_2(INT, "::Muli", bi32_t(-58) * bi32_t(0), bi32_t(-58), bi32_t(0));
	VALIDATE_FUNCTION_CALL_2(INT, "::Muli", bi32_t(-58) * bi32_t(17), bi32_t(-58), bi32_t(17));
	VALIDATE_FUNCTION_CALL_2(UINT, "::Mului", bu32_t(58) * bi32_t(17), bu32_t(58), bu32_t(17));
	VALIDATE_FUNCTION_CALL_2(LONG, "::Mull", (bi64_t(-58) << 40) * bi64_t(17), (bi64_t(-58) << 40), bi64_t(17));
	VALIDATE_FUNCTION_CALL_2(ULONG, "::Mulul", (bu64_t(58) << 40) * bu64_t(17), (bu64_t(58) << 40), bu64_t(17));
	VALIDATE_FUNCTION_CALL_2(FLOAT, "::Mulf", bf32_t(0.0f) * bf32_t(84.3f), bf32_t(-0.0f), bf32_t(84.3f));
	VALIDATE_FUNCTION_CALL_2(FLOAT, "::Mulf", bf32_t(-98.1f) * bf32_t(84.3f), bf32_t(-98.1f), bf32_t(84.3f));
	VALIDATE_FUNCTION_CALL_2(FLOAT, "::Mulf", bf32_t(1e20f) * bf32_t(3.3e10f), bf32_t(1e20f), bf32_t(3.3e10f));
	VALIDATE_FUNCTION_CALL_2(DOUBLE, "::Muld", bf64_t(0.0) * bf64_t(84.3), bf64_t(-0.0), bf64_t(84.3));
	VALIDATE_FUNCTION_CALL_2(DOUBLE, "::Muld", bf64_t(-98.1) * bf64_t(84.3), bf64_t(-98.1), bf64_t(84.3));
	VALIDATE_FUNCTION_CALL_2(DOUBLE, "::Muld", bf64_t(1e40) * bf64_t(3.3e20), bf64_t(1e40), bf64_t(3.3e20));

	VALIDATE_FUNCTION_CALL_2(INT, "::Divi", bi32_t(0) / bi32_t(3), bi32_t(0), bi32_t(3));
	VALIDATE_FUNCTION_CALL_2(INT, "::Divi", bi32_t(-10) / bi32_t(3), bi32_t(-10), bi32_t(3));
	VALIDATE_FUNCTION_CALL_2(UINT, "::Divui", Bond::BOND_UINT_MAX / bu32_t(5678), BOND_UINT_MAX, bu32_t(5678));
	VALIDATE_FUNCTION_CALL_2(LONG, "::Divl", Bond::BOND_LONG_MIN / bi64_t(5678), BOND_LONG_MIN, bi64_t(5678));
	VALIDATE_FUNCTION_CALL_2(ULONG, "::Divul", Bond::BOND_ULONG_MAX / bu64_t(5678), BOND_ULONG_MAX, bu64_t(5678));
	VALIDATE_FUNCTION_CALL_2(FLOAT, "::Divf", bf32_t(0.0f) / bf32_t(84.3f), bf32_t(-0.0f), bf32_t(84.3f));
	VALIDATE_FUNCTION_CALL_2(FLOAT, "::Divf", bf32_t(-98.1f) / bf32_t(84.3f), bf32_t(-98.1f), bf32_t(84.3f));
	VALIDATE_FUNCTION_CALL_2(FLOAT, "::Divf", bf32_t(1e20f) / bf32_t(3.3e10f), bf32_t(1e20f), bf32_t(3.3e10f));
	VALIDATE_FUNCTION_CALL_2(DOUBLE, "::Divd", bf64_t(0.0) / bf64_t(84.3), bf64_t(-0.0), bf64_t(84.3));
	VALIDATE_FUNCTION_CALL_2(DOUBLE, "::Divd", bf64_t(-98.1) / bf64_t(84.3), bf64_t(-98.1), bf64_t(84.3));
	VALIDATE_FUNCTION_CALL_2(DOUBLE, "::Divd", bf64_t(1e40) / bf64_t(3.3e20), bf64_t(1e40), bf64_t(3.3e20));

	VALIDATE_FUNCTION_CALL_2(INT, "::Remi", bi32_t(0) % bi32_t(3), bi32_t(0), bi32_t(3));
	VALIDATE_FUNCTION_CALL_2(INT, "::Remi", bi32_t(10) % bi32_t(3), bi32_t(10), bi32_t(3));
	VALIDATE_FUNCTION_CALL_2(INT, "::Remi", bi32_t(-10) % bi32_t(3), bi32_t(-10), bi32_t(3));
	VALIDATE_FUNCTION_CALL_2(INT, "::Remi", bi32_t(2352) % bi32_t(24), bi32_t(2352), bi32_t(24));
	VALIDATE_FUNCTION_CALL_2(INT, "::Remui", bu32_t(0) % bu32_t(3), bu32_t(0), bu32_t(3));
	VALIDATE_FUNCTION_CALL_2(INT, "::Remui", bu32_t(10) % bu32_t(3), bu32_t(10), bu32_t(3));
	VALIDATE_FUNCTION_CALL_2(INT, "::Remui", bu32_t(2352) % bu32_t(24), bu32_t(2352), bu32_t(24));
	VALIDATE_FUNCTION_CALL_2(UINT, "::Remui", Bond::BOND_UINT_MAX % bu32_t(5678), BOND_UINT_MAX, bu32_t(5678));
	VALIDATE_FUNCTION_CALL_2(LONG, "::Reml", Bond::BOND_LONG_MIN % bi64_t(5678), BOND_LONG_MIN, bi64_t(5678));
	VALIDATE_FUNCTION_CALL_2(ULONG, "::Remul", Bond::BOND_ULONG_MAX % bu64_t(5678), BOND_ULONG_MAX, bu64_t(5678));

	VALIDATE_FUNCTION_CALL_2(INT, "::Lshi", bi32_t(0) << bi32_t(1), bi32_t(0), bi32_t(1));
	VALIDATE_FUNCTION_CALL_2(UINT, "::Lshi", bu32_t(1) << bu32_t(1), bu32_t(1), bu32_t(1));
	VALIDATE_FUNCTION_CALL_2(INT, "::Lshi", bi32_t(1) << bi32_t(31), bi32_t(1), bi32_t(31));
	VALIDATE_FUNCTION_CALL_2(INT, "::Lshi", bi32_t(2) << bi32_t(31), bi32_t(2), bi32_t(31));
	VALIDATE_FUNCTION_CALL_2(INT, "::Lshi", bi32_t(3) << bi32_t(31), bi32_t(3), bi32_t(31));
	VALIDATE_FUNCTION_CALL_2(INT, "::Lshi", bi32_t(-3) << bi32_t(16), bi32_t(-3), bi32_t(16));

	VALIDATE_FUNCTION_CALL_2(LONG, "::Lshl", bi64_t(0) << bi64_t(1), bi64_t(0), bi64_t(1));
	VALIDATE_FUNCTION_CALL_2(ULONG, "::Lshl", bu64_t(1) << bu64_t(1), bu64_t(1), bu64_t(1));
	VALIDATE_FUNCTION_CALL_2(LONG, "::Lshl", bi64_t(1) << bi64_t(63), bi64_t(1), bi64_t(63));
	VALIDATE_FUNCTION_CALL_2(LONG, "::Lshl", bi64_t(2) << bi64_t(63), bi64_t(2), bi64_t(63));
	VALIDATE_FUNCTION_CALL_2(LONG, "::Lshl", bi64_t(3) << bi64_t(63), bi64_t(3), bi64_t(63));
	VALIDATE_FUNCTION_CALL_2(LONG, "::Lshl", bi64_t(-3) << bi64_t(16), bi64_t(-3), bi64_t(16));

	VALIDATE_FUNCTION_CALL_2(INT, "::Rshi", bi32_t(0) >> bi32_t(1), bi32_t(0), bi32_t(1));
	VALIDATE_FUNCTION_CALL_2(UINT, "::Rshi", bu32_t(2) >> bu32_t(1), bu32_t(2), bu32_t(1));
	VALIDATE_FUNCTION_CALL_2(INT, "::Rshi", bi32_t(1) >> bi32_t(1), bi32_t(1), bi32_t(1));
	VALIDATE_FUNCTION_CALL_2(INT, "::Rshi", bi32_t(2) >> bi32_t(1), bi32_t(2), bi32_t(1));
	VALIDATE_FUNCTION_CALL_2(INT, "::Rshi", bi32_t(3) >> bi32_t(1), bi32_t(3), bi32_t(1));
	VALIDATE_FUNCTION_CALL_2(INT, "::Rshi", -(bi32_t(1) << 20) >> bi32_t(16), -(bi32_t(1) << 20), bi32_t(16));
	VALIDATE_FUNCTION_CALL_2(UINT, "::Rshui", bu32_t(-(bi32_t(1) << 20)) >> bu32_t(16), bu32_t(-(bi32_t(1) << 20)), bu32_t(16));

	VALIDATE_FUNCTION_CALL_2(LONG, "::Rshl", bi64_t(0) >> bi64_t(1), bi64_t(0), bi64_t(1));
	VALIDATE_FUNCTION_CALL_2(LONG, "::Rshl", bi64_t(1) >> bi64_t(1), bi64_t(1), bi64_t(1));
	VALIDATE_FUNCTION_CALL_2(LONG, "::Rshl", bi64_t(2) >> bi64_t(1), bi64_t(2), bi64_t(1));
	VALIDATE_FUNCTION_CALL_2(LONG, "::Rshl", bi64_t(3) >> bi64_t(1), bi64_t(3), bi64_t(1));
	VALIDATE_FUNCTION_CALL_2(LONG, "::Rshl", -(bi64_t(1) << bi64_t(20)) >> bi64_t(16), -(bi64_t(1) << bi64_t(20)), bi64_t(16));
	VALIDATE_FUNCTION_CALL_2(ULONG, "::Rshul", bu64_t(-(bi64_t(1) << bi64_t(20))) >> bu64_t(16), bu64_t(-(bi64_t(1) << bi64_t(20))), bu64_t(16));

	VALIDATE_FUNCTION_CALL_2(INT, "::Andi", bi32_t(0xffff) & bi32_t(0xff), bi32_t(0xffff), bi32_t(0xff));
	VALIDATE_FUNCTION_CALL_2(LONG, "::Andl", (bi64_t(0xffff) << 31) & (bi64_t(0xff) << 31), (bi64_t(0xffff) << 31), (bi64_t(0xff) << 31));
	VALIDATE_FUNCTION_CALL_2(INT, "::Ori", bi32_t(0xffff) | bi32_t(0xff), bi32_t(0xffff), bi32_t(0xff));
	VALIDATE_FUNCTION_CALL_2(LONG, "::Orl", (bi64_t(0xffff) << 31) | (bi64_t(0xff) << 31), (bi64_t(0xffff) << 31), (bi64_t(0xff) << 31));
	VALIDATE_FUNCTION_CALL_2(INT, "::Xori", bi32_t(0xffff) ^ bi32_t(0xff), bi32_t(0xffff), bi32_t(0xff));
	VALIDATE_FUNCTION_CALL_2(LONG, "::Xorl", (bi64_t(0xffff) << 31) ^ (bi64_t(0xff) << 31), (bi64_t(0xffff) << 31), (bi64_t(0xff) << 31));

	return true;
}


DEFINE_VM_TEST(UnaryOperators, "scripts/vm_UnaryOperators.bond")
{
	using namespace Bond;

	VALIDATE_FUNCTION_CALL_1(INT, "::Negi", bi32_t(0), bi32_t(0));
	VALIDATE_FUNCTION_CALL_1(INT, "::Negi", bi32_t(-1), bi32_t(1));
	VALIDATE_FUNCTION_CALL_1(INT, "::Negi", bi32_t(1), bi32_t(-1));
	VALIDATE_FUNCTION_CALL_1(INT, "::Negi", bi32_t(-BOND_INT_MAX), bi32_t(BOND_INT_MAX));
	VALIDATE_FUNCTION_CALL_1(INT, "::Negi", bi32_t(BOND_INT_MAX), bi32_t(-BOND_INT_MAX));

	VALIDATE_FUNCTION_CALL_1(LONG, "::Negl", bi64_t(0), bi64_t(0));
	VALIDATE_FUNCTION_CALL_1(LONG, "::Negl", bi64_t(-1), bi64_t(1));
	VALIDATE_FUNCTION_CALL_1(LONG, "::Negl", bi64_t(1), bi64_t(-1));
	VALIDATE_FUNCTION_CALL_1(LONG, "::Negl", bi64_t(-BOND_LONG_MAX), bi64_t(BOND_LONG_MAX));
	VALIDATE_FUNCTION_CALL_1(LONG, "::Negl", bi64_t(BOND_LONG_MAX), bi64_t(-BOND_LONG_MAX));

	VALIDATE_FUNCTION_CALL_1(FLOAT, "::Negf", bf32_t(0.0f), bf32_t(0.0f));
	VALIDATE_FUNCTION_CALL_1(FLOAT, "::Negf", bf32_t(-1.0f), bf32_t(1.0f));
	VALIDATE_FUNCTION_CALL_1(FLOAT, "::Negf", bf32_t(1.0f), bf32_t(-1.0f));
	VALIDATE_FUNCTION_CALL_1(FLOAT, "::Negf", bf32_t(-2.34e21f), bf32_t(2.34e21f));
	VALIDATE_FUNCTION_CALL_1(FLOAT, "::Negf", bf32_t(2.34e21f), bf32_t(-2.34e21f));

	VALIDATE_FUNCTION_CALL_1(DOUBLE, "::Negd", bf64_t(0.0), bf64_t(0.0));
	VALIDATE_FUNCTION_CALL_1(DOUBLE, "::Negd", bf64_t(-1.0), bf64_t(1.0));
	VALIDATE_FUNCTION_CALL_1(DOUBLE, "::Negd", bf64_t(1.0), bf64_t(-1.0));
	VALIDATE_FUNCTION_CALL_1(DOUBLE, "::Negd", bf64_t(-2.34e21), bf64_t(2.34e21));
	VALIDATE_FUNCTION_CALL_1(DOUBLE, "::Negd", bf64_t(2.34e21), bf64_t(-2.34e21));

	VALIDATE_FUNCTION_CALL_1(UINT, "::Not", bu32_t(1), bu8_t(0));
	VALIDATE_FUNCTION_CALL_1(UINT, "::Not", bu32_t(0), bu8_t(1));
	VALIDATE_FUNCTION_CALL_1(UINT, "::Not", bu32_t(0), bu8_t(23));

	VALIDATE_FUNCTION_CALL_1(INT, "::Preinc1", bi32_t(6), bi32_t(5));
	VALIDATE_FUNCTION_CALL_1(INT, "::Preinc2", bi32_t(8), bi32_t(7));
	VALIDATE_FUNCTION_CALL_1(INT, "::Preinc3", bi32_t(10), bi32_t(9));
	VALIDATE_FUNCTION_CALL_1(INT, "::Preinc4", bi32_t(12), bi32_t(11));

	bi32_t value = 13;
	VALIDATE_FUNCTION_CALL_1(INT, "::Preinc5", bi32_t(14), &value);
	ASSERT_FORMAT(value == bi32_t(14),
		("Expected 14, but was %" BOND_PRId32 ".", value));

	VALIDATE_FUNCTION_CALL_1(INT, "::Postinc1", bi32_t(6), bi32_t(5));
	VALIDATE_FUNCTION_CALL_1(INT, "::Postinc2", bi32_t(8), bi32_t(7));
	VALIDATE_FUNCTION_CALL_1(INT, "::Postinc3", bi32_t(10), bi32_t(9));
	VALIDATE_FUNCTION_CALL_1(INT, "::Postinc4", bi32_t(12), bi32_t(11));

	VALIDATE_FUNCTION_CALL_1(INT, "::Postinc5", bi32_t(14), &value);
	ASSERT_FORMAT(value == bi32_t(15),
		("Expected 15, but was %" BOND_PRId32 ".", value));

	VALIDATE_FUNCTION_CALL_1(INT, "::Predec1", bi32_t(4), bi32_t(5));
	VALIDATE_FUNCTION_CALL_1(INT, "::Predec2", bi32_t(6), bi32_t(7));
	VALIDATE_FUNCTION_CALL_1(INT, "::Predec3", bi32_t(8), bi32_t(9));
	VALIDATE_FUNCTION_CALL_1(INT, "::Predec4", bi32_t(10), bi32_t(11));

	VALIDATE_FUNCTION_CALL_1(INT, "::Predec5", bi32_t(14), &value);
	ASSERT_FORMAT(value == bi32_t(14),
		("Expected 14, but was %" BOND_PRId32 ".", value));

	VALIDATE_FUNCTION_CALL_1(INT, "::Postdec1", bi32_t(4), bi32_t(5));
	VALIDATE_FUNCTION_CALL_1(INT, "::Postdec2", bi32_t(6), bi32_t(7));
	VALIDATE_FUNCTION_CALL_1(INT, "::Postdec3", bi32_t(8), bi32_t(9));
	VALIDATE_FUNCTION_CALL_1(INT, "::Postdec4", bi32_t(10), bi32_t(11));

	VALIDATE_FUNCTION_CALL_1(INT, "::Postdec5", bi32_t(14), &value);
	ASSERT_FORMAT(value == bi32_t(13),
		("Expected 13, but was %" BOND_PRId32 ".", value));

	return true;
}


DEFINE_VM_TEST(ComparisonOperators, "scripts/vm_ComparisonOperators.bond")
{
	using namespace Bond;

	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpeqi", bi32_t(1), bi32_t(0), bi32_t(0));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpeqi", bi32_t(1), bi32_t(-1), bi32_t(-1));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpeqi", bi32_t(0), bi32_t(0), bi32_t(1));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpeqi", bi32_t(0), bi32_t(8382), bi32_t(2398));

	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpeql", bi32_t(1), bi64_t(0), bi64_t(0));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpeql", bi32_t(1), bi64_t(-1), bi64_t(-1));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpeql", bi32_t(1), bi64_t(8382) << 31, bi64_t(8382) << 31);
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpeql", bi32_t(0), bi64_t(0), bi64_t(1));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpeql", bi32_t(0), bi64_t(8382) << 31, bi64_t(2398) << 31);

	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpeqf", bi32_t(1), bf32_t(0.0f), bf32_t(0.0f));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpeqf", bi32_t(1), bf32_t(-1.0f), bf32_t(-1.0f));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpeqf", bi32_t(1), bf32_t(5.67e27f), bf32_t(5.67e27f));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpeqf", bi32_t(0), bf32_t(0.0f), bf32_t(1.0f));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpeqf", bi32_t(0), bf32_t(5.67e27f), bf32_t(5.68e27f));

	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpeqd", bi32_t(1), bf64_t(0.0), bf64_t(0.0));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpeqd", bi32_t(1), bf64_t(-1.0), bf64_t(-1.0));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpeqd", bi32_t(1), bf64_t(5.67e27), bf64_t(5.67e27));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpeqd", bi32_t(0), bf64_t(0.0), bf64_t(1.0));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpeqd", bi32_t(0), bf64_t(5.67e27), bf64_t(5.68e27));

	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpneqi", bi32_t(0), bi32_t(0), bi32_t(0));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpneqi", bi32_t(0), bi32_t(-1), bi32_t(-1));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpneqi", bi32_t(1), bi32_t(0), bi32_t(1));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpneqi", bi32_t(1), bi32_t(8382), bi32_t(2398));

	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpneql", bi32_t(0), bi64_t(0), bi64_t(0));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpneql", bi32_t(0), bi64_t(-1), bi64_t(-1));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpneql", bi32_t(0), bi64_t(8382) << 31, bi64_t(8382) << 31);
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpneql", bi32_t(1), bi64_t(0), bi64_t(1));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpneql", bi32_t(1), bi64_t(8382) << 31, bi64_t(2398) << 31);

	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpneqf", bi32_t(0), bf32_t(0.0f), bf32_t(0.0f));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpneqf", bi32_t(0), bf32_t(-1.0f), bf32_t(-1.0f));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpneqf", bi32_t(0), bf32_t(5.67e27f), bf32_t(5.67e27f));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpneqf", bi32_t(1), bf32_t(0.0f), bf32_t(1.0f));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpneqf", bi32_t(1), bf32_t(5.67e27f), bf32_t(5.68e27f));

	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpneqd", bi32_t(0), bf64_t(0.0), bf64_t(0.0));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpneqd", bi32_t(0), bf64_t(-1.0), bf64_t(-1.0));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpneqd", bi32_t(0), bf64_t(5.67e27), bf64_t(5.67e27));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpneqd", bi32_t(1), bf64_t(0.0), bf64_t(1.0));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpneqd", bi32_t(1), bf64_t(5.67e27), bf64_t(5.68e27));

	VALIDATE_FUNCTION_CALL_2(INT, "::Cmplti", bi32_t(0), bi32_t(0), bi32_t(0));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmplti", bi32_t(1), bi32_t(0), bi32_t(1));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmplti", bi32_t(1), bi32_t(-2), bi32_t(2));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmplti", bi32_t(0), bi32_t(8382), bi32_t(2398));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmplti", bi32_t(1), bi32_t(2398), bi32_t(8382));

	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpltui", bi32_t(0), bu32_t(0), bu32_t(0));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpltui", bi32_t(1), bu32_t(0), bu32_t(1));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpltui", bi32_t(0), bi32_t(-2), bu32_t(2));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpltui", bi32_t(0), bu32_t(8382), bu32_t(2398));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpltui", bi32_t(1), bu32_t(2398), bu32_t(8382));

	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpltl", bi32_t(0), bi64_t(0), bi64_t(0));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpltl", bi32_t(1), bi64_t(0), bi64_t(1));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpltl", bi32_t(1), bi64_t(-2), bi64_t(2));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpltl", bi32_t(0), bi64_t(8382) << 31, bi64_t(2398) << 31);
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpltl", bi32_t(1), bi64_t(2938) << 31, bi64_t(8382) << 31);

	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpltul", bi32_t(0), bu64_t(0), bu64_t(0));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpltul", bi32_t(1), bu64_t(0), bu64_t(1));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpltul", bi32_t(0), bi64_t(-2), bu64_t(2));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpltul", bi32_t(0), bu64_t(8382) << 31, bu64_t(2398) << 31);
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpltul", bi32_t(1), bu64_t(2938) << 31, bu64_t(8382) << 31);

	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpltf", bi32_t(0), bf32_t(0.0f), bf32_t(0.0f));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpltf", bi32_t(1), bf32_t(0.0f), bf32_t(1.0f));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpltf", bi32_t(1), bf32_t(-2.0f), bf32_t(2.0f));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpltf", bi32_t(0), bf32_t(5.68e27f), bf32_t(5.67e27f));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpltf", bi32_t(1), bf32_t(5.67e27f), bf32_t(5.68e27f));

	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpltd", bi32_t(0), bf64_t(0.0), bf64_t(0.0));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpltd", bi32_t(1), bf64_t(0.0), bf64_t(1.0));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpltd", bi32_t(1), bf64_t(-2.0), bf64_t(2.0));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpltd", bi32_t(0), bf64_t(5.68e27), bf64_t(5.67e27));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpltd", bi32_t(1), bf64_t(5.67e27), bf64_t(5.68e27));

	VALIDATE_FUNCTION_CALL_2(INT, "::Cmplei", bi32_t(1), bi32_t(0), bi32_t(0));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmplei", bi32_t(1), bi32_t(0), bi32_t(1));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmplei", bi32_t(1), bi32_t(-2), bi32_t(2));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmplei", bi32_t(0), bi32_t(8382), bi32_t(2398));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmplei", bi32_t(1), bi32_t(2398), bi32_t(8382));

	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpleui", bi32_t(1), bu32_t(0), bu32_t(0));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpleui", bi32_t(1), bu32_t(0), bu32_t(1));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpleui", bi32_t(0), bi32_t(-2), bu32_t(2));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpleui", bi32_t(0), bu32_t(8382), bu32_t(2398));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpleui", bi32_t(1), bu32_t(2398), bu32_t(8382));

	VALIDATE_FUNCTION_CALL_2(INT, "::Cmplel", bi32_t(1), bi64_t(0), bi64_t(0));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmplel", bi32_t(1), bi64_t(0), bi64_t(1));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmplel", bi32_t(1), bi64_t(-2), bi64_t(2));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmplel", bi32_t(0), bi64_t(8382) << 31, bi64_t(2398) << 31);
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmplel", bi32_t(1), bi64_t(2938) << 31, bi64_t(8382) << 31);

	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpleul", bi32_t(1), bu64_t(0), bu64_t(0));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpleul", bi32_t(1), bu64_t(0), bu64_t(1));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpleul", bi32_t(0), bi64_t(-2), bu64_t(2));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpleul", bi32_t(0), bu64_t(8382) << 31, bu64_t(2398) << 31);
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpleul", bi32_t(1), bu64_t(2938) << 31, bu64_t(8382) << 31);

	VALIDATE_FUNCTION_CALL_2(INT, "::Cmplef", bi32_t(1), bf32_t(0.0f), bf32_t(0.0f));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmplef", bi32_t(1), bf32_t(0.0f), bf32_t(1.0f));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmplef", bi32_t(1), bf32_t(-2.0f), bf32_t(2.0f));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmplef", bi32_t(0), bf32_t(5.68e27f), bf32_t(5.67e27f));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmplef", bi32_t(1), bf32_t(5.67e27f), bf32_t(5.68e27f));

	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpled", bi32_t(1), bf64_t(0.0), bf64_t(0.0));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpled", bi32_t(1), bf64_t(0.0), bf64_t(1.0));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpled", bi32_t(1), bf64_t(-2.0), bf64_t(2.0));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpled", bi32_t(0), bf64_t(5.68e27), bf64_t(5.67e27));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpled", bi32_t(1), bf64_t(5.67e27), bf64_t(5.68e27));

	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgti", bi32_t(0), bi32_t(0), bi32_t(0));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgti", bi32_t(1), bi32_t(1), bi32_t(0));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgti", bi32_t(1), bi32_t(2), bi32_t(-2));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgti", bi32_t(0), bi32_t(2398), bi32_t(8382));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgti", bi32_t(1), bi32_t(8382), bi32_t(2398));

	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgtui", bi32_t(0), bu32_t(0), bu32_t(0));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgtui", bi32_t(1), bu32_t(1), bu32_t(0));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgtui", bi32_t(0), bu32_t(2), bi32_t(-2));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgtui", bi32_t(0), bu32_t(2398), bu32_t(8382));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgtui", bi32_t(1), bu32_t(8382), bu32_t(2398));

	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgtl", bi32_t(0), bi64_t(0), bi64_t(0));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgtl", bi32_t(1), bi64_t(1), bi64_t(0));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgtl", bi32_t(1), bi64_t(2), bi64_t(-2));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgtl", bi32_t(0), bi64_t(2398) << 31, bi64_t(8382) << 31);
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgtl", bi32_t(1), bi64_t(8382) << 31, bi64_t(2938) << 31);

	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgtul", bi32_t(0), bu64_t(0), bu64_t(0));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgtul", bi32_t(1), bu64_t(1), bu64_t(0));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgtul", bi32_t(0), bu64_t(2), bi64_t(-2));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgtul", bi32_t(0), bu64_t(2398) << 31, bu64_t(8382) << 31);
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgtul", bi32_t(1), bu64_t(8382) << 31, bu64_t(2938) << 31);

	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgtf", bi32_t(0), bf32_t(0.0f), bf32_t(0.0f));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgtf", bi32_t(1), bf32_t(1.0f), bf32_t(0.0f));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgtf", bi32_t(1), bf32_t(2.0f), bf32_t(-2.0f));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgtf", bi32_t(0), bf32_t(5.67e27f), bf32_t(5.68e27f));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgtf", bi32_t(1), bf32_t(5.68e27f), bf32_t(5.67e27f));

	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgtd", bi32_t(0), bf64_t(0.0), bf64_t(0.0));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgtd", bi32_t(1), bf64_t(1.0), bf64_t(0.0));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgtd", bi32_t(1), bf64_t(2.0), bf64_t(-2.0));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgtd", bi32_t(0), bf64_t(5.67e27), bf64_t(5.68e27));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgtd", bi32_t(1), bf64_t(5.68e27), bf64_t(5.67e27));

	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgei", bi32_t(1), bi32_t(0), bi32_t(0));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgei", bi32_t(1), bi32_t(1), bi32_t(0));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgei", bi32_t(1), bi32_t(2), bi32_t(-2));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgei", bi32_t(0), bi32_t(2398), bi32_t(8382));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgei", bi32_t(1), bi32_t(8382), bi32_t(2398));

	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgeui", bi32_t(1), bu32_t(0), bu32_t(0));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgeui", bi32_t(1), bu32_t(1), bu32_t(0));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgeui", bi32_t(0), bu32_t(2), bi32_t(-2));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgeui", bi32_t(0), bu32_t(2398), bu32_t(8382));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgeui", bi32_t(1), bu32_t(8382), bu32_t(2398));

	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgel", bi32_t(1), bi64_t(0), bi64_t(0));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgel", bi32_t(1), bi64_t(1), bi64_t(0));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgel", bi32_t(1), bi64_t(2), bi64_t(-2));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgel", bi32_t(0), bi64_t(2398) << 31, bi64_t(8382) << 31);
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgel", bi32_t(1), bi64_t(8382) << 31, bi64_t(2938) << 31);

	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgeul", bi32_t(1), bu64_t(0), bu64_t(0));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgeul", bi32_t(1), bu64_t(1), bu64_t(0));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgeul", bi32_t(0), bu64_t(2), bi64_t(-2));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgeul", bi32_t(0), bu64_t(2398) << 31, bu64_t(8382) << 31);
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgeul", bi32_t(1), bu64_t(8382) << 31, bu64_t(2938) << 31);

	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgef", bi32_t(1), bf32_t(0.0f), bf32_t(0.0f));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgef", bi32_t(1), bf32_t(1.0f), bf32_t(0.0f));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgef", bi32_t(1), bf32_t(2.0f), bf32_t(-2.0f));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgef", bi32_t(0), bf32_t(5.67e27f), bf32_t(5.68e27f));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgef", bi32_t(1), bf32_t(5.68e27f), bf32_t(5.67e27f));

	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpged", bi32_t(1), bf64_t(0.0), bf64_t(0.0));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpged", bi32_t(1), bf64_t(1.0), bf64_t(0.0));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpged", bi32_t(1), bf64_t(2.0), bf64_t(-2.0));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpged", bi32_t(0), bf64_t(5.67e27), bf64_t(5.68e27));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpged", bi32_t(1), bf64_t(5.68e27), bf64_t(5.67e27));

	return true;
}


DEFINE_VM_TEST(Branches, "scripts/vm_Branches.bond")
{
	using namespace Bond;

	VALIDATE_FUNCTION_CALL_2(UINT, "::And", bu32_t(0), bu8_t(0), bu8_t(0));
	VALIDATE_FUNCTION_CALL_2(UINT, "::And", bu32_t(0), bu8_t(0), bu8_t(1));
	VALIDATE_FUNCTION_CALL_2(UINT, "::And", bu32_t(0), bu8_t(1), bu8_t(0));
	VALIDATE_FUNCTION_CALL_2(UINT, "::And", bu32_t(1), bu8_t(1), bu8_t(1));

	VALIDATE_FUNCTION_CALL_2(UINT, "::Or", bu32_t(0), bu8_t(0), bu8_t(0));
	VALIDATE_FUNCTION_CALL_2(UINT, "::Or", bu32_t(1), bu8_t(0), bu8_t(1));
	VALIDATE_FUNCTION_CALL_2(UINT, "::Or", bu32_t(1), bu8_t(1), bu8_t(0));
	VALIDATE_FUNCTION_CALL_2(UINT, "::Or", bu32_t(1), bu8_t(1), bu8_t(1));

	VALIDATE_FUNCTION_CALL_1(INT, "::AndShortCircuit", bi32_t(4), bu8_t(0));
	VALIDATE_FUNCTION_CALL_1(INT, "::AndShortCircuit", bi32_t(5), bu8_t(1));

	VALIDATE_FUNCTION_CALL_1(INT, "::OrShortCircuit", bi32_t(5), bu8_t(0));
	VALIDATE_FUNCTION_CALL_1(INT, "::OrShortCircuit", bi32_t(4), bu8_t(1));

	VALIDATE_FUNCTION_CALL_1(INT, "::Ternary", bi32_t(5), bu8_t(0));
	VALIDATE_FUNCTION_CALL_1(INT, "::Ternary", bi32_t(4), bu8_t(1));

	VALIDATE_FUNCTION_CALL_1(INT, "::If", bi32_t(5), bu8_t(0));
	VALIDATE_FUNCTION_CALL_1(INT, "::If", bi32_t(4), bu8_t(1));

	VALIDATE_FUNCTION_CALL_1(INT, "::IfElse", bi32_t(5), bu8_t(0));
	VALIDATE_FUNCTION_CALL_1(INT, "::IfElse", bi32_t(4), bu8_t(1));

	VALIDATE_FUNCTION_CALL_1(INT, "::LookupSwitchDefault", bi32_t(8), bu8_t(0));
	VALIDATE_FUNCTION_CALL_1(INT, "::LookupSwitchDefault", bi32_t(5), bu8_t(1));
	VALIDATE_FUNCTION_CALL_1(INT, "::LookupSwitchDefault", bi32_t(8), bu8_t(2));
	VALIDATE_FUNCTION_CALL_1(INT, "::LookupSwitchDefault", bi32_t(6), bu8_t(3));
	VALIDATE_FUNCTION_CALL_1(INT, "::LookupSwitchDefault", bi32_t(8), bu8_t(4));
	VALIDATE_FUNCTION_CALL_1(INT, "::LookupSwitchDefault", bi32_t(8), bu8_t(99));
	VALIDATE_FUNCTION_CALL_1(INT, "::LookupSwitchDefault", bi32_t(7), bu8_t(100));
	VALIDATE_FUNCTION_CALL_1(INT, "::LookupSwitchDefault", bi32_t(8), bu8_t(101));

	VALIDATE_FUNCTION_CALL_1(INT, "::LookupSwitchNoDefault", bi32_t(4), bu8_t(0));
	VALIDATE_FUNCTION_CALL_1(INT, "::LookupSwitchNoDefault", bi32_t(5), bu8_t(1));
	VALIDATE_FUNCTION_CALL_1(INT, "::LookupSwitchNoDefault", bi32_t(4), bu8_t(2));
	VALIDATE_FUNCTION_CALL_1(INT, "::LookupSwitchNoDefault", bi32_t(6), bu8_t(3));
	VALIDATE_FUNCTION_CALL_1(INT, "::LookupSwitchNoDefault", bi32_t(4), bu8_t(4));
	VALIDATE_FUNCTION_CALL_1(INT, "::LookupSwitchNoDefault", bi32_t(4), bu8_t(99));
	VALIDATE_FUNCTION_CALL_1(INT, "::LookupSwitchNoDefault", bi32_t(7), bu8_t(100));
	VALIDATE_FUNCTION_CALL_1(INT, "::LookupSwitchNoDefault", bi32_t(4), bu8_t(101));

	VALIDATE_FUNCTION_CALL_2(INT, "::ElseIf", bi32_t(6), bu8_t(0), bu8_t(0));
	VALIDATE_FUNCTION_CALL_2(INT, "::ElseIf", bi32_t(5), bu8_t(0), bu8_t(1));
	VALIDATE_FUNCTION_CALL_2(INT, "::ElseIf", bi32_t(4), bu8_t(1), bu8_t(0));
	VALIDATE_FUNCTION_CALL_2(INT, "::ElseIf", bi32_t(4), bu8_t(1), bu8_t(1));

	VALIDATE_FUNCTION_CALL_2(INT, "::NestedIf", bi32_t(7), bu8_t(0), bu8_t(0));
	VALIDATE_FUNCTION_CALL_2(INT, "::NestedIf", bi32_t(6), bu8_t(0), bu8_t(1));
	VALIDATE_FUNCTION_CALL_2(INT, "::NestedIf", bi32_t(5), bu8_t(1), bu8_t(0));
	VALIDATE_FUNCTION_CALL_2(INT, "::NestedIf", bi32_t(4), bu8_t(1), bu8_t(1));

	VALIDATE_FUNCTION_CALL_1(INT, "::While", bi32_t(5), bi32_t(5));
	VALIDATE_FUNCTION_CALL_1(INT, "::For", bi32_t(5), bi32_t(5));

	return true;
}


#define TEST_ITEMS                              \
  TEST_ITEM(Constants)                          \
  TEST_ITEM(StackOperations)                    \
  TEST_ITEM(MemoryOperations)                   \
  TEST_ITEM(TypeConversions)                    \
  TEST_ITEM(BinaryOperators)                    \
  TEST_ITEM(UnaryOperators)                     \
  TEST_ITEM(ComparisonOperators)                \
  TEST_ITEM(Branches)                           \

RUN_TESTS(VM, TEST_ITEMS)