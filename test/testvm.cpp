#include "framework/testvmframework.h"
#include <cstring>

struct Char3
{
	int8_t x;
	int8_t y;
	int8_t z;
	int8_t padding;
};


struct Char3Pair
{
	Char3 first;
	Char3 second;
};


struct Vector3
{
	int32_t x;
	int32_t y;
	int32_t z;
};


DEFINE_VM_TEST(Constants, "scripts/vm_Constants.bond")
{
	using namespace Bond;
	VALIDATE_FUNCTION_CALL_0(INT, "::Constc", int32_t(-7));
	VALIDATE_FUNCTION_CALL_0(UINT, "::Constuc", int32_t(130));
	VALIDATE_FUNCTION_CALL_0(INT, "::Consts", int32_t(-345));
	VALIDATE_FUNCTION_CALL_0(UINT, "::Constus", int32_t(40000));
	VALIDATE_FUNCTION_CALL_0(INT, "::Const32", int32_t(-34567));
	VALIDATE_FUNCTION_CALL_0(LONG, "::Const64", int64_t(-34567));
	VALIDATE_FUNCTION_CALL_0(INT, "::Consti_n2", int32_t(-2));
	VALIDATE_FUNCTION_CALL_0(INT, "::Consti_n1", int32_t(-1));
	VALIDATE_FUNCTION_CALL_0(INT, "::Consti_0", int32_t(0));
	VALIDATE_FUNCTION_CALL_0(INT, "::Consti_1", int32_t(1));
	VALIDATE_FUNCTION_CALL_0(INT, "::Consti_2", int32_t(2));
	VALIDATE_FUNCTION_CALL_0(INT, "::Consti_3", int32_t(3));
	VALIDATE_FUNCTION_CALL_0(INT, "::Consti_4", int32_t(4));
	VALIDATE_FUNCTION_CALL_0(INT, "::Consti_8", int32_t(8));
	VALIDATE_FUNCTION_CALL_0(LONG, "::Constl_n2", int64_t(-2));
	VALIDATE_FUNCTION_CALL_0(LONG, "::Constl_n1", int64_t(-1));
	VALIDATE_FUNCTION_CALL_0(LONG, "::Constl_0", int64_t(0));
	VALIDATE_FUNCTION_CALL_0(LONG, "::Constl_1", int64_t(1));
	VALIDATE_FUNCTION_CALL_0(LONG, "::Constl_2", int64_t(2));
	VALIDATE_FUNCTION_CALL_0(LONG, "::Constl_3", int64_t(3));
	VALIDATE_FUNCTION_CALL_0(LONG, "::Constl_4", int64_t(4));
	VALIDATE_FUNCTION_CALL_0(LONG, "::Constl_8", int64_t(8));
	VALIDATE_FUNCTION_CALL_0(FLOAT, "::Constf_n2", -2.0f);
	VALIDATE_FUNCTION_CALL_0(FLOAT, "::Constf_n1", -1.0f);
	VALIDATE_FUNCTION_CALL_0(FLOAT, "::Constf_nh", -0.5f);
	VALIDATE_FUNCTION_CALL_0(FLOAT, "::Constf_0", 0.0f);
	VALIDATE_FUNCTION_CALL_0(FLOAT, "::Constf_h",  0.5f);
	VALIDATE_FUNCTION_CALL_0(FLOAT, "::Constf_1", 1.0f);
	VALIDATE_FUNCTION_CALL_0(FLOAT, "::Constf_2", 2.0f);
	VALIDATE_FUNCTION_CALL_0(DOUBLE, "::Constd_n2", -2.0);
	VALIDATE_FUNCTION_CALL_0(DOUBLE, "::Constd_n1", -1.0);
	VALIDATE_FUNCTION_CALL_0(DOUBLE, "::Constd_nh", -0.5);
	VALIDATE_FUNCTION_CALL_0(DOUBLE, "::Constd_0", 0.0);
	VALIDATE_FUNCTION_CALL_0(DOUBLE, "::Constd_h",  0.5);
	VALIDATE_FUNCTION_CALL_0(DOUBLE, "::Constd_1", 1.0);
	VALIDATE_FUNCTION_CALL_0(DOUBLE, "::Constd_2", 2.0);

	return true;
}


DEFINE_VM_TEST(StackOperations, "scripts/vm_StackOperations.bond")
{
	using namespace Bond;
	VALIDATE_FUNCTION_CALL_5(INT, "::PushcPopc", int8_t(-123), int8_t(0), int8_t(0), int8_t(0), int8_t(0), int8_t(-123));
	VALIDATE_FUNCTION_CALL_5(UINT, "::PushucPopc", uint8_t(234), uint8_t(0), uint8_t(0), uint8_t(0), uint8_t(0), uint8_t(234));
	VALIDATE_FUNCTION_CALL_5(INT, "::PushsPops", int16_t(-1234), int16_t(0), int16_t(0), int16_t(0), int16_t(0), int16_t(-1234));
	VALIDATE_FUNCTION_CALL_5(UINT, "::PushusPops", uint16_t(60000), uint16_t(0), uint16_t(0), uint16_t(0), uint16_t(0), uint16_t(60000));
	VALIDATE_FUNCTION_CALL_5(INT, "::Push32Pop32", int32_t(-56789), int32_t(0), int32_t(0), int32_t(0), int32_t(0), int32_t(-56789));
	VALIDATE_FUNCTION_CALL_5(LONG, "::Push64Pop64", int64_t(-567890), int64_t(0), int64_t(0), int64_t(0), int64_t(0), int64_t(-567890));

	return true;
}


DEFINE_VM_TEST(MemoryOperations, "scripts/vm_MemoryOperations.bond")
{
	using namespace Bond;
	int8_t chars[] = {0, -123, 0, 0, 0};
	VALIDATE_FUNCTION_CALL_2(INT, "::LoadcStorec", int32_t(-123), chars + 1, chars + 3);
	ASSERT_FORMAT(chars[1] == chars[3],
		("Expected %" BOND_PRId32 ", but was %" BOND_PRId32 ".", chars[1], chars[3]));
	ASSERT_MESSAGE((chars[0] == 0) && (chars[2] == 0) && (chars[4] == 0),
		"Memory stomp by LoadcStorec() detected.");

	uint8_t uchars[] = {0, 0xff, 0, 0, 0};
	VALIDATE_FUNCTION_CALL_2(UINT, "::LoaducStorec", uint32_t(0xff), uchars + 1, uchars + 3);
	ASSERT_FORMAT(uchars[1] == uchars[3],
		("Expected %" BOND_PRIu32 ", but was %" BOND_PRIu32 ".", uchars[1], uchars[3]));
	ASSERT_MESSAGE((uchars[0] == 0) && (uchars[2] == 0) && (uchars[4] == 0),
		"Memory stomp by LoaducStorec() detected.");

	int16_t shorts[] = {0, -234, 0, 0, 0};
	VALIDATE_FUNCTION_CALL_2(INT, "::LoadsStores", int32_t(-234), shorts + 1, shorts + 3);
	ASSERT_FORMAT(shorts[1] == shorts[3],
		("Expected %" BOND_PRId32 ", but was %" BOND_PRId32 ".", shorts[1], shorts[3]));
	ASSERT_MESSAGE((shorts[0] == 0) && (shorts[2] == 0) && (shorts[4] == 0),
		"Memory stomp by LoadsStores() detected.");

	uint16_t ushorts[] = {0, 0xffff, 0, 0, 0};
	VALIDATE_FUNCTION_CALL_2(UINT, "::LoadusStores", int32_t(0xffff), ushorts + 1, ushorts + 3);
	ASSERT_FORMAT(ushorts[1] == ushorts[3],
		("Expected %" BOND_PRIu32 ", but was %" BOND_PRIu32 ".", ushorts[1], ushorts[3]));
	ASSERT_MESSAGE((ushorts[0] == 0) && (ushorts[2] == 0) && (ushorts[4] == 0),
		"Memory stomp by LoadusStores() detected.");

	int32_t ints[] = {0, -345, 0, 0, 0};
	VALIDATE_FUNCTION_CALL_2(INT, "::Load32Store32", int32_t(-345), ints + 1, ints + 3);
	ASSERT_FORMAT(ints[1] == ints[3],
		("Expected %" BOND_PRId32 ", but was %" BOND_PRId32 ".", ints[1], ints[3]));
	ASSERT_MESSAGE((ints[0] == 0) && (ints[2] == 0) && (ints[4] == 0),
		"Memory stomp by Load32Store32() detected.");

	int64_t longs[] = {0, -456, 0, 0, 0};
	VALIDATE_FUNCTION_CALL_2(LONG, "::Load64Store64", int64_t(-456), longs + 1, longs + 3);
	ASSERT_FORMAT(longs[1] == longs[3],
		("Expected %" BOND_PRId64 ", but was %" BOND_PRId64 ".", longs[1], longs[3]));
	ASSERT_MESSAGE((longs[0] == 0) && (longs[2] == 0) && (longs[4] == 0),
		"Memory stomp by Load64Store64() detected.");

	VALIDATE_FUNCTION_CALL_1(INT, "::Loadfp", int32_t(-567), int32_t(-567));

	return true;
}


DEFINE_VM_TEST(TypeConversions, "scripts/vm_TypeConversions.bond")
{
	using namespace Bond;
	VALIDATE_FUNCTION_CALL_1(INT, "::Itoc", int32_t(int8_t(-17)), int32_t(-17));
	VALIDATE_FUNCTION_CALL_1(INT, "::Itoc", int32_t(int8_t(0xabc)), int32_t(0xabc));
	VALIDATE_FUNCTION_CALL_1(INT, "::Itoc", int32_t(int8_t(0x864)), int32_t(0x864));

	VALIDATE_FUNCTION_CALL_1(UINT, "::Uitouc", uint32_t(uint8_t(17)), uint32_t(17));
	VALIDATE_FUNCTION_CALL_1(UINT, "::Uitouc", uint32_t(uint8_t(0xabc)), uint32_t(0xabc));
	VALIDATE_FUNCTION_CALL_1(UINT, "::Uitouc", uint32_t(uint8_t(0x864)), uint32_t(0x864));

	VALIDATE_FUNCTION_CALL_1(INT, "::Itos", int32_t(int16_t(-17)), int32_t(-17));
	VALIDATE_FUNCTION_CALL_1(INT, "::Itos", int32_t(int16_t(0xabc)), int32_t(0xabc));
	VALIDATE_FUNCTION_CALL_1(INT, "::Itos", int32_t(int16_t(0xabcde)), int32_t(0xabcde));
	VALIDATE_FUNCTION_CALL_1(INT, "::Itos", int32_t(int16_t(0x987654)), int32_t(0x987654));

	VALIDATE_FUNCTION_CALL_1(UINT, "::Uitous", uint32_t(uint16_t(17)), uint32_t(17));
	VALIDATE_FUNCTION_CALL_1(UINT, "::Uitous", uint32_t(uint16_t(0xabc)), uint32_t(0xabc));
	VALIDATE_FUNCTION_CALL_1(UINT, "::Uitous", uint32_t(uint16_t(0xabcde)), uint32_t(0xabcde));
	VALIDATE_FUNCTION_CALL_1(UINT, "::Uitous", uint32_t(uint16_t(0x987654)), uint32_t(0x987654));

	VALIDATE_FUNCTION_CALL_1(LONG, "::Itol", int64_t(int32_t(-4567)), int32_t(-4567));
	VALIDATE_FUNCTION_CALL_1(LONG, "::Itol", int64_t(int32_t(4567)), int32_t(4567));

	VALIDATE_FUNCTION_CALL_1(ULONG, "::Uitoul", uint64_t(uint32_t(7654)), uint32_t(7654));

	VALIDATE_FUNCTION_CALL_1(FLOAT, "::Itof", float(int32_t(0)), int32_t(0));
	VALIDATE_FUNCTION_CALL_1(FLOAT, "::Itof", float(int32_t(1)), int32_t(1));
	VALIDATE_FUNCTION_CALL_1(FLOAT, "::Itof", float(int32_t(2)), int32_t(2));
	VALIDATE_FUNCTION_CALL_1(FLOAT, "::Itof", float(int32_t(-1)), int32_t(-1));
	VALIDATE_FUNCTION_CALL_1(FLOAT, "::Itof", float(int32_t(-2)), int32_t(-2));
	VALIDATE_FUNCTION_CALL_1(FLOAT, "::Itof", float(int32_t(-3000)), int32_t(-3000));

	VALIDATE_FUNCTION_CALL_1(FLOAT, "::Uitof", float(uint32_t(0)), uint32_t(0));
	VALIDATE_FUNCTION_CALL_1(FLOAT, "::Uitof", float(uint32_t(1)), uint32_t(1));
	VALIDATE_FUNCTION_CALL_1(FLOAT, "::Uitof", float(uint32_t(2)), uint32_t(2));
	VALIDATE_FUNCTION_CALL_1(FLOAT, "::Uitof", float(uint32_t(3000)), uint32_t(3000));

	VALIDATE_FUNCTION_CALL_1(DOUBLE, "::Itod", double(int32_t(0)), int32_t(0));
	VALIDATE_FUNCTION_CALL_1(DOUBLE, "::Itod", double(int32_t(1)), int32_t(1));
	VALIDATE_FUNCTION_CALL_1(DOUBLE, "::Itod", double(int32_t(2)), int32_t(2));
	VALIDATE_FUNCTION_CALL_1(DOUBLE, "::Itod", double(int32_t(-1)), int32_t(-1));
	VALIDATE_FUNCTION_CALL_1(DOUBLE, "::Itod", double(int32_t(-2)), int32_t(-2));
	VALIDATE_FUNCTION_CALL_1(DOUBLE, "::Itod", double(int32_t(-3000)), int32_t(-3000));

	VALIDATE_FUNCTION_CALL_1(DOUBLE, "::Uitod", double(uint32_t(0)), uint32_t(0));
	VALIDATE_FUNCTION_CALL_1(DOUBLE, "::Uitod", double(uint32_t(1)), uint32_t(1));
	VALIDATE_FUNCTION_CALL_1(DOUBLE, "::Uitod", double(uint32_t(2)), uint32_t(2));
	VALIDATE_FUNCTION_CALL_1(DOUBLE, "::Uitod", double(uint32_t(3000)), uint32_t(3000));

	VALIDATE_FUNCTION_CALL_1(INT, "::Ltoi", int32_t(int64_t(-8745)), int64_t(-8745));
	VALIDATE_FUNCTION_CALL_1(INT, "::Ltoi", int32_t(int64_t(0xabcdef) << 16), int64_t(0xabcdef) << 16);
	VALIDATE_FUNCTION_CALL_1(INT, "::Ltoi", int32_t(int64_t(8745)), int64_t(8745));

	VALIDATE_FUNCTION_CALL_1(FLOAT, "::Ltof", float(int64_t(0)), int64_t(0));
	VALIDATE_FUNCTION_CALL_1(FLOAT, "::Ltof", float(int64_t(1)), int64_t(1));
	VALIDATE_FUNCTION_CALL_1(FLOAT, "::Ltof", float(int64_t(2)), int64_t(2));
	VALIDATE_FUNCTION_CALL_1(FLOAT, "::Ltof", float(int64_t(-1)), int64_t(-1));
	VALIDATE_FUNCTION_CALL_1(FLOAT, "::Ltof", float(int64_t(-2)), int64_t(-2));
	VALIDATE_FUNCTION_CALL_1(FLOAT, "::Ltof", float(int64_t(-3000)), int64_t(-3000));

	VALIDATE_FUNCTION_CALL_1(FLOAT, "::Ultof", float(uint64_t(0)), uint64_t(0));
	VALIDATE_FUNCTION_CALL_1(FLOAT, "::Ultof", float(uint64_t(1)), uint64_t(1));
	VALIDATE_FUNCTION_CALL_1(FLOAT, "::Ultof", float(uint64_t(2)), uint64_t(2));
	VALIDATE_FUNCTION_CALL_1(FLOAT, "::Ultof", float(uint64_t(3000)), uint64_t(3000));

	VALIDATE_FUNCTION_CALL_1(DOUBLE, "::Ltod", double(int64_t(0)), int64_t(0));
	VALIDATE_FUNCTION_CALL_1(DOUBLE, "::Ltod", double(int64_t(1)), int64_t(1));
	VALIDATE_FUNCTION_CALL_1(DOUBLE, "::Ltod", double(int64_t(2)), int64_t(2));
	VALIDATE_FUNCTION_CALL_1(DOUBLE, "::Ltod", double(int64_t(-1)), int64_t(-1));
	VALIDATE_FUNCTION_CALL_1(DOUBLE, "::Ltod", double(int64_t(-2)), int64_t(-2));
	VALIDATE_FUNCTION_CALL_1(DOUBLE, "::Ltod", double(int64_t(-3000)), int64_t(-3000));

	VALIDATE_FUNCTION_CALL_1(DOUBLE, "::Ultod", double(uint64_t(0)), uint64_t(0));
	VALIDATE_FUNCTION_CALL_1(DOUBLE, "::Ultod", double(uint64_t(1)), uint64_t(1));
	VALIDATE_FUNCTION_CALL_1(DOUBLE, "::Ultod", double(uint64_t(2)), uint64_t(2));
	VALIDATE_FUNCTION_CALL_1(DOUBLE, "::Ultod", double(uint64_t(3000)), uint64_t(3000));

	VALIDATE_FUNCTION_CALL_1(INT, "::Ftoi", int32_t(0.0f), 0.0f);
	VALIDATE_FUNCTION_CALL_1(INT, "::Ftoi", int32_t(0.1f), 0.1f);
	VALIDATE_FUNCTION_CALL_1(INT, "::Ftoi", int32_t(1.1f), 1.1f);
	VALIDATE_FUNCTION_CALL_1(INT, "::Ftoi", int32_t(-0.1f), -0.1f);
	VALIDATE_FUNCTION_CALL_1(INT, "::Ftoi", int32_t(-1.1f), -1.1f);
	VALIDATE_FUNCTION_CALL_1(INT, "::Ftoi", int32_t(1.23e9f), 1.23e9f);

	VALIDATE_FUNCTION_CALL_1(UINT, "::Ftoui", uint32_t(0.0f), 0.0f);
	VALIDATE_FUNCTION_CALL_1(UINT, "::Ftoui", uint32_t(0.1f), 0.1f);
	VALIDATE_FUNCTION_CALL_1(UINT, "::Ftoui", uint32_t(1.1f), 1.1f);
	VALIDATE_FUNCTION_CALL_1(UINT, "::Ftoui", uint32_t(1.23e9f), 1.23e9f);

	VALIDATE_FUNCTION_CALL_1(LONG, "::Ftol", int64_t(0.0f), 0.0f);
	VALIDATE_FUNCTION_CALL_1(LONG, "::Ftol", int64_t(0.1f), 0.1f);
	VALIDATE_FUNCTION_CALL_1(LONG, "::Ftol", int64_t(1.1f), 1.1f);
	VALIDATE_FUNCTION_CALL_1(LONG, "::Ftol", int64_t(-0.1f), -0.1f);
	VALIDATE_FUNCTION_CALL_1(LONG, "::Ftol", int64_t(-1.1f), -1.1f);
	VALIDATE_FUNCTION_CALL_1(LONG, "::Ftol", int64_t(1.23e10f), 1.23e10f);

	VALIDATE_FUNCTION_CALL_1(ULONG, "::Ftoul", uint64_t(0.0f), 0.0f);
	VALIDATE_FUNCTION_CALL_1(ULONG, "::Ftoul", uint64_t(0.1f), 0.1f);
	VALIDATE_FUNCTION_CALL_1(ULONG, "::Ftoul", uint64_t(1.1f), 1.1f);
	VALIDATE_FUNCTION_CALL_1(ULONG, "::Ftoul", uint64_t(1.23e10f), 1.23e10f);

	VALIDATE_FUNCTION_CALL_1(DOUBLE, "::Ftod", 0.0, 0.0f);
	VALIDATE_FUNCTION_CALL_1(DOUBLE, "::Ftod", 0.1, 0.1f);
	VALIDATE_FUNCTION_CALL_1(DOUBLE, "::Ftod", 1.1, 1.1f);
	VALIDATE_FUNCTION_CALL_1(DOUBLE, "::Ftod", -0.1, -0.1f);
	VALIDATE_FUNCTION_CALL_1(DOUBLE, "::Ftod", -1.1, -1.1f);
	VALIDATE_FUNCTION_CALL_1(DOUBLE, "::Ftod", 1.23e10, 1.23e10f);

	VALIDATE_FUNCTION_CALL_1(INT, "::Dtoi", int32_t(0.0), 0.0);
	VALIDATE_FUNCTION_CALL_1(INT, "::Dtoi", int32_t(0.1), 0.1);
	VALIDATE_FUNCTION_CALL_1(INT, "::Dtoi", int32_t(1.1), 1.1);
	VALIDATE_FUNCTION_CALL_1(INT, "::Dtoi", int32_t(-0.1), -0.1);
	VALIDATE_FUNCTION_CALL_1(INT, "::Dtoi", int32_t(-1.1), -1.1);
	VALIDATE_FUNCTION_CALL_1(INT, "::Dtoi", int32_t(1.23e9), 1.23e9);

	VALIDATE_FUNCTION_CALL_1(UINT, "::Dtoui", uint32_t(0.0), 0.0);
	VALIDATE_FUNCTION_CALL_1(UINT, "::Dtoui", uint32_t(0.1), 0.1);
	VALIDATE_FUNCTION_CALL_1(UINT, "::Dtoui", uint32_t(1.1), 1.1);
	VALIDATE_FUNCTION_CALL_1(UINT, "::Dtoui", uint32_t(1.23e9), 1.23e9);

	VALIDATE_FUNCTION_CALL_1(LONG, "::Dtol", int64_t(0.0), 0.0);
	VALIDATE_FUNCTION_CALL_1(LONG, "::Dtol", int64_t(0.1), 0.1);
	VALIDATE_FUNCTION_CALL_1(LONG, "::Dtol", int64_t(1.1), 1.1);
	VALIDATE_FUNCTION_CALL_1(LONG, "::Dtol", int64_t(-0.1), -0.1);
	VALIDATE_FUNCTION_CALL_1(LONG, "::Dtol", int64_t(-1.1), -1.1);
	VALIDATE_FUNCTION_CALL_1(LONG, "::Dtol", int64_t(1.23e10), 1.23e10);

	VALIDATE_FUNCTION_CALL_1(ULONG, "::Dtoul", uint64_t(0.0), 0.0);
	VALIDATE_FUNCTION_CALL_1(ULONG, "::Dtoul", uint64_t(0.1), 0.1);
	VALIDATE_FUNCTION_CALL_1(ULONG, "::Dtoul", uint64_t(1.1), 1.1);
	VALIDATE_FUNCTION_CALL_1(ULONG, "::Dtoul", uint64_t(1.23e10), 1.23e10);

	VALIDATE_FUNCTION_CALL_1(FLOAT, "::Dtof", 0.0f, 0.0);
	VALIDATE_FUNCTION_CALL_1(FLOAT, "::Dtof", 0.1f, 0.1);
	VALIDATE_FUNCTION_CALL_1(FLOAT, "::Dtof", 1.1f, 1.1);
	VALIDATE_FUNCTION_CALL_1(FLOAT, "::Dtof", -0.1f, -0.1);
	VALIDATE_FUNCTION_CALL_1(FLOAT, "::Dtof", -1.1f, -1.1);
	VALIDATE_FUNCTION_CALL_1(FLOAT, "::Dtof", 1.23e10f, 1.23e10);

	return true;
}


DEFINE_VM_TEST(BinaryOperators, "scripts/vm_BinaryOperators.bond")
{
	using namespace Bond;
	VALIDATE_FUNCTION_CALL_2(INT, "::Addi", int32_t(-65) + int32_t(13), int32_t(-65), int32_t(13));
	VALIDATE_FUNCTION_CALL_2(UINT, "::Addi", uint32_t(65) + uint32_t(13), uint32_t(65), uint32_t(13));
	VALIDATE_FUNCTION_CALL_2(LONG, "::Addl", (int64_t(-65) << 40) + (int64_t(13) << 40), (int64_t(-65) << 40), (int64_t(13) << 40));
	VALIDATE_FUNCTION_CALL_2(ULONG, "::Addl", (uint64_t(65) << 40) + (uint64_t(13) << 40), (uint64_t(65) << 40), (uint64_t(13) << 40));
	VALIDATE_FUNCTION_CALL_2(FLOAT, "::Addf", -65.0f + 13.0f, -65.0f, 13.0f);
	VALIDATE_FUNCTION_CALL_2(FLOAT, "::Addf", 1e20f + 0.5e20f, 1e20f, 0.5e20f);
	VALIDATE_FUNCTION_CALL_2(DOUBLE, "::Addd", -65.0 + 13.0, -65.0, 13.0);
	VALIDATE_FUNCTION_CALL_2(DOUBLE, "::Addd", 1e40 + 0.5e40, 1e40, 0.5e40);

	VALIDATE_FUNCTION_CALL_2(INT, "::Subi", int32_t(-43) - int32_t(12), int32_t(-43), int32_t(12));
	VALIDATE_FUNCTION_CALL_2(UINT, "::Subi", uint32_t(43) - uint32_t(12), uint32_t(43), uint32_t(12));
	VALIDATE_FUNCTION_CALL_2(LONG, "::Subl", (int64_t(-43) << 40) - (int64_t(12) << 40), (int64_t(-43) << 40), (int64_t(12) << 40));
	VALIDATE_FUNCTION_CALL_2(ULONG, "::Subl", (uint64_t(43) << 40) - (uint64_t(12) << 40), (uint64_t(43) << 40), (uint64_t(12) << 40));
	VALIDATE_FUNCTION_CALL_2(FLOAT, "::Subf", -43.0f - 12.0f, -43.0f, 12.0f);
	VALIDATE_FUNCTION_CALL_2(FLOAT, "::Subf", 1e20f - 0.5e20f, 1e20f, 0.5e20f);
	VALIDATE_FUNCTION_CALL_2(DOUBLE, "::Subd", -43.0 - 12.0, -43.0, 12.0);
	VALIDATE_FUNCTION_CALL_2(DOUBLE, "::Subd", 1e40 - 0.5e40, 1e40, 0.5e40);

	VALIDATE_FUNCTION_CALL_2(INT, "::Muli", int32_t(-58) * int32_t(0), int32_t(-58), int32_t(0));
	VALIDATE_FUNCTION_CALL_2(INT, "::Muli", int32_t(-58) * int32_t(17), int32_t(-58), int32_t(17));
	VALIDATE_FUNCTION_CALL_2(UINT, "::Mului", uint32_t(58) * uint32_t(17), uint32_t(58), uint32_t(17));
	VALIDATE_FUNCTION_CALL_2(LONG, "::Mull", (int64_t(-58) << 40) * int64_t(17), (int64_t(-58) << 40), int64_t(17));
	VALIDATE_FUNCTION_CALL_2(ULONG, "::Mulul", (uint64_t(58) << 40) * uint64_t(17), (uint64_t(58) << 40), uint64_t(17));
	VALIDATE_FUNCTION_CALL_2(FLOAT, "::Mulf", 0.0f * 84.3f, -0.0f, 84.3f);
	VALIDATE_FUNCTION_CALL_2(FLOAT, "::Mulf", -98.1f * 84.3f, -98.1f, 84.3f);
	VALIDATE_FUNCTION_CALL_2(FLOAT, "::Mulf", 1e20f * 3.3e10f, 1e20f, 3.3e10f);
	VALIDATE_FUNCTION_CALL_2(DOUBLE, "::Muld", 0.0 * 84.3, -0.0, 84.3);
	VALIDATE_FUNCTION_CALL_2(DOUBLE, "::Muld", -98.1 * 84.3, -98.1, 84.3);
	VALIDATE_FUNCTION_CALL_2(DOUBLE, "::Muld", 1e40 * 3.3e20, 1e40, 3.3e20);

	VALIDATE_FUNCTION_CALL_2(INT, "::Divi", int32_t(0) / int32_t(3), int32_t(0), int32_t(3));
	VALIDATE_FUNCTION_CALL_2(INT, "::Divi", int32_t(-10) / int32_t(3), int32_t(-10), int32_t(3));
	VALIDATE_FUNCTION_CALL_2(UINT, "::Divui", Bond::BOND_UINT_MAX / uint32_t(5678), BOND_UINT_MAX, uint32_t(5678));
	VALIDATE_FUNCTION_CALL_2(LONG, "::Divl", Bond::BOND_LONG_MIN / int64_t(5678), BOND_LONG_MIN, int64_t(5678));
	VALIDATE_FUNCTION_CALL_2(ULONG, "::Divul", Bond::BOND_ULONG_MAX / uint64_t(5678), BOND_ULONG_MAX, uint64_t(5678));
	VALIDATE_FUNCTION_CALL_2(FLOAT, "::Divf", 0.0f / 84.3f, -0.0f, 84.3f);
	VALIDATE_FUNCTION_CALL_2(FLOAT, "::Divf", -98.1f / 84.3f, -98.1f, 84.3f);
	VALIDATE_FUNCTION_CALL_2(FLOAT, "::Divf", 1e20f / 3.3e10f, 1e20f, 3.3e10f);
	VALIDATE_FUNCTION_CALL_2(DOUBLE, "::Divd", 0.0 / 84.3, -0.0, 84.3);
	VALIDATE_FUNCTION_CALL_2(DOUBLE, "::Divd", -98.1 / 84.3, -98.1, 84.3);
	VALIDATE_FUNCTION_CALL_2(DOUBLE, "::Divd", 1e40 / 3.3e20, 1e40, 3.3e20);

	VALIDATE_FUNCTION_CALL_2(INT, "::Remi", int32_t(0) % int32_t(3), int32_t(0), int32_t(3));
	VALIDATE_FUNCTION_CALL_2(INT, "::Remi", int32_t(10) % int32_t(3), int32_t(10), int32_t(3));
	VALIDATE_FUNCTION_CALL_2(INT, "::Remi", int32_t(-10) % int32_t(3), int32_t(-10), int32_t(3));
	VALIDATE_FUNCTION_CALL_2(INT, "::Remi", int32_t(2352) % int32_t(24), int32_t(2352), int32_t(24));
	VALIDATE_FUNCTION_CALL_2(INT, "::Remui", uint32_t(0) % uint32_t(3), uint32_t(0), uint32_t(3));
	VALIDATE_FUNCTION_CALL_2(INT, "::Remui", uint32_t(10) % uint32_t(3), uint32_t(10), uint32_t(3));
	VALIDATE_FUNCTION_CALL_2(INT, "::Remui", uint32_t(2352) % uint32_t(24), uint32_t(2352), uint32_t(24));
	VALIDATE_FUNCTION_CALL_2(UINT, "::Remui", Bond::BOND_UINT_MAX % uint32_t(5678), BOND_UINT_MAX, uint32_t(5678));
	VALIDATE_FUNCTION_CALL_2(LONG, "::Reml", Bond::BOND_LONG_MIN % int64_t(5678), BOND_LONG_MIN, int64_t(5678));
	VALIDATE_FUNCTION_CALL_2(ULONG, "::Remul", Bond::BOND_ULONG_MAX % uint64_t(5678), BOND_ULONG_MAX, uint64_t(5678));

	VALIDATE_FUNCTION_CALL_2(INT, "::Lshi", int32_t(0) << int32_t(1), int32_t(0), int32_t(1));
	VALIDATE_FUNCTION_CALL_2(UINT, "::Lshi", uint32_t(1) << uint32_t(1), uint32_t(1), uint32_t(1));
	VALIDATE_FUNCTION_CALL_2(INT, "::Lshi", int32_t(1 & 1) << int32_t(31), int32_t(1), int32_t(31));
	VALIDATE_FUNCTION_CALL_2(INT, "::Lshi", int32_t(2 & 1) << int32_t(31), int32_t(2), int32_t(31));
	VALIDATE_FUNCTION_CALL_2(INT, "::Lshi", int32_t(3 & 1) << int32_t(31), int32_t(3), int32_t(31));
	VALIDATE_FUNCTION_CALL_2(INT, "::Lshi", int32_t(-3) << int32_t(16), int32_t(-3), int32_t(16));

	VALIDATE_FUNCTION_CALL_2(LONG, "::Lshl", int64_t(0) << int64_t(1), int64_t(0), int64_t(1));
	VALIDATE_FUNCTION_CALL_2(ULONG, "::Lshl", uint64_t(1) << uint64_t(1), uint64_t(1), uint64_t(1));
	VALIDATE_FUNCTION_CALL_2(LONG, "::Lshl", int64_t(1 & 1) << int64_t(63), int64_t(1), int64_t(63));
	VALIDATE_FUNCTION_CALL_2(LONG, "::Lshl", int64_t(2 & 1) << int64_t(63), int64_t(2), int64_t(63));
	VALIDATE_FUNCTION_CALL_2(LONG, "::Lshl", int64_t(3 & 1) << int64_t(63), int64_t(3), int64_t(63));
	VALIDATE_FUNCTION_CALL_2(LONG, "::Lshl", int64_t(-3) << int64_t(16), int64_t(-3), int64_t(16));

	VALIDATE_FUNCTION_CALL_2(INT, "::Rshi", int32_t(0) >> int32_t(1), int32_t(0), int32_t(1));
	VALIDATE_FUNCTION_CALL_2(UINT, "::Rshi", uint32_t(2) >> uint32_t(1), uint32_t(2), uint32_t(1));
	VALIDATE_FUNCTION_CALL_2(INT, "::Rshi", int32_t(1) >> int32_t(1), int32_t(1), int32_t(1));
	VALIDATE_FUNCTION_CALL_2(INT, "::Rshi", int32_t(2) >> int32_t(1), int32_t(2), int32_t(1));
	VALIDATE_FUNCTION_CALL_2(INT, "::Rshi", int32_t(3) >> int32_t(1), int32_t(3), int32_t(1));
	VALIDATE_FUNCTION_CALL_2(INT, "::Rshi", -(int32_t(1) << 20) >> int32_t(16), -(int32_t(1) << 20), int32_t(16));
	VALIDATE_FUNCTION_CALL_2(UINT, "::Rshui", uint32_t(-(int32_t(1) << 20)) >> uint32_t(16), uint32_t(-(int32_t(1) << 20)), uint32_t(16));

	VALIDATE_FUNCTION_CALL_2(LONG, "::Rshl", int64_t(0) >> int64_t(1), int64_t(0), int64_t(1));
	VALIDATE_FUNCTION_CALL_2(LONG, "::Rshl", int64_t(1) >> int64_t(1), int64_t(1), int64_t(1));
	VALIDATE_FUNCTION_CALL_2(LONG, "::Rshl", int64_t(2) >> int64_t(1), int64_t(2), int64_t(1));
	VALIDATE_FUNCTION_CALL_2(LONG, "::Rshl", int64_t(3) >> int64_t(1), int64_t(3), int64_t(1));
	VALIDATE_FUNCTION_CALL_2(LONG, "::Rshl", -(int64_t(1) << int64_t(20)) >> int64_t(16), -(int64_t(1) << int64_t(20)), int64_t(16));
	VALIDATE_FUNCTION_CALL_2(ULONG, "::Rshul", uint64_t(-(int64_t(1) << int64_t(20))) >> uint64_t(16), uint64_t(-(int64_t(1) << int64_t(20))), uint64_t(16));

	VALIDATE_FUNCTION_CALL_2(INT, "::Andi", int32_t(0xfff0) & int32_t(0xff), int32_t(0xfff0), int32_t(0xff));
	VALIDATE_FUNCTION_CALL_2(LONG, "::Andl", (int64_t(0xfff0) << 31) & (int64_t(0xff) << 31), (int64_t(0xfff0) << 31), (int64_t(0xff) << 31));
	VALIDATE_FUNCTION_CALL_2(INT, "::Ori", int32_t(0xfff0) | int32_t(0xff), int32_t(0xfff0), int32_t(0xff));
	VALIDATE_FUNCTION_CALL_2(LONG, "::Orl", (int64_t(0xfff0) << 31) | (int64_t(0xff) << 31), (int64_t(0xfff0) << 31), (int64_t(0xff) << 31));
	VALIDATE_FUNCTION_CALL_2(INT, "::Xori", int32_t(0xfff0) ^ int32_t(0xff), int32_t(0xfff0), int32_t(0xff));
	VALIDATE_FUNCTION_CALL_2(LONG, "::Xorl", (int64_t(0xfff0) << 31) ^ (int64_t(0xff) << 31), (int64_t(0xfff0) << 31), (int64_t(0xff) << 31));

	return true;
}


DEFINE_VM_TEST(UnaryOperators, "scripts/vm_UnaryOperators.bond")
{
	using namespace Bond;

	VALIDATE_FUNCTION_CALL_1(INT, "::Negi", int32_t(0), int32_t(0));
	VALIDATE_FUNCTION_CALL_1(INT, "::Negi", int32_t(-1), int32_t(1));
	VALIDATE_FUNCTION_CALL_1(INT, "::Negi", int32_t(1), int32_t(-1));
	VALIDATE_FUNCTION_CALL_1(INT, "::Negi", int32_t(-BOND_INT_MAX), int32_t(BOND_INT_MAX));
	VALIDATE_FUNCTION_CALL_1(INT, "::Negi", int32_t(BOND_INT_MAX), int32_t(-BOND_INT_MAX));

	VALIDATE_FUNCTION_CALL_1(LONG, "::Negl", int64_t(0), int64_t(0));
	VALIDATE_FUNCTION_CALL_1(LONG, "::Negl", int64_t(-1), int64_t(1));
	VALIDATE_FUNCTION_CALL_1(LONG, "::Negl", int64_t(1), int64_t(-1));
	VALIDATE_FUNCTION_CALL_1(LONG, "::Negl", int64_t(-BOND_LONG_MAX), int64_t(BOND_LONG_MAX));
	VALIDATE_FUNCTION_CALL_1(LONG, "::Negl", int64_t(BOND_LONG_MAX), int64_t(-BOND_LONG_MAX));

	VALIDATE_FUNCTION_CALL_1(FLOAT, "::Negf", 0.0f, 0.0f);
	VALIDATE_FUNCTION_CALL_1(FLOAT, "::Negf", -1.0f, 1.0f);
	VALIDATE_FUNCTION_CALL_1(FLOAT, "::Negf", 1.0f, -1.0f);
	VALIDATE_FUNCTION_CALL_1(FLOAT, "::Negf", -2.34e21f, 2.34e21f);
	VALIDATE_FUNCTION_CALL_1(FLOAT, "::Negf", 2.34e21f, -2.34e21f);

	VALIDATE_FUNCTION_CALL_1(DOUBLE, "::Negd", 0.0, 0.0);
	VALIDATE_FUNCTION_CALL_1(DOUBLE, "::Negd", -1.0, 1.0);
	VALIDATE_FUNCTION_CALL_1(DOUBLE, "::Negd", 1.0, -1.0);
	VALIDATE_FUNCTION_CALL_1(DOUBLE, "::Negd", -2.34e21, 2.34e21);
	VALIDATE_FUNCTION_CALL_1(DOUBLE, "::Negd", 2.34e21, -2.34e21);

	VALIDATE_FUNCTION_CALL_1(UINT, "::Not", uint32_t(1), uint8_t(0));
	VALIDATE_FUNCTION_CALL_1(UINT, "::Not", uint32_t(0), uint8_t(1));
	VALIDATE_FUNCTION_CALL_1(UINT, "::Not", uint32_t(0), uint8_t(23));

	VALIDATE_FUNCTION_CALL_1(INT, "::Preinc1", int32_t(6), int32_t(5));
	VALIDATE_FUNCTION_CALL_1(INT, "::Preinc2", int32_t(8), int32_t(7));
	VALIDATE_FUNCTION_CALL_1(INT, "::Preinc3", int32_t(10), int16_t(9));
	VALIDATE_FUNCTION_CALL_1(INT, "::Preinc4", int32_t(12), int32_t(11));

	int32_t value = 13;
	VALIDATE_FUNCTION_CALL_1(INT, "::Preinc5", int32_t(14), &value);
	ASSERT_FORMAT(value == int32_t(14),
		("Expected 14, but was %" BOND_PRId32 ".", value));

	VALIDATE_FUNCTION_CALL_1(INT, "::Postinc1", int32_t(6), int32_t(5));
	VALIDATE_FUNCTION_CALL_1(INT, "::Postinc2", int32_t(8), int32_t(7));
	VALIDATE_FUNCTION_CALL_1(INT, "::Postinc3", int32_t(10), int16_t(9));
	VALIDATE_FUNCTION_CALL_1(INT, "::Postinc4", int32_t(12), int32_t(11));

	VALIDATE_FUNCTION_CALL_1(INT, "::Postinc5", int32_t(14), &value);
	ASSERT_FORMAT(value == int32_t(15),
		("Expected 15, but was %" BOND_PRId32 ".", value));

	VALIDATE_FUNCTION_CALL_1(INT, "::Predec1", int32_t(4), int32_t(5));
	VALIDATE_FUNCTION_CALL_1(INT, "::Predec2", int32_t(6), int32_t(7));
	VALIDATE_FUNCTION_CALL_1(INT, "::Predec3", int32_t(8), int16_t(9));
	VALIDATE_FUNCTION_CALL_1(INT, "::Predec4", int32_t(10), int32_t(11));

	VALIDATE_FUNCTION_CALL_1(INT, "::Predec5", int32_t(14), &value);
	ASSERT_FORMAT(value == int32_t(14),
		("Expected 14, but was %" BOND_PRId32 ".", value));

	VALIDATE_FUNCTION_CALL_1(INT, "::Postdec1", int32_t(4), int32_t(5));
	VALIDATE_FUNCTION_CALL_1(INT, "::Postdec2", int32_t(6), int32_t(7));
	VALIDATE_FUNCTION_CALL_1(INT, "::Postdec3", int32_t(8), int16_t(9));
	VALIDATE_FUNCTION_CALL_1(INT, "::Postdec4", int32_t(10), int32_t(11));

	VALIDATE_FUNCTION_CALL_1(INT, "::Postdec5", int32_t(14), &value);
	ASSERT_FORMAT(value == int32_t(13),
		("Expected 13, but was %" BOND_PRId32 ".", value));

	return true;
}


DEFINE_VM_TEST(ComparisonOperators, "scripts/vm_ComparisonOperators.bond")
{
	using namespace Bond;

	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpeqi", int32_t(1), int32_t(0), int32_t(0));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpeqi", int32_t(1), int32_t(-1), int32_t(-1));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpeqi", int32_t(0), int32_t(0), int32_t(1));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpeqi", int32_t(0), int32_t(8382), int32_t(2398));

	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpeql", int32_t(1), int64_t(0), int64_t(0));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpeql", int32_t(1), int64_t(-1), int64_t(-1));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpeql", int32_t(1), int64_t(8382) << 31, int64_t(8382) << 31);
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpeql", int32_t(0), int64_t(0), int64_t(1));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpeql", int32_t(0), int64_t(8382) << 31, int64_t(2398) << 31);

	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpeqf", int32_t(1), 0.0f, 0.0f);
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpeqf", int32_t(1), -1.0f, -1.0f);
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpeqf", int32_t(1), 5.67e27f, 5.67e27f);
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpeqf", int32_t(0), 0.0f, 1.0f);
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpeqf", int32_t(0), 5.67e27f, 5.68e27f);

	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpeqd", int32_t(1), 0.0, 0.0);
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpeqd", int32_t(1), -1.0, -1.0);
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpeqd", int32_t(1), 5.67e27, 5.67e27);
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpeqd", int32_t(0), 0.0, 1.0);
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpeqd", int32_t(0), 5.67e27, 5.68e27);

	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpneqi", int32_t(0), int32_t(0), int32_t(0));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpneqi", int32_t(0), int32_t(-1), int32_t(-1));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpneqi", int32_t(1), int32_t(0), int32_t(1));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpneqi", int32_t(1), int32_t(8382), int32_t(2398));

	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpneql", int32_t(0), int64_t(0), int64_t(0));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpneql", int32_t(0), int64_t(-1), int64_t(-1));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpneql", int32_t(0), int64_t(8382) << 31, int64_t(8382) << 31);
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpneql", int32_t(1), int64_t(0), int64_t(1));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpneql", int32_t(1), int64_t(8382) << 31, int64_t(2398) << 31);

	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpneqf", int32_t(0), 0.0f, 0.0f);
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpneqf", int32_t(0), -1.0f, -1.0f);
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpneqf", int32_t(0), 5.67e27f, 5.67e27f);
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpneqf", int32_t(1), 0.0f, 1.0f);
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpneqf", int32_t(1), 5.67e27f, 5.68e27f);

	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpneqd", int32_t(0), 0.0, 0.0);
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpneqd", int32_t(0), -1.0, -1.0);
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpneqd", int32_t(0), 5.67e27, 5.67e27);
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpneqd", int32_t(1), 0.0, 1.0);
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpneqd", int32_t(1), 5.67e27, 5.68e27);

	VALIDATE_FUNCTION_CALL_2(INT, "::Cmplti", int32_t(0), int32_t(0), int32_t(0));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmplti", int32_t(1), int32_t(0), int32_t(1));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmplti", int32_t(1), int32_t(-2), int32_t(2));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmplti", int32_t(0), int32_t(8382), int32_t(2398));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmplti", int32_t(1), int32_t(2398), int32_t(8382));

	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpltui", int32_t(0), uint32_t(0), uint32_t(0));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpltui", int32_t(1), uint32_t(0), uint32_t(1));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpltui", int32_t(0), int32_t(-2), uint32_t(2));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpltui", int32_t(0), uint32_t(8382), uint32_t(2398));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpltui", int32_t(1), uint32_t(2398), uint32_t(8382));

	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpltl", int32_t(0), int64_t(0), int64_t(0));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpltl", int32_t(1), int64_t(0), int64_t(1));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpltl", int32_t(1), int64_t(-2), int64_t(2));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpltl", int32_t(0), int64_t(8382) << 31, int64_t(2398) << 31);
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpltl", int32_t(1), int64_t(2938) << 31, int64_t(8382) << 31);

	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpltul", int32_t(0), uint64_t(0), uint64_t(0));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpltul", int32_t(1), uint64_t(0), uint64_t(1));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpltul", int32_t(0), int64_t(-2), uint64_t(2));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpltul", int32_t(0), uint64_t(8382) << 31, uint64_t(2398) << 31);
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpltul", int32_t(1), uint64_t(2938) << 31, uint64_t(8382) << 31);

	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpltf", int32_t(0), 0.0f, 0.0f);
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpltf", int32_t(1), 0.0f, 1.0f);
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpltf", int32_t(1), -2.0f, 2.0f);
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpltf", int32_t(0), 5.68e27f, 5.67e27f);
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpltf", int32_t(1), 5.67e27f, 5.68e27f);

	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpltd", int32_t(0), 0.0, 0.0);
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpltd", int32_t(1), 0.0, 1.0);
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpltd", int32_t(1), -2.0, 2.0);
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpltd", int32_t(0), 5.68e27, 5.67e27);
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpltd", int32_t(1), 5.67e27, 5.68e27);

	VALIDATE_FUNCTION_CALL_2(INT, "::Cmplei", int32_t(1), int32_t(0), int32_t(0));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmplei", int32_t(1), int32_t(0), int32_t(1));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmplei", int32_t(1), int32_t(-2), int32_t(2));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmplei", int32_t(0), int32_t(8382), int32_t(2398));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmplei", int32_t(1), int32_t(2398), int32_t(8382));

	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpleui", int32_t(1), uint32_t(0), uint32_t(0));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpleui", int32_t(1), uint32_t(0), uint32_t(1));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpleui", int32_t(0), int32_t(-2), uint32_t(2));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpleui", int32_t(0), uint32_t(8382), uint32_t(2398));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpleui", int32_t(1), uint32_t(2398), uint32_t(8382));

	VALIDATE_FUNCTION_CALL_2(INT, "::Cmplel", int32_t(1), int64_t(0), int64_t(0));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmplel", int32_t(1), int64_t(0), int64_t(1));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmplel", int32_t(1), int64_t(-2), int64_t(2));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmplel", int32_t(0), int64_t(8382) << 31, int64_t(2398) << 31);
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmplel", int32_t(1), int64_t(2938) << 31, int64_t(8382) << 31);

	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpleul", int32_t(1), uint64_t(0), uint64_t(0));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpleul", int32_t(1), uint64_t(0), uint64_t(1));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpleul", int32_t(0), int64_t(-2), uint64_t(2));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpleul", int32_t(0), uint64_t(8382) << 31, uint64_t(2398) << 31);
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpleul", int32_t(1), uint64_t(2938) << 31, uint64_t(8382) << 31);

	VALIDATE_FUNCTION_CALL_2(INT, "::Cmplef", int32_t(1), 0.0f, 0.0f);
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmplef", int32_t(1), 0.0f, 1.0f);
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmplef", int32_t(1), -2.0f, 2.0f);
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmplef", int32_t(0), 5.68e27f, 5.67e27f);
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmplef", int32_t(1), 5.67e27f, 5.68e27f);

	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpled", int32_t(1), 0.0, 0.0);
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpled", int32_t(1), 0.0, 1.0);
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpled", int32_t(1), -2.0, 2.0);
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpled", int32_t(0), 5.68e27, 5.67e27);
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpled", int32_t(1), 5.67e27, 5.68e27);

	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgti", int32_t(0), int32_t(0), int32_t(0));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgti", int32_t(1), int32_t(1), int32_t(0));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgti", int32_t(1), int32_t(2), int32_t(-2));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgti", int32_t(0), int32_t(2398), int32_t(8382));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgti", int32_t(1), int32_t(8382), int32_t(2398));

	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgtui", int32_t(0), uint32_t(0), uint32_t(0));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgtui", int32_t(1), uint32_t(1), uint32_t(0));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgtui", int32_t(0), uint32_t(2), int32_t(-2));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgtui", int32_t(0), uint32_t(2398), uint32_t(8382));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgtui", int32_t(1), uint32_t(8382), uint32_t(2398));

	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgtl", int32_t(0), int64_t(0), int64_t(0));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgtl", int32_t(1), int64_t(1), int64_t(0));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgtl", int32_t(1), int64_t(2), int64_t(-2));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgtl", int32_t(0), int64_t(2398) << 31, int64_t(8382) << 31);
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgtl", int32_t(1), int64_t(8382) << 31, int64_t(2938) << 31);

	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgtul", int32_t(0), uint64_t(0), uint64_t(0));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgtul", int32_t(1), uint64_t(1), uint64_t(0));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgtul", int32_t(0), uint64_t(2), int64_t(-2));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgtul", int32_t(0), uint64_t(2398) << 31, uint64_t(8382) << 31);
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgtul", int32_t(1), uint64_t(8382) << 31, uint64_t(2938) << 31);

	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgtf", int32_t(0), 0.0f, 0.0f);
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgtf", int32_t(1), 1.0f, 0.0f);
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgtf", int32_t(1), 2.0f, -2.0f);
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgtf", int32_t(0), 5.67e27f, 5.68e27f);
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgtf", int32_t(1), 5.68e27f, 5.67e27f);

	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgtd", int32_t(0), 0.0, 0.0);
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgtd", int32_t(1), 1.0, 0.0);
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgtd", int32_t(1), 2.0, -2.0);
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgtd", int32_t(0), 5.67e27, 5.68e27);
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgtd", int32_t(1), 5.68e27, 5.67e27);

	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgei", int32_t(1), int32_t(0), int32_t(0));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgei", int32_t(1), int32_t(1), int32_t(0));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgei", int32_t(1), int32_t(2), int32_t(-2));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgei", int32_t(0), int32_t(2398), int32_t(8382));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgei", int32_t(1), int32_t(8382), int32_t(2398));

	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgeui", int32_t(1), uint32_t(0), uint32_t(0));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgeui", int32_t(1), uint32_t(1), uint32_t(0));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgeui", int32_t(0), uint32_t(2), int32_t(-2));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgeui", int32_t(0), uint32_t(2398), uint32_t(8382));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgeui", int32_t(1), uint32_t(8382), uint32_t(2398));

	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgel", int32_t(1), int64_t(0), int64_t(0));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgel", int32_t(1), int64_t(1), int64_t(0));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgel", int32_t(1), int64_t(2), int64_t(-2));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgel", int32_t(0), int64_t(2398) << 31, int64_t(8382) << 31);
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgel", int32_t(1), int64_t(8382) << 31, int64_t(2938) << 31);

	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgeul", int32_t(1), uint64_t(0), uint64_t(0));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgeul", int32_t(1), uint64_t(1), uint64_t(0));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgeul", int32_t(0), uint64_t(2), int64_t(-2));
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgeul", int32_t(0), uint64_t(2398) << 31, uint64_t(8382) << 31);
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgeul", int32_t(1), uint64_t(8382) << 31, uint64_t(2938) << 31);

	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgef", int32_t(1), 0.0f, 0.0f);
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgef", int32_t(1), 1.0f, 0.0f);
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgef", int32_t(1), 2.0f, -2.0f);
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgef", int32_t(0), 5.67e27f, 5.68e27f);
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpgef", int32_t(1), 5.68e27f, 5.67e27f);

	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpged", int32_t(1), 0.0, 0.0);
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpged", int32_t(1), 1.0, 0.0);
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpged", int32_t(1), 2.0, -2.0);
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpged", int32_t(0), 5.67e27, 5.68e27);
	VALIDATE_FUNCTION_CALL_2(INT, "::Cmpged", int32_t(1), 5.68e27, 5.67e27);

	return true;
}


DEFINE_VM_TEST(Branches, "scripts/vm_Branches.bond")
{
	using namespace Bond;

	VALIDATE_FUNCTION_CALL_2(UINT, "::And", uint32_t(0), uint8_t(0), uint8_t(0));
	VALIDATE_FUNCTION_CALL_2(UINT, "::And", uint32_t(0), uint8_t(0), uint8_t(1));
	VALIDATE_FUNCTION_CALL_2(UINT, "::And", uint32_t(0), uint8_t(1), uint8_t(0));
	VALIDATE_FUNCTION_CALL_2(UINT, "::And", uint32_t(1), uint8_t(1), uint8_t(1));

	VALIDATE_FUNCTION_CALL_2(UINT, "::Or", uint32_t(0), uint8_t(0), uint8_t(0));
	VALIDATE_FUNCTION_CALL_2(UINT, "::Or", uint32_t(1), uint8_t(0), uint8_t(1));
	VALIDATE_FUNCTION_CALL_2(UINT, "::Or", uint32_t(1), uint8_t(1), uint8_t(0));
	VALIDATE_FUNCTION_CALL_2(UINT, "::Or", uint32_t(1), uint8_t(1), uint8_t(1));

	VALIDATE_FUNCTION_CALL_1(INT, "::AndShortCircuit", int32_t(4), uint8_t(0));
	VALIDATE_FUNCTION_CALL_1(INT, "::AndShortCircuit", int32_t(5), uint8_t(1));

	VALIDATE_FUNCTION_CALL_1(INT, "::OrShortCircuit", int32_t(5), uint8_t(0));
	VALIDATE_FUNCTION_CALL_1(INT, "::OrShortCircuit", int32_t(4), uint8_t(1));

	VALIDATE_FUNCTION_CALL_1(INT, "::Ternary", int32_t(5), uint8_t(0));
	VALIDATE_FUNCTION_CALL_1(INT, "::Ternary", int32_t(4), uint8_t(1));

	VALIDATE_FUNCTION_CALL_1(INT, "::If", int32_t(5), uint8_t(0));
	VALIDATE_FUNCTION_CALL_1(INT, "::If", int32_t(4), uint8_t(1));

	VALIDATE_FUNCTION_CALL_1(INT, "::IfNot", int32_t(4), uint8_t(0));
	VALIDATE_FUNCTION_CALL_1(INT, "::IfNot", int32_t(5), uint8_t(1));

	VALIDATE_FUNCTION_CALL_2(INT, "::IfNotAAndNotB", int32_t(4), uint8_t(0), uint8_t(0));
	VALIDATE_FUNCTION_CALL_2(INT, "::IfNotAAndNotB", int32_t(5), uint8_t(0), uint8_t(1));
	VALIDATE_FUNCTION_CALL_2(INT, "::IfNotAAndNotB", int32_t(5), uint8_t(1), uint8_t(0));
	VALIDATE_FUNCTION_CALL_2(INT, "::IfNotAAndNotB", int32_t(5), uint8_t(1), uint8_t(1));

	VALIDATE_FUNCTION_CALL_2(INT, "::IfNotAOrNotB", int32_t(4), uint8_t(0), uint8_t(0));
	VALIDATE_FUNCTION_CALL_2(INT, "::IfNotAOrNotB", int32_t(4), uint8_t(0), uint8_t(1));
	VALIDATE_FUNCTION_CALL_2(INT, "::IfNotAOrNotB", int32_t(4), uint8_t(1), uint8_t(0));
	VALIDATE_FUNCTION_CALL_2(INT, "::IfNotAOrNotB", int32_t(5), uint8_t(1), uint8_t(1));

	VALIDATE_FUNCTION_CALL_1(INT, "::IfElse", int32_t(5), uint8_t(0));
	VALIDATE_FUNCTION_CALL_1(INT, "::IfElse", int32_t(4), uint8_t(1));

	VALIDATE_FUNCTION_CALL_2(INT, "::ElseIf", int32_t(6), uint8_t(0), uint8_t(0));
	VALIDATE_FUNCTION_CALL_2(INT, "::ElseIf", int32_t(5), uint8_t(0), uint8_t(1));
	VALIDATE_FUNCTION_CALL_2(INT, "::ElseIf", int32_t(4), uint8_t(1), uint8_t(0));
	VALIDATE_FUNCTION_CALL_2(INT, "::ElseIf", int32_t(4), uint8_t(1), uint8_t(1));

	VALIDATE_FUNCTION_CALL_2(INT, "::NestedIf", int32_t(7), uint8_t(0), uint8_t(0));
	VALIDATE_FUNCTION_CALL_2(INT, "::NestedIf", int32_t(6), uint8_t(0), uint8_t(1));
	VALIDATE_FUNCTION_CALL_2(INT, "::NestedIf", int32_t(5), uint8_t(1), uint8_t(0));
	VALIDATE_FUNCTION_CALL_2(INT, "::NestedIf", int32_t(4), uint8_t(1), uint8_t(1));

	VALIDATE_FUNCTION_CALL_1(INT, "::LookupSwitchDefault", int32_t(8), int32_t(0));
	VALIDATE_FUNCTION_CALL_1(INT, "::LookupSwitchDefault", int32_t(5), int32_t(1));
	VALIDATE_FUNCTION_CALL_1(INT, "::LookupSwitchDefault", int32_t(8), int32_t(2));
	VALIDATE_FUNCTION_CALL_1(INT, "::LookupSwitchDefault", int32_t(6), int32_t(3));
	VALIDATE_FUNCTION_CALL_1(INT, "::LookupSwitchDefault", int32_t(8), int32_t(4));
	VALIDATE_FUNCTION_CALL_1(INT, "::LookupSwitchDefault", int32_t(8), int32_t(99));
	VALIDATE_FUNCTION_CALL_1(INT, "::LookupSwitchDefault", int32_t(7), int32_t(100));
	VALIDATE_FUNCTION_CALL_1(INT, "::LookupSwitchDefault", int32_t(8), int32_t(101));

	VALIDATE_FUNCTION_CALL_1(INT, "::LookupSwitchNoDefault", int32_t(4), int32_t(0));
	VALIDATE_FUNCTION_CALL_1(INT, "::LookupSwitchNoDefault", int32_t(5), int32_t(1));
	VALIDATE_FUNCTION_CALL_1(INT, "::LookupSwitchNoDefault", int32_t(4), int32_t(2));
	VALIDATE_FUNCTION_CALL_1(INT, "::LookupSwitchNoDefault", int32_t(6), int32_t(3));
	VALIDATE_FUNCTION_CALL_1(INT, "::LookupSwitchNoDefault", int32_t(4), int32_t(4));
	VALIDATE_FUNCTION_CALL_1(INT, "::LookupSwitchNoDefault", int32_t(4), int32_t(99));
	VALIDATE_FUNCTION_CALL_1(INT, "::LookupSwitchNoDefault", int32_t(7), int32_t(100));
	VALIDATE_FUNCTION_CALL_1(INT, "::LookupSwitchNoDefault", int32_t(4), int32_t(101));

	VALIDATE_FUNCTION_CALL_1(INT, "::TableSwitchDefault", int32_t(9), int32_t(6));
	VALIDATE_FUNCTION_CALL_1(INT, "::TableSwitchDefault", int32_t(5), int32_t(7));
	VALIDATE_FUNCTION_CALL_1(INT, "::TableSwitchDefault", int32_t(6), int32_t(8));
	VALIDATE_FUNCTION_CALL_1(INT, "::TableSwitchDefault", int32_t(9), int32_t(9));
	VALIDATE_FUNCTION_CALL_1(INT, "::TableSwitchDefault", int32_t(7), int32_t(10));
	VALIDATE_FUNCTION_CALL_1(INT, "::TableSwitchDefault", int32_t(8), int32_t(11));
	VALIDATE_FUNCTION_CALL_1(INT, "::TableSwitchDefault", int32_t(9), int32_t(12));

	VALIDATE_FUNCTION_CALL_1(INT, "::TableSwitchNoDefault", int32_t(4), int32_t(6));
	VALIDATE_FUNCTION_CALL_1(INT, "::TableSwitchNoDefault", int32_t(5), int32_t(7));
	VALIDATE_FUNCTION_CALL_1(INT, "::TableSwitchNoDefault", int32_t(6), int32_t(8));
	VALIDATE_FUNCTION_CALL_1(INT, "::TableSwitchNoDefault", int32_t(4), int32_t(9));
	VALIDATE_FUNCTION_CALL_1(INT, "::TableSwitchNoDefault", int32_t(7), int32_t(10));
	VALIDATE_FUNCTION_CALL_1(INT, "::TableSwitchNoDefault", int32_t(8), int32_t(11));
	VALIDATE_FUNCTION_CALL_1(INT, "::TableSwitchNoDefault", int32_t(4), int32_t(12));

	VALIDATE_FUNCTION_CALL_1(INT, "::While", int32_t(5), int32_t(5));
	VALIDATE_FUNCTION_CALL_1(INT, "::For", int32_t(5), int32_t(5));

	VALIDATE_FUNCTION_CALL_2(INT, "::NestedLoopsWithJumps", int32_t(360), int32_t(5), int32_t(8));

	return true;
}


DEFINE_VM_TEST(Pointers, "scripts/vm_Pointers.bond")
{
	using namespace Bond;

	int32_t oneD[8];
	memset(oneD, 0, sizeof(oneD));

	vm.CallVoidFunction("::SetValueAt_3", &oneD, int32_t(7));
	ASSERT_FORMAT(oneD[3] == 7, ("Expected 7, but was %" BOND_PRId32 ".", oneD[3]));
	ASSERT_MESSAGE((oneD[1] == 0) && (oneD[2] == 0) && (oneD[4] == 0) && (oneD[5] == 0),
		"Memory stomp by SetValueAt_3() detected.");

	vm.CallVoidFunction("::SetValueAt", &oneD, int32_t(5), int32_t(864));
	ASSERT_FORMAT(oneD[5] == 864, ("Expected 864, but was %" BOND_PRId32 ".", oneD[5]));
	ASSERT_MESSAGE((oneD[3] == 7) && (oneD[4] == 0) && (oneD[6] == 0) && (oneD[7] == 0),
		"Memory stomp by SetValueAt() detected.");

	oneD[4] = 2468;
	VALIDATE_FUNCTION_CALL_1(INT, "::GetValueAt_4", oneD[4], &oneD);

	oneD[6] = 753;
	VALIDATE_FUNCTION_CALL_2(INT, "::GetValueAt", oneD[6], &oneD, int32_t(6));

	int32_t twoD[7][13];
	memset(twoD, 0, sizeof(twoD));

	vm.CallVoidFunction("::SetValue2DAt_4_9", &twoD, int32_t(901));
	ASSERT_FORMAT(twoD[4][9] == 901, ("Expected 901, but was %" BOND_PRId32 ".", twoD[4][9]));
	ASSERT_MESSAGE((twoD[3][8] == 0) && (twoD[3][9] == 0) && (twoD[3][10] == 0) &&
		(twoD[4][8] == 0) && (twoD[4][10] == 0) &&
		(twoD[5][8] == 0) && (twoD[5][9] == 0) && (twoD[5][10] == 0),
		"Memory stomp by SetValue2DAt_4_9() detected.");

	vm.CallVoidFunction("::SetValue2DAt", &twoD, int32_t(2), int32_t(4), int32_t(98973));
	ASSERT_FORMAT(twoD[2][4] == 98973, ("Expected 98973, but was %" BOND_PRId32 ".", twoD[2][4]));
	ASSERT_MESSAGE((twoD[1][3] == 0) && (twoD[1][4] == 0) && (twoD[1][5] == 0) &&
		(twoD[2][3] == 0) && (twoD[2][5] == 0) &&
		(twoD[3][3] == 0) && (twoD[3][4] == 0) && (twoD[3][5] == 0),
		"Memory stomp by SetValue2DAt() detected.");

	twoD[1][12] = 1357;
	VALIDATE_FUNCTION_CALL_1(INT, "::GetValue2DAt_1_12", twoD[1][12], &twoD);

	twoD[6][11] = 9182;
	VALIDATE_FUNCTION_CALL_3(INT, "::GetValue2DAt", twoD[6][11], &twoD, int32_t(6), int32_t(11));

	VALIDATE_FUNCTION_CALL_2(PTR, "::PointerOffset", &twoD[1][2] + 19, &twoD[1][2], int32_t(19));
	VALIDATE_FUNCTION_CALL_0(INT, "::LocalPointerDifference", int32_t(4));
	VALIDATE_FUNCTION_CALL_2(INT, "::PointerDifference", int32_t(&twoD[6][9] - &twoD[1][3]), &twoD[6][9], &twoD[1][3]);

	uint8_t c;
	VALIDATE_FUNCTION_CALL_2(PTR, "::SmallStructPointerOffset", &c + (5 * 32), &c, int32_t(5));
	VALIDATE_FUNCTION_CALL_2(INT, "::SmallStructPointerDifference", 9, &c + (9 * 32), &c);

	VALIDATE_FUNCTION_CALL_2(PTR, "::LargeStructPointerOffset", &c + (7 * 100000), &c, int32_t(7));
	VALIDATE_FUNCTION_CALL_2(INT, "::LargeStructPointerDifference", 3, &c + (3 * 100000), &c);

	VALIDATE_FUNCTION_CALL_0(INT, "::LocalAlignment", int32_t(64));
	VALIDATE_FUNCTION_CALL_0(INT, "::LocalOverlap", int32_t(1));

	return true;
}


DEFINE_VM_TEST(CompoundAssignmentOperators, "scripts/vm_CompoundAssignmentOperators.bond")
{
	using namespace Bond;

	int32_t i = -97; int32_t oi = 27; int32_t ri = i + oi;
	VALIDATE_FUNCTION_CALL_2(INT, "::Addi", ri, &i, oi);
	ASSERT_FORMAT(i == ri, ("Expected %" BOND_PRId32 ", but was %" BOND_PRId32 ".", ri, i));

	uint32_t u = 97; uint32_t ou = 27; uint32_t ru = u + ou;
	VALIDATE_FUNCTION_CALL_2(UINT, "::Addi", ru, &u, ou);
	ASSERT_FORMAT(u == ru, ("Expected %" BOND_PRIu32 ", but was %" BOND_PRIu32 ".", ru, u));

	int64_t l = int64_t(-97) << 40; int64_t ol = int64_t(27) << 40; int64_t rl = l + ol;
	VALIDATE_FUNCTION_CALL_2(LONG, "::Addl", rl, &l, ol);
	ASSERT_FORMAT(l == rl, ("Expected %" BOND_PRId64 ", but was %" BOND_PRId64 ".", rl, l));

	uint64_t ul = uint64_t(97) << 40; uint64_t oul = uint64_t(27) << 40; uint64_t rul = ul + oul;
	VALIDATE_FUNCTION_CALL_2(ULONG, "::Addl", rul, &ul, oul);
	ASSERT_FORMAT(ul == rul, ("Expected %" BOND_PRId64 ", but was %" BOND_PRId64 ".", rul, ul));

	float f = -97.0f; float of = 27.0f; float rf = f + of;
	VALIDATE_FUNCTION_CALL_2(FLOAT, "::Addf", rf, &f, of);
	ASSERT_FORMAT(f == rf, ("Expected %f, but was %f.", rf, f));

	double d = -97.0; double od = 27.0; double rd = d + od;
	VALIDATE_FUNCTION_CALL_2(DOUBLE, "::Addd", rd, &d, od);
	ASSERT_FORMAT(d == rd, ("Expected %f, but was %.", rd, d));


	i = -85; oi = 39; ri = i - oi;
	VALIDATE_FUNCTION_CALL_2(INT, "::Subi", ri, &i, oi);
	ASSERT_FORMAT(i == ri, ("Expected %" BOND_PRId32 ", but was %" BOND_PRId32 ".", ri, i));

	u = 85; ou = 39; ru = u - ou;
	VALIDATE_FUNCTION_CALL_2(UINT, "::Subi", ru, &u, ou);
	ASSERT_FORMAT(u == ru, ("Expected %" BOND_PRIu32 ", but was %" BOND_PRIu32 ".", ru, u));

	l = int64_t(-85) << 40; ol = int64_t(39) << 40; rl = l - ol;
	VALIDATE_FUNCTION_CALL_2(LONG, "::Subl", rl, &l, ol);
	ASSERT_FORMAT(l == rl, ("Expected %" BOND_PRId64 ", but was %" BOND_PRId64 ".", rl, l));

	ul = uint64_t(85) << 40; oul = uint64_t(39) << 40; rul = ul - oul;
	VALIDATE_FUNCTION_CALL_2(ULONG, "::Subl", rul, &ul, oul);
	ASSERT_FORMAT(ul == rul, ("Expected %" BOND_PRId64 ", but was %" BOND_PRId64 ".", rul, ul));

	f = -85.0f; of = 39.0f; rf = f - of;
	VALIDATE_FUNCTION_CALL_2(FLOAT, "::Subf", rf, &f, of);
	ASSERT_FORMAT(f == rf, ("Expected %f, but was %f.", rf, f));

	d = -85.0; od = 39.0; rd = d - od;
	VALIDATE_FUNCTION_CALL_2(DOUBLE, "::Subd", rd, &d, od);
	ASSERT_FORMAT(d == rd, ("Expected %f, but was %f.", rd, d));


	i = -48; oi = 135; ri = i * oi;
	VALIDATE_FUNCTION_CALL_2(INT, "::Muli", ri, &i, oi);
	ASSERT_FORMAT(i == ri, ("Expected %" BOND_PRId32 ", but was %" BOND_PRId32 ".", ri, i));

	u = 48; ou = 135; ru = u * ou;
	VALIDATE_FUNCTION_CALL_2(UINT, "::Mului", ru, &u, ou);
	ASSERT_FORMAT(u == ru, ("Expected %" BOND_PRIu32 ", but was %" BOND_PRIu32 ".", ru, u));

	l = int64_t(-48) << 40; ol = int64_t(135) << 40; rl = l * ol;
	VALIDATE_FUNCTION_CALL_2(LONG, "::Mull", rl, &l, ol);
	ASSERT_FORMAT(l == rl, ("Expected %" BOND_PRId64 ", but was %" BOND_PRId64 ".", rl, l));

	ul = uint64_t(48) << 40; oul = uint64_t(135) << 40; rul = ul * oul;
	VALIDATE_FUNCTION_CALL_2(ULONG, "::Mulul", rul, &ul, oul);
	ASSERT_FORMAT(ul == rul, ("Expected %" BOND_PRId64 ", but was %" BOND_PRId64 ".", rul, ul));

	f = -48.0f; of = 135.0f; rf = f * of;
	VALIDATE_FUNCTION_CALL_2(FLOAT, "::Mulf", rf, &f, of);
	ASSERT_FORMAT(f == rf, ("Expected %f, but was %f.", rf, f));

	d = -48.0; od = 135.0; rd = d * od;
	VALIDATE_FUNCTION_CALL_2(DOUBLE, "::Muld", rd, &d, od);
	ASSERT_FORMAT(d == rd, ("Expected %f, but was %f.", rd, d));


	i = -10; oi = 3; ri = i / oi;
	VALIDATE_FUNCTION_CALL_2(INT, "::Divi", ri, &i, oi);
	ASSERT_FORMAT(i == ri, ("Expected %" BOND_PRId32 ", but was %" BOND_PRId32 ".", ri, i));

	u = 10; ou = 3; ru = u / ou;
	VALIDATE_FUNCTION_CALL_2(UINT, "::Divui", ru, &u, ou);
	ASSERT_FORMAT(u == ru, ("Expected %" BOND_PRIu32 ", but was %" BOND_PRIu32 ".", ru, u));

	l = int64_t(-10) << 40; ol = int64_t(3); rl = l / ol;
	VALIDATE_FUNCTION_CALL_2(LONG, "::Divl", rl, &l, ol);
	ASSERT_FORMAT(l == rl, ("Expected %" BOND_PRId64 ", but was %" BOND_PRId64 ".", rl, l));

	ul = uint64_t(10) << 40; oul = uint64_t(3); rul = ul / oul;
	VALIDATE_FUNCTION_CALL_2(ULONG, "::Divul", rul, &ul, oul);
	ASSERT_FORMAT(ul == rul, ("Expected %" BOND_PRId64 ", but was %" BOND_PRId64 ".", rul, ul));

	f = -10.0f; of = 3.0f; rf = f / of;
	VALIDATE_FUNCTION_CALL_2(FLOAT, "::Divf", rf, &f, of);
	ASSERT_FORMAT(f == rf, ("Expected %f, but was %f.", rf, f));

	d = -10.0; od = 3.0; rd = d / od;
	VALIDATE_FUNCTION_CALL_2(DOUBLE, "::Divd", rd, &d, od);
	ASSERT_FORMAT(d == rd, ("Expected %f, but was %f.", rd, d));


	i = -10; oi = 3; ri = i % oi;
	VALIDATE_FUNCTION_CALL_2(INT, "::Remi", ri, &i, oi);
	ASSERT_FORMAT(i == ri, ("Expected %" BOND_PRId32 ", but was %" BOND_PRId32 ".", ri, i));

	u = 10; ou = 3; ru = u % ou;
	VALIDATE_FUNCTION_CALL_2(UINT, "::Remui", ru, &u, ou);
	ASSERT_FORMAT(u == ru, ("Expected %" BOND_PRIu32 ", but was %" BOND_PRIu32 ".", ru, u));

	l = int64_t(-10) << 40; ol = int64_t(34); rl = l % ol;
	VALIDATE_FUNCTION_CALL_2(LONG, "::Reml", rl, &l, ol);
	ASSERT_FORMAT(l == rl, ("Expected %" BOND_PRId64 ", but was %" BOND_PRId64 ".", rl, l));

	ul = uint64_t(10) << 40; oul = uint64_t(34); rul = ul % oul;
	VALIDATE_FUNCTION_CALL_2(ULONG, "::Remul", rul, &ul, oul);
	ASSERT_FORMAT(ul == rul, ("Expected %" BOND_PRId64 ", but was %" BOND_PRId64 ".", rul, ul));


	i = 7; oi = 7; ri = i << oi;
	VALIDATE_FUNCTION_CALL_2(INT, "::Lshi", ri, &i, oi);
	ASSERT_FORMAT(i == ri, ("Expected %" BOND_PRId32 ", but was %" BOND_PRId32 ".", ri, i));

	l = 7; ol = 50; rl = l << ol;
	VALIDATE_FUNCTION_CALL_2(LONG, "::Lshl", rl, &l, ol);
	ASSERT_FORMAT(l == rl, ("Expected %" BOND_PRId64 ", but was %" BOND_PRId64 ".", rl, l));


	i = -(int32_t(1) << 20); oi = 16; ri = i >> oi;
	VALIDATE_FUNCTION_CALL_2(INT, "::Rshi", ri, &i, oi);
	ASSERT_FORMAT(i == ri, ("Expected %" BOND_PRId32 ", but was %" BOND_PRId32 ".", ri, i));

	u = uint32_t(-(int32_t(1) << 20)); ou = 16; ru = u >> ou;
	VALIDATE_FUNCTION_CALL_2(UINT, "::Rshui", ru, &u, ou);
	ASSERT_FORMAT(u == ru, ("Expected %" BOND_PRIu32 ", but was %" BOND_PRIu32 ".", ru, u));

	l = -(int64_t(1) << 20); ol = 16; rl = l >> ol;
	VALIDATE_FUNCTION_CALL_2(LONG, "::Rshl", rl, &l, ol);
	ASSERT_FORMAT(l == rl, ("Expected %" BOND_PRId64 ", but was %" BOND_PRId64 ".", rl, l));

	ul = uint64_t(-(int64_t(1) << 20)); oul = 16; rul = ul >> oul;
	VALIDATE_FUNCTION_CALL_2(ULONG, "::Rshul", rul, &ul, oul);
	ASSERT_FORMAT(ul == rul, ("Expected %" BOND_PRId64 ", but was %" BOND_PRId64 ".", rul, ul));


	i = 0xfff0; oi = 0xff; ri = i & oi;
	VALIDATE_FUNCTION_CALL_2(INT, "::Andi", ri, &i, oi);
	ASSERT_FORMAT(i == ri, ("Expected %" BOND_PRId32 ", but was %" BOND_PRId32 ".", ri, i));

	l = int64_t(0xfff0) << 31; ol = int64_t(0xff) << 31; rl = l & ol;
	VALIDATE_FUNCTION_CALL_2(LONG, "::Andl", rl, &l, ol);
	ASSERT_FORMAT(l == rl, ("Expected %" BOND_PRId64 ", but was %" BOND_PRId64 ".", rl, l));


	i = 0xfff0; oi = 0xff; ri = i | oi;
	VALIDATE_FUNCTION_CALL_2(INT, "::Ori", ri, &i, oi);
	ASSERT_FORMAT(i == ri, ("Expected %" BOND_PRId32 ", but was %" BOND_PRId32 ".", ri, i));

	l = int64_t(0xfff0) << 31; ol = int64_t(0xff) << 31; rl = l | ol;
	VALIDATE_FUNCTION_CALL_2(LONG, "::Orl", rl, &l, ol);
	ASSERT_FORMAT(l == rl, ("Expected %" BOND_PRId64 ", but was %" BOND_PRId64 ".", rl, l));


	i = 0xfff0; oi = 0xff; ri = i ^ oi;
	VALIDATE_FUNCTION_CALL_2(INT, "::Xori", ri, &i, oi);
	ASSERT_FORMAT(i == ri, ("Expected %" BOND_PRId32 ", but was %" BOND_PRId32 ".", ri, i));

	l = int64_t(0xfff0) << 31; ol = int64_t(0xff) << 31; rl = l ^ ol;
	VALIDATE_FUNCTION_CALL_2(LONG, "::Xorl", rl, &l, ol);
	ASSERT_FORMAT(l == rl, ("Expected %" BOND_PRId64 ", but was %" BOND_PRId64 ".", rl, l));

	return true;
}


DEFINE_VM_TEST(FunctionCalls, "scripts/vm_FunctionCalls.bond")
{
	using namespace Bond;

	VALIDATE_FUNCTION_CALL_1(UINT, "::FibonacciRecursive", uint32_t(6765), uint32_t(20));
	VALIDATE_FUNCTION_CALL_0(UINT, "::TestSetCharValue", uint32_t(1));
	VALIDATE_FUNCTION_CALL_4(INT, "::StrangeMath", int32_t((10 + 40) - (30 + 80)), int32_t(10), int32_t(40), int32_t(30), int32_t(80));

	return true;
}


DEFINE_VM_TEST(Structs, "scripts/vm_Structs.bond")
{
	using namespace Bond;

	Char3 c3;
	memset(&c3, 0, sizeof(c3));
	vm.CallVoidFunction("::SetChar3", &c3, int8_t(45), int8_t(-56), int8_t(67));
	ASSERT_FORMAT(c3.x == 45, ("Expected 45, but was %" BOND_PRId32 ".", c3.x));
	ASSERT_FORMAT(c3.y == -56, ("Expected -56, but was %" BOND_PRId32 ".", c3.y));
	ASSERT_FORMAT(c3.z == 67, ("Expected 67, but was %" BOND_PRId32 ".", c3.z));
	ASSERT_FORMAT(c3.padding == 0, ("Expected 0, but was %" BOND_PRId32 ".", c3.padding));

	VALIDATE_FUNCTION_CALL_1(INT, "::GetChar3X", int32_t(45), c3);
	VALIDATE_FUNCTION_CALL_1(INT, "::GetChar3Y", int32_t(-56), c3);
	VALIDATE_FUNCTION_CALL_1(INT, "::GetChar3Z", int32_t(67), c3);

	Char3Pair char3Pair;
	vm.CallVoidFunction("::SetChar3PairX", &char3Pair, int8_t(23), int8_t(32));
	ASSERT_FORMAT(char3Pair.first.x == 23, ("Expected 23, but was %" BOND_PRId32 ".", char3Pair.first.x));
	ASSERT_FORMAT(char3Pair.second.x == 32, ("Expected 32, but was %" BOND_PRId32 ".", char3Pair.second.x));

	VALIDATE_FUNCTION_CALL_0(UINT, "::InitializeChar3Pair", uint32_t(1));

	Vector3 v3a;
	Vector3 v3b;
	Vector3 v3c;
	memset(&v3a, 0, sizeof(v3a));
	vm.CallVoidFunction("::SetVector3", &v3a, int32_t(11), int32_t(-12), int32_t(13));
	ASSERT_FORMAT(v3a.x == 11, ("Expected 11, but was %" BOND_PRId32 ".", v3a.x));
	ASSERT_FORMAT(v3a.y == -12, ("Expected -12, but was %" BOND_PRId32 ".", v3a.y));
	ASSERT_FORMAT(v3a.z == 13, ("Expected 13, but was %" BOND_PRId32 ".", v3a.z));

	v3a.x = 88;
	v3a.y = 888;
	v3a.z = 8888;
	VALIDATE_FUNCTION_CALL_1(INT, "::GetVector3X", v3a.x, v3a);
	VALIDATE_FUNCTION_CALL_1(INT, "::Vector3::GetX", v3a.x, &v3a);
	VALIDATE_FUNCTION_CALL_1(INT, "::GetVector3Y", v3a.y, v3a);
	VALIDATE_FUNCTION_CALL_1(INT, "::Vector3::GetY", v3a.y, &v3a);
	VALIDATE_FUNCTION_CALL_1(INT, "::GetVector3Z", v3a.z, v3a);
	VALIDATE_FUNCTION_CALL_1(INT, "::Vector3::GetZ", v3a.z, &v3a);

	v3a.x = 2;
	v3a.y = 3;
	v3a.z = 4;
	v3b.x = 5;
	v3b.y = -6;
	v3b.z = 7;
	VALIDATE_FUNCTION_CALL_2(INT, "::Vector3::Dot", int32_t(20), &v3a, &v3b);

	vm.CallFunction("::ConstructVector3", &v3a, int32_t(99), int32_t(-100), int32_t(101));
	ASSERT_FORMAT(v3a.x == 99, ("Expected 99, but was %" BOND_PRId32 ".", v3a.x));
	ASSERT_FORMAT(v3a.y == -100, ("Expected -100, but was %" BOND_PRId32 ".", v3a.y));
	ASSERT_FORMAT(v3a.z == 101, ("Expected 101, but was %" BOND_PRId32 ".", v3a.z));

	vm.CallFunction("::ConstructVector3Indirect", &v3a, int32_t(102), int32_t(-103), int32_t(104));
	ASSERT_FORMAT(v3a.x == 102, ("Expected 102, but was %" BOND_PRId32 ".", v3a.x));
	ASSERT_FORMAT(v3a.y == -103, ("Expected -103, but was %" BOND_PRId32 ".", v3a.y));
	ASSERT_FORMAT(v3a.z == 104, ("Expected 104, but was %" BOND_PRId32 ".", v3a.z));

	VALIDATE_FUNCTION_CALL_1(INT, "::ConstructVector3AndGetY", int32_t(12321), int32_t(12321));

	v3a.x = 31;
	v3a.y = 32;
	v3a.z = 33;
	v3b.x = 41;
	v3b.y = 42;
	v3b.z = 43;
	v3c.x = 51;
	v3c.y = 52;
	v3c.z = 53;

	vm.CallFunction("::SelectVector3", &v3c, uint8_t(1), v3a, v3b);
	ASSERT_FORMAT(v3c.x == 31, ("Expected 31, but was %" BOND_PRId32 ".", v3c.x));
	ASSERT_FORMAT(v3c.y == 32, ("Expected 32, but was %" BOND_PRId32 ".", v3c.y));
	ASSERT_FORMAT(v3c.z == 33, ("Expected 33, but was %" BOND_PRId32 ".", v3c.z));

	vm.CallFunction("::SelectVector3", &v3c, uint8_t(0), v3a, v3b);
	ASSERT_FORMAT(v3c.x == 41, ("Expected 41, but was %" BOND_PRId32 ".", v3c.x));
	ASSERT_FORMAT(v3c.y == 42, ("Expected 42, but was %" BOND_PRId32 ".", v3c.y));
	ASSERT_FORMAT(v3c.z == 43, ("Expected 43, but was %" BOND_PRId32 ".", v3c.z));

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
  TEST_ITEM(Pointers)                           \
  TEST_ITEM(CompoundAssignmentOperators)        \
  TEST_ITEM(FunctionCalls)                      \
  TEST_ITEM(Structs)                            \

RUN_TESTS(VM, TEST_ITEMS)
