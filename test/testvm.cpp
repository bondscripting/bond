#include "framework/testvmframework.h"
#include <cstring>

namespace
{
// Avoids MSVC C4310 for intentionally truncating constant casts.
template <typename To, typename From>
To TruncateCast(From value)
{
	return static_cast<To>(value);
}
}


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
	VALIDATE_FUNCTION_CALL(int32_t, "Constc", int32_t(-7));
	VALIDATE_FUNCTION_CALL(uint32_t, "Constuc", int32_t(130));
	VALIDATE_FUNCTION_CALL(int32_t, "Consts", int32_t(-345));
	VALIDATE_FUNCTION_CALL(uint32_t, "Constus", int32_t(40000));
	VALIDATE_FUNCTION_CALL(int32_t, "Const32", int32_t(-34567));
	VALIDATE_FUNCTION_CALL(int64_t, "Const64", int64_t(-34567));
	VALIDATE_FUNCTION_CALL(int32_t, "Consti_n2", int32_t(-2));
	VALIDATE_FUNCTION_CALL(int32_t, "Consti_n1", int32_t(-1));
	VALIDATE_FUNCTION_CALL(int32_t, "Consti_0", int32_t(0));
	VALIDATE_FUNCTION_CALL(int32_t, "Consti_1", int32_t(1));
	VALIDATE_FUNCTION_CALL(int32_t, "Consti_2", int32_t(2));
	VALIDATE_FUNCTION_CALL(int32_t, "Consti_3", int32_t(3));
	VALIDATE_FUNCTION_CALL(int32_t, "Consti_4", int32_t(4));
	VALIDATE_FUNCTION_CALL(int32_t, "Consti_8", int32_t(8));
	VALIDATE_FUNCTION_CALL(int64_t, "Constl_n2", int64_t(-2));
	VALIDATE_FUNCTION_CALL(int64_t, "Constl_n1", int64_t(-1));
	VALIDATE_FUNCTION_CALL(int64_t, "Constl_0", int64_t(0));
	VALIDATE_FUNCTION_CALL(int64_t, "Constl_1", int64_t(1));
	VALIDATE_FUNCTION_CALL(int64_t, "Constl_2", int64_t(2));
	VALIDATE_FUNCTION_CALL(int64_t, "Constl_3", int64_t(3));
	VALIDATE_FUNCTION_CALL(int64_t, "Constl_4", int64_t(4));
	VALIDATE_FUNCTION_CALL(int64_t, "Constl_8", int64_t(8));
	VALIDATE_FUNCTION_CALL(float, "Constf_n2", -2.0f);
	VALIDATE_FUNCTION_CALL(float, "Constf_n1", -1.0f);
	VALIDATE_FUNCTION_CALL(float, "Constf_nh", -0.5f);
	VALIDATE_FUNCTION_CALL(float, "Constf_0", 0.0f);
	VALIDATE_FUNCTION_CALL(float, "Constf_h",  0.5f);
	VALIDATE_FUNCTION_CALL(float, "Constf_1", 1.0f);
	VALIDATE_FUNCTION_CALL(float, "Constf_2", 2.0f);
	VALIDATE_FUNCTION_CALL(double, "Constd_n2", -2.0);
	VALIDATE_FUNCTION_CALL(double, "Constd_n1", -1.0);
	VALIDATE_FUNCTION_CALL(double, "Constd_nh", -0.5);
	VALIDATE_FUNCTION_CALL(double, "Constd_0", 0.0);
	VALIDATE_FUNCTION_CALL(double, "Constd_h",  0.5);
	VALIDATE_FUNCTION_CALL(double, "Constd_1", 1.0);
	VALIDATE_FUNCTION_CALL(double, "Constd_2", 2.0);
}


DEFINE_VM_TEST(StackOperations, "scripts/vm_StackOperations.bond")
{
	using namespace Bond;
	VALIDATE_FUNCTION_CALL(int32_t, "PushcPopc", int32_t(-123), int8_t(0), int8_t(0), int8_t(0), int8_t(0), int8_t(-123));
	VALIDATE_FUNCTION_CALL(uint32_t, "PushucPopc", uint32_t(234), uint8_t(0), uint8_t(0), uint8_t(0), uint8_t(0), uint8_t(234));
	VALIDATE_FUNCTION_CALL(int32_t, "PushsPops", int32_t(-1234), int16_t(0), int16_t(0), int16_t(0), int16_t(0), int16_t(-1234));
	VALIDATE_FUNCTION_CALL(uint32_t, "PushusPops", uint32_t(60000), uint16_t(0), uint16_t(0), uint16_t(0), uint16_t(0), uint16_t(60000));
	VALIDATE_FUNCTION_CALL(int32_t, "Push32Pop32", int32_t(-56789), int32_t(0), int32_t(0), int32_t(0), int32_t(0), int32_t(-56789));
	VALIDATE_FUNCTION_CALL(int64_t, "Push64Pop64", int64_t(-567890), int64_t(0), int64_t(0), int64_t(0), int64_t(0), int64_t(-567890));
}


DEFINE_VM_TEST(MemoryOperations, "scripts/vm_MemoryOperations.bond")
{
	using namespace Bond;
	int8_t chars[] = {0, -123, 0, 0, 0};
	VALIDATE_FUNCTION_CALL(int32_t, "LoadcStorec", int32_t(-123), chars + 1, chars + 3);
	ASSERT_FORMAT(chars[1] == chars[3], "Expected %" BOND_PRId32 ", but was %" BOND_PRId32 ".", chars[1], chars[3]);
	ASSERT_MESSAGE((chars[0] == 0) && (chars[2] == 0) && (chars[4] == 0), "Memory stomp by LoadcStorec() detected.");

	uint8_t uchars[] = {0, 0xff, 0, 0, 0};
	VALIDATE_FUNCTION_CALL(uint32_t, "LoaducStorec", uint32_t(0xff), uchars + 1, uchars + 3);
	ASSERT_FORMAT(uchars[1] == uchars[3], "Expected %" BOND_PRIu32 ", but was %" BOND_PRIu32 ".", uchars[1], uchars[3]);
	ASSERT_MESSAGE((uchars[0] == 0) && (uchars[2] == 0) && (uchars[4] == 0), "Memory stomp by LoaducStorec() detected.");

	int16_t shorts[] = {0, -234, 0, 0, 0};
	VALIDATE_FUNCTION_CALL(int32_t, "LoadsStores", int32_t(-234), shorts + 1, shorts + 3);
	ASSERT_FORMAT(shorts[1] == shorts[3], "Expected %" BOND_PRId32 ", but was %" BOND_PRId32 ".", shorts[1], shorts[3]);
	ASSERT_MESSAGE((shorts[0] == 0) && (shorts[2] == 0) && (shorts[4] == 0), "Memory stomp by LoadsStores() detected.");

	uint16_t ushorts[] = {0, 0xffff, 0, 0, 0};
	VALIDATE_FUNCTION_CALL(uint32_t, "LoadusStores", int32_t(0xffff), ushorts + 1, ushorts + 3);
	ASSERT_FORMAT(ushorts[1] == ushorts[3], "Expected %" BOND_PRIu32 ", but was %" BOND_PRIu32 ".", ushorts[1], ushorts[3]);
	ASSERT_MESSAGE((ushorts[0] == 0) && (ushorts[2] == 0) && (ushorts[4] == 0), "Memory stomp by LoadusStores() detected.");

	int32_t ints[] = {0, -345, 0, 0, 0};
	VALIDATE_FUNCTION_CALL(int32_t, "Load32Store32", int32_t(-345), ints + 1, ints + 3);
	ASSERT_FORMAT(ints[1] == ints[3], "Expected %" BOND_PRId32 ", but was %" BOND_PRId32 ".", ints[1], ints[3]);
	ASSERT_MESSAGE((ints[0] == 0) && (ints[2] == 0) && (ints[4] == 0), "Memory stomp by Load32Store32() detected.");

	int64_t longs[] = {0, -456, 0, 0, 0};
	VALIDATE_FUNCTION_CALL(int64_t, "Load64Store64", int64_t(-456), longs + 1, longs + 3);
	ASSERT_FORMAT(longs[1] == longs[3], "Expected %" BOND_PRId64 ", but was %" BOND_PRId64 ".", longs[1], longs[3]);
	ASSERT_MESSAGE((longs[0] == 0) && (longs[2] == 0) && (longs[4] == 0), "Memory stomp by Load64Store64() detected.");

	VALIDATE_FUNCTION_CALL(int32_t, "Loadfp", int32_t(-567), int32_t(-567));
}


DEFINE_VM_TEST(TypeConversions, "scripts/vm_TypeConversions.bond")
{
	using namespace Bond;
	VALIDATE_FUNCTION_CALL(int32_t, "Itoc", int32_t(int8_t(-17)), int32_t(-17));
	VALIDATE_FUNCTION_CALL(int32_t, "Itoc", int32_t(TruncateCast<int8_t>(0xabc)), int32_t(0xabc));
	VALIDATE_FUNCTION_CALL(int32_t, "Itoc", int32_t(TruncateCast<int8_t>(0x864)), int32_t(0x864));

	VALIDATE_FUNCTION_CALL(uint32_t, "Uitouc", uint32_t(uint8_t(17)), uint32_t(17));
	VALIDATE_FUNCTION_CALL(uint32_t, "Uitouc", uint32_t(TruncateCast<uint8_t>(0xabc)), uint32_t(0xabc));
	VALIDATE_FUNCTION_CALL(uint32_t, "Uitouc", uint32_t(TruncateCast<uint8_t>(0x864)), uint32_t(0x864));

	VALIDATE_FUNCTION_CALL(int32_t, "Itos", int32_t(int16_t(-17)), int32_t(-17));
	VALIDATE_FUNCTION_CALL(int32_t, "Itos", int32_t(int16_t(0xabc)), int32_t(0xabc));
	VALIDATE_FUNCTION_CALL(int32_t, "Itos", int32_t(TruncateCast<int16_t>(0xabcde)), int32_t(0xabcde));
	VALIDATE_FUNCTION_CALL(int32_t, "Itos", int32_t(TruncateCast<int16_t>(0x987654)), int32_t(0x987654));

	VALIDATE_FUNCTION_CALL(uint32_t, "Uitous", uint32_t(uint16_t(17)), uint32_t(17));
	VALIDATE_FUNCTION_CALL(uint32_t, "Uitous", uint32_t(uint16_t(0xabc)), uint32_t(0xabc));
	VALIDATE_FUNCTION_CALL(uint32_t, "Uitous", uint32_t(TruncateCast<uint16_t>(0xabcde)), uint32_t(0xabcde));
	VALIDATE_FUNCTION_CALL(uint32_t, "Uitous", uint32_t(TruncateCast<uint16_t>(0x987654)), uint32_t(0x987654));

	VALIDATE_FUNCTION_CALL(int32_t, "Uctoc", int32_t(-56), uint8_t(200));
	VALIDATE_FUNCTION_CALL(int32_t, "Uctoc", int32_t(100), uint8_t(100));
	VALIDATE_FUNCTION_CALL(uint32_t, "Ctouc", uint32_t(255), int8_t(-1));
	VALIDATE_FUNCTION_CALL(uint32_t, "Ctouc", uint32_t(100), int8_t(100));
	VALIDATE_FUNCTION_CALL(int32_t, "Ustos", int32_t(-1), uint16_t(0xffff));
	VALIDATE_FUNCTION_CALL(int32_t, "Ustos", int32_t(1000), uint16_t(1000));
	VALIDATE_FUNCTION_CALL(uint32_t, "Stous", uint32_t(0xffff), int16_t(-1));
	VALIDATE_FUNCTION_CALL(uint32_t, "Stous", uint32_t(1000), int16_t(1000));
	VALIDATE_FUNCTION_CALL(uint32_t, "Ctous", uint32_t(0xffff), int8_t(-1));
	VALIDATE_FUNCTION_CALL(uint32_t, "Ctous", uint32_t(100), int8_t(100));

	VALIDATE_FUNCTION_CALL(int64_t, "Itol", int64_t(int32_t(-4567)), int32_t(-4567));
	VALIDATE_FUNCTION_CALL(int64_t, "Itol", int64_t(int32_t(4567)), int32_t(4567));

	VALIDATE_FUNCTION_CALL(uint64_t, "Uitoul", uint64_t(uint32_t(7654)), uint32_t(7654));

	VALIDATE_FUNCTION_CALL(float, "Itof", float(int32_t(0)), int32_t(0));
	VALIDATE_FUNCTION_CALL(float, "Itof", float(int32_t(1)), int32_t(1));
	VALIDATE_FUNCTION_CALL(float, "Itof", float(int32_t(2)), int32_t(2));
	VALIDATE_FUNCTION_CALL(float, "Itof", float(int32_t(-1)), int32_t(-1));
	VALIDATE_FUNCTION_CALL(float, "Itof", float(int32_t(-2)), int32_t(-2));
	VALIDATE_FUNCTION_CALL(float, "Itof", float(int32_t(-3000)), int32_t(-3000));

	VALIDATE_FUNCTION_CALL(float, "Uitof", float(uint32_t(0)), uint32_t(0));
	VALIDATE_FUNCTION_CALL(float, "Uitof", float(uint32_t(1)), uint32_t(1));
	VALIDATE_FUNCTION_CALL(float, "Uitof", float(uint32_t(2)), uint32_t(2));
	VALIDATE_FUNCTION_CALL(float, "Uitof", float(uint32_t(3000)), uint32_t(3000));

	VALIDATE_FUNCTION_CALL(double, "Itod", double(int32_t(0)), int32_t(0));
	VALIDATE_FUNCTION_CALL(double, "Itod", double(int32_t(1)), int32_t(1));
	VALIDATE_FUNCTION_CALL(double, "Itod", double(int32_t(2)), int32_t(2));
	VALIDATE_FUNCTION_CALL(double, "Itod", double(int32_t(-1)), int32_t(-1));
	VALIDATE_FUNCTION_CALL(double, "Itod", double(int32_t(-2)), int32_t(-2));
	VALIDATE_FUNCTION_CALL(double, "Itod", double(int32_t(-3000)), int32_t(-3000));

	VALIDATE_FUNCTION_CALL(double, "Uitod", double(uint32_t(0)), uint32_t(0));
	VALIDATE_FUNCTION_CALL(double, "Uitod", double(uint32_t(1)), uint32_t(1));
	VALIDATE_FUNCTION_CALL(double, "Uitod", double(uint32_t(2)), uint32_t(2));
	VALIDATE_FUNCTION_CALL(double, "Uitod", double(uint32_t(3000)), uint32_t(3000));

	VALIDATE_FUNCTION_CALL(int32_t, "Ltoi", int32_t(int64_t(-8745)), int64_t(-8745));
	VALIDATE_FUNCTION_CALL(int32_t, "Ltoi", TruncateCast<int32_t>(int64_t(0xabcdef) << 16), int64_t(0xabcdef) << 16);
	VALIDATE_FUNCTION_CALL(int32_t, "Ltoi", int32_t(int64_t(8745)), int64_t(8745));

	VALIDATE_FUNCTION_CALL(float, "Ltof", float(int64_t(0)), int64_t(0));
	VALIDATE_FUNCTION_CALL(float, "Ltof", float(int64_t(1)), int64_t(1));
	VALIDATE_FUNCTION_CALL(float, "Ltof", float(int64_t(2)), int64_t(2));
	VALIDATE_FUNCTION_CALL(float, "Ltof", float(int64_t(-1)), int64_t(-1));
	VALIDATE_FUNCTION_CALL(float, "Ltof", float(int64_t(-2)), int64_t(-2));
	VALIDATE_FUNCTION_CALL(float, "Ltof", float(int64_t(-3000)), int64_t(-3000));

	VALIDATE_FUNCTION_CALL(float, "Ultof", float(uint64_t(0)), uint64_t(0));
	VALIDATE_FUNCTION_CALL(float, "Ultof", float(uint64_t(1)), uint64_t(1));
	VALIDATE_FUNCTION_CALL(float, "Ultof", float(uint64_t(2)), uint64_t(2));
	VALIDATE_FUNCTION_CALL(float, "Ultof", float(uint64_t(3000)), uint64_t(3000));

	VALIDATE_FUNCTION_CALL(double, "Ltod", double(int64_t(0)), int64_t(0));
	VALIDATE_FUNCTION_CALL(double, "Ltod", double(int64_t(1)), int64_t(1));
	VALIDATE_FUNCTION_CALL(double, "Ltod", double(int64_t(2)), int64_t(2));
	VALIDATE_FUNCTION_CALL(double, "Ltod", double(int64_t(-1)), int64_t(-1));
	VALIDATE_FUNCTION_CALL(double, "Ltod", double(int64_t(-2)), int64_t(-2));
	VALIDATE_FUNCTION_CALL(double, "Ltod", double(int64_t(-3000)), int64_t(-3000));

	VALIDATE_FUNCTION_CALL(double, "Ultod", double(uint64_t(0)), uint64_t(0));
	VALIDATE_FUNCTION_CALL(double, "Ultod", double(uint64_t(1)), uint64_t(1));
	VALIDATE_FUNCTION_CALL(double, "Ultod", double(uint64_t(2)), uint64_t(2));
	VALIDATE_FUNCTION_CALL(double, "Ultod", double(uint64_t(3000)), uint64_t(3000));

	VALIDATE_FUNCTION_CALL(int32_t, "Ftoi", int32_t(0.0f), 0.0f);
	VALIDATE_FUNCTION_CALL(int32_t, "Ftoi", int32_t(0.1f), 0.1f);
	VALIDATE_FUNCTION_CALL(int32_t, "Ftoi", int32_t(1.1f), 1.1f);
	VALIDATE_FUNCTION_CALL(int32_t, "Ftoi", int32_t(-0.1f), -0.1f);
	VALIDATE_FUNCTION_CALL(int32_t, "Ftoi", int32_t(-1.1f), -1.1f);
	VALIDATE_FUNCTION_CALL(int32_t, "Ftoi", int32_t(1.23e9f), 1.23e9f);

	VALIDATE_FUNCTION_CALL(uint32_t, "Ftoui", uint32_t(0.0f), 0.0f);
	VALIDATE_FUNCTION_CALL(uint32_t, "Ftoui", uint32_t(0.1f), 0.1f);
	VALIDATE_FUNCTION_CALL(uint32_t, "Ftoui", uint32_t(1.1f), 1.1f);
	VALIDATE_FUNCTION_CALL(uint32_t, "Ftoui", uint32_t(1.23e9f), 1.23e9f);

	VALIDATE_FUNCTION_CALL(int64_t, "Ftol", int64_t(0.0f), 0.0f);
	VALIDATE_FUNCTION_CALL(int64_t, "Ftol", int64_t(0.1f), 0.1f);
	VALIDATE_FUNCTION_CALL(int64_t, "Ftol", int64_t(1.1f), 1.1f);
	VALIDATE_FUNCTION_CALL(int64_t, "Ftol", int64_t(-0.1f), -0.1f);
	VALIDATE_FUNCTION_CALL(int64_t, "Ftol", int64_t(-1.1f), -1.1f);
	VALIDATE_FUNCTION_CALL(int64_t, "Ftol", int64_t(1.23e10f), 1.23e10f);

	VALIDATE_FUNCTION_CALL(uint64_t, "Ftoul", uint64_t(0.0f), 0.0f);
	VALIDATE_FUNCTION_CALL(uint64_t, "Ftoul", uint64_t(0.1f), 0.1f);
	VALIDATE_FUNCTION_CALL(uint64_t, "Ftoul", uint64_t(1.1f), 1.1f);
	VALIDATE_FUNCTION_CALL(uint64_t, "Ftoul", uint64_t(1.23e10f), 1.23e10f);

	VALIDATE_FUNCTION_CALL(double, "Ftod", 0.0, 0.0f);
	VALIDATE_FUNCTION_CALL(double, "Ftod", 0.1, 0.1f);
	VALIDATE_FUNCTION_CALL(double, "Ftod", 1.1, 1.1f);
	VALIDATE_FUNCTION_CALL(double, "Ftod", -0.1, -0.1f);
	VALIDATE_FUNCTION_CALL(double, "Ftod", -1.1, -1.1f);
	VALIDATE_FUNCTION_CALL(double, "Ftod", 1.23e10, 1.23e10f);

	VALIDATE_FUNCTION_CALL(int32_t, "Dtoi", int32_t(0.0), 0.0);
	VALIDATE_FUNCTION_CALL(int32_t, "Dtoi", int32_t(0.1), 0.1);
	VALIDATE_FUNCTION_CALL(int32_t, "Dtoi", int32_t(1.1), 1.1);
	VALIDATE_FUNCTION_CALL(int32_t, "Dtoi", int32_t(-0.1), -0.1);
	VALIDATE_FUNCTION_CALL(int32_t, "Dtoi", int32_t(-1.1), -1.1);
	VALIDATE_FUNCTION_CALL(int32_t, "Dtoi", int32_t(1.23e9), 1.23e9);

	VALIDATE_FUNCTION_CALL(uint32_t, "Dtoui", uint32_t(0.0), 0.0);
	VALIDATE_FUNCTION_CALL(uint32_t, "Dtoui", uint32_t(0.1), 0.1);
	VALIDATE_FUNCTION_CALL(uint32_t, "Dtoui", uint32_t(1.1), 1.1);
	VALIDATE_FUNCTION_CALL(uint32_t, "Dtoui", uint32_t(1.23e9), 1.23e9);

	VALIDATE_FUNCTION_CALL(int64_t, "Dtol", int64_t(0.0), 0.0);
	VALIDATE_FUNCTION_CALL(int64_t, "Dtol", int64_t(0.1), 0.1);
	VALIDATE_FUNCTION_CALL(int64_t, "Dtol", int64_t(1.1), 1.1);
	VALIDATE_FUNCTION_CALL(int64_t, "Dtol", int64_t(-0.1), -0.1);
	VALIDATE_FUNCTION_CALL(int64_t, "Dtol", int64_t(-1.1), -1.1);
	VALIDATE_FUNCTION_CALL(int64_t, "Dtol", int64_t(1.23e10), 1.23e10);

	VALIDATE_FUNCTION_CALL(uint64_t, "Dtoul", uint64_t(0.0), 0.0);
	VALIDATE_FUNCTION_CALL(uint64_t, "Dtoul", uint64_t(0.1), 0.1);
	VALIDATE_FUNCTION_CALL(uint64_t, "Dtoul", uint64_t(1.1), 1.1);
	VALIDATE_FUNCTION_CALL(uint64_t, "Dtoul", uint64_t(1.23e10), 1.23e10);

	VALIDATE_FUNCTION_CALL(float, "Dtof", 0.0f, 0.0);
	VALIDATE_FUNCTION_CALL(float, "Dtof", 0.1f, 0.1);
	VALIDATE_FUNCTION_CALL(float, "Dtof", 1.1f, 1.1);
	VALIDATE_FUNCTION_CALL(float, "Dtof", -0.1f, -0.1);
	VALIDATE_FUNCTION_CALL(float, "Dtof", -1.1f, -1.1);
	VALIDATE_FUNCTION_CALL(float, "Dtof", 1.23e10f, 1.23e10);
}


DEFINE_VM_TEST(BinaryOperators, "scripts/vm_BinaryOperators.bond")
{
	using namespace Bond;
	VALIDATE_FUNCTION_CALL(int32_t, "Addi", int32_t(-65) + int32_t(13), int32_t(-65), int32_t(13));
	VALIDATE_FUNCTION_CALL(uint32_t, "Addi", uint32_t(65) + uint32_t(13), uint32_t(65), uint32_t(13));
	VALIDATE_FUNCTION_CALL(int64_t, "Addl", (-(int64_t(65) << 40)) + (int64_t(13) << 40), (-(int64_t(65) << 40)), (int64_t(13) << 40));
	VALIDATE_FUNCTION_CALL(uint64_t, "Addl", (uint64_t(65) << 40) + (uint64_t(13) << 40), (uint64_t(65) << 40), (uint64_t(13) << 40));
	VALIDATE_FUNCTION_CALL(float, "Addf", -65.0f + 13.0f, -65.0f, 13.0f);
	VALIDATE_FUNCTION_CALL(float, "Addf", 1e20f + 0.5e20f, 1e20f, 0.5e20f);
	VALIDATE_FUNCTION_CALL(double, "Addd", -65.0 + 13.0, -65.0, 13.0);
	VALIDATE_FUNCTION_CALL(double, "Addd", 1e40 + 0.5e40, 1e40, 0.5e40);

	VALIDATE_FUNCTION_CALL(int32_t, "Subi", int32_t(-43) - int32_t(12), int32_t(-43), int32_t(12));
	VALIDATE_FUNCTION_CALL(uint32_t, "Subi", uint32_t(43) - uint32_t(12), uint32_t(43), uint32_t(12));
	VALIDATE_FUNCTION_CALL(int64_t, "Subl", (-(int64_t(43) << 40)) - (int64_t(12) << 40), (-(int64_t(43) << 40)), (int64_t(12) << 40));
	VALIDATE_FUNCTION_CALL(uint64_t, "Subl", (uint64_t(43) << 40) - (uint64_t(12) << 40), (uint64_t(43) << 40), (uint64_t(12) << 40));
	VALIDATE_FUNCTION_CALL(float, "Subf", -43.0f - 12.0f, -43.0f, 12.0f);
	VALIDATE_FUNCTION_CALL(float, "Subf", 1e20f - 0.5e20f, 1e20f, 0.5e20f);
	VALIDATE_FUNCTION_CALL(double, "Subd", -43.0 - 12.0, -43.0, 12.0);
	VALIDATE_FUNCTION_CALL(double, "Subd", 1e40 - 0.5e40, 1e40, 0.5e40);

	VALIDATE_FUNCTION_CALL(int32_t, "Muli", int32_t(-58) * int32_t(0), int32_t(-58), int32_t(0));
	VALIDATE_FUNCTION_CALL(int32_t, "Muli", int32_t(-58) * int32_t(17), int32_t(-58), int32_t(17));
	VALIDATE_FUNCTION_CALL(uint32_t, "Mului", uint32_t(58) * uint32_t(17), uint32_t(58), uint32_t(17));
	VALIDATE_FUNCTION_CALL(uint32_t, "Mului", uint32_t(0xFFFFFFFFu) * uint32_t(3), uint32_t(0xFFFFFFFFu), uint32_t(3));
	VALIDATE_FUNCTION_CALL(int64_t, "Mull", (-(int64_t(58) << 40)) * int64_t(17), (-(int64_t(58) << 40)), int64_t(17));
	VALIDATE_FUNCTION_CALL(uint64_t, "Mulul", (uint64_t(58) << 40) * uint64_t(17), (uint64_t(58) << 40), uint64_t(17));
	VALIDATE_FUNCTION_CALL(uint64_t, "Mulul", ~uint64_t(0) * uint64_t(3), ~uint64_t(0), uint64_t(3));
	VALIDATE_FUNCTION_CALL(float, "Mulf", 0.0f * 84.3f, -0.0f, 84.3f);
	VALIDATE_FUNCTION_CALL(float, "Mulf", -98.1f * 84.3f, -98.1f, 84.3f);
	VALIDATE_FUNCTION_CALL(float, "Mulf", 1e20f * 3.3e10f, 1e20f, 3.3e10f);
	VALIDATE_FUNCTION_CALL(double, "Muld", 0.0 * 84.3, -0.0, 84.3);
	VALIDATE_FUNCTION_CALL(double, "Muld", -98.1 * 84.3, -98.1, 84.3);
	VALIDATE_FUNCTION_CALL(double, "Muld", 1e40 * 3.3e20, 1e40, 3.3e20);

	VALIDATE_FUNCTION_CALL(int32_t, "Divi", int32_t(0) / int32_t(3), int32_t(0), int32_t(3));
	VALIDATE_FUNCTION_CALL(int32_t, "Divi", int32_t(-10) / int32_t(3), int32_t(-10), int32_t(3));
	VALIDATE_FUNCTION_CALL(uint32_t, "Divui", Bond::BOND_UINT_MAX / uint32_t(5678), BOND_UINT_MAX, uint32_t(5678));
	VALIDATE_FUNCTION_CALL(int64_t, "Divl", Bond::BOND_LONG_MIN / int64_t(5678), BOND_LONG_MIN, int64_t(5678));
	VALIDATE_FUNCTION_CALL(uint64_t, "Divul", Bond::BOND_ULONG_MAX / uint64_t(5678), BOND_ULONG_MAX, uint64_t(5678));
	VALIDATE_FUNCTION_CALL(float, "Divf", 0.0f / 84.3f, -0.0f, 84.3f);
	VALIDATE_FUNCTION_CALL(float, "Divf", -98.1f / 84.3f, -98.1f, 84.3f);
	VALIDATE_FUNCTION_CALL(float, "Divf", 1e20f / 3.3e10f, 1e20f, 3.3e10f);
	VALIDATE_FUNCTION_CALL(double, "Divd", 0.0 / 84.3, -0.0, 84.3);
	VALIDATE_FUNCTION_CALL(double, "Divd", -98.1 / 84.3, -98.1, 84.3);
	VALIDATE_FUNCTION_CALL(double, "Divd", 1e40 / 3.3e20, 1e40, 3.3e20);

	VALIDATE_FUNCTION_CALL(int32_t, "Remi", int32_t(0) % int32_t(3), int32_t(0), int32_t(3));
	VALIDATE_FUNCTION_CALL(int32_t, "Remi", int32_t(10) % int32_t(3), int32_t(10), int32_t(3));
	VALIDATE_FUNCTION_CALL(int32_t, "Remi", int32_t(-10) % int32_t(3), int32_t(-10), int32_t(3));
	VALIDATE_FUNCTION_CALL(int32_t, "Remi", int32_t(2352) % int32_t(24), int32_t(2352), int32_t(24));
	VALIDATE_FUNCTION_CALL(int32_t, "Remui", uint32_t(0) % uint32_t(3), uint32_t(0), uint32_t(3));
	VALIDATE_FUNCTION_CALL(int32_t, "Remui", uint32_t(10) % uint32_t(3), uint32_t(10), uint32_t(3));
	VALIDATE_FUNCTION_CALL(int32_t, "Remui", uint32_t(2352) % uint32_t(24), uint32_t(2352), uint32_t(24));
	VALIDATE_FUNCTION_CALL(uint32_t, "Remui", Bond::BOND_UINT_MAX % uint32_t(5678), BOND_UINT_MAX, uint32_t(5678));
	VALIDATE_FUNCTION_CALL(int64_t, "Reml", Bond::BOND_LONG_MIN % int64_t(5678), BOND_LONG_MIN, int64_t(5678));
	VALIDATE_FUNCTION_CALL(uint64_t, "Remul", Bond::BOND_ULONG_MAX % uint64_t(5678), BOND_ULONG_MAX, uint64_t(5678));

	VALIDATE_FUNCTION_CALL(int32_t, "Lshi", int32_t(0) << int32_t(1), int32_t(0), int32_t(1));
	VALIDATE_FUNCTION_CALL(uint32_t, "Lshi", uint32_t(1) << uint32_t(1), uint32_t(1), uint32_t(1));
	VALIDATE_FUNCTION_CALL(int32_t, "Lshi", int32_t(1 & 1) << int32_t(31), int32_t(1), int32_t(31));
	VALIDATE_FUNCTION_CALL(int32_t, "Lshi", int32_t(2 & 1) << int32_t(31), int32_t(2), int32_t(31));
	VALIDATE_FUNCTION_CALL(int32_t, "Lshi", int32_t(3 & 1) << int32_t(31), int32_t(3), int32_t(31));
	VALIDATE_FUNCTION_CALL(int32_t, "Lshi", -(int32_t(3) << int32_t(16)), int32_t(-3), int32_t(16));

	VALIDATE_FUNCTION_CALL(int64_t, "Lshl", int64_t(0) << int64_t(1), int64_t(0), int64_t(1));
	VALIDATE_FUNCTION_CALL(uint64_t, "Lshl", uint64_t(1) << uint64_t(1), uint64_t(1), uint64_t(1));
	VALIDATE_FUNCTION_CALL(int64_t, "Lshl", int64_t(1 & 1) << int64_t(63), int64_t(1), int64_t(63));
	VALIDATE_FUNCTION_CALL(int64_t, "Lshl", int64_t(2 & 1) << int64_t(63), int64_t(2), int64_t(63));
	VALIDATE_FUNCTION_CALL(int64_t, "Lshl", int64_t(3 & 1) << int64_t(63), int64_t(3), int64_t(63));
	VALIDATE_FUNCTION_CALL(int64_t, "Lshl", -(int64_t(3) << int64_t(16)), int64_t(-3), int64_t(16));

	VALIDATE_FUNCTION_CALL(int32_t, "Rshi", int32_t(0) >> int32_t(1), int32_t(0), int32_t(1));
	VALIDATE_FUNCTION_CALL(uint32_t, "Rshi", uint32_t(2) >> uint32_t(1), uint32_t(2), uint32_t(1));
	VALIDATE_FUNCTION_CALL(int32_t, "Rshi", int32_t(1) >> int32_t(1), int32_t(1), int32_t(1));
	VALIDATE_FUNCTION_CALL(int32_t, "Rshi", int32_t(2) >> int32_t(1), int32_t(2), int32_t(1));
	VALIDATE_FUNCTION_CALL(int32_t, "Rshi", int32_t(3) >> int32_t(1), int32_t(3), int32_t(1));
	VALIDATE_FUNCTION_CALL(int32_t, "Rshi", -(int32_t(1) << 20) >> int32_t(16), -(int32_t(1) << 20), int32_t(16));
	VALIDATE_FUNCTION_CALL(uint32_t, "Rshui", uint32_t(-(int32_t(1) << 20)) >> uint32_t(16), uint32_t(-(int32_t(1) << 20)), uint32_t(16));

	VALIDATE_FUNCTION_CALL(int64_t, "Rshl", int64_t(0) >> int64_t(1), int64_t(0), int64_t(1));
	VALIDATE_FUNCTION_CALL(int64_t, "Rshl", int64_t(1) >> int64_t(1), int64_t(1), int64_t(1));
	VALIDATE_FUNCTION_CALL(int64_t, "Rshl", int64_t(2) >> int64_t(1), int64_t(2), int64_t(1));
	VALIDATE_FUNCTION_CALL(int64_t, "Rshl", int64_t(3) >> int64_t(1), int64_t(3), int64_t(1));
	VALIDATE_FUNCTION_CALL(int64_t, "Rshl", -(int64_t(1) << int64_t(20)) >> int64_t(16), -(int64_t(1) << int64_t(20)), int64_t(16));
	VALIDATE_FUNCTION_CALL(uint64_t, "Rshul", uint64_t(-(int64_t(1) << int64_t(20))) >> uint64_t(16), uint64_t(-(int64_t(1) << int64_t(20))), uint64_t(16));

	// Shifts take the type of the promoted left operand.
	VALIDATE_FUNCTION_CALL(uint32_t, "RshuiInt", uint32_t(0x08000000), uint32_t(0x80000000), int32_t(4));
	VALIDATE_FUNCTION_CALL(int64_t, "LshiLong", int64_t(INT32_MIN), int32_t(1), int64_t(31));
	VALIDATE_FUNCTION_CALL(int64_t, "LshlInt", int64_t(1) << 40, int64_t(1), int32_t(40));
	VALIDATE_FUNCTION_CALL(int64_t, "LshUChar", int64_t(INT32_MIN), uint8_t(0x80), int32_t(24));
	VALIDATE_FUNCTION_CALL(int64_t, "LshUShort", int64_t(INT32_MIN), uint16_t(0x8000), int32_t(16));
	VALIDATE_FUNCTION_CALL(uint32_t, "RshuiConstant", uint32_t(0x08000000));
	VALIDATE_FUNCTION_CALL(int64_t, "LshiConstant", int64_t(INT32_MIN));

	VALIDATE_FUNCTION_CALL(int32_t, "Andi", int32_t(0xfff0) & int32_t(0xff), int32_t(0xfff0), int32_t(0xff));
	VALIDATE_FUNCTION_CALL(int64_t, "Andl", (int64_t(0xfff0) << 31) & (int64_t(0xff) << 31), (int64_t(0xfff0) << 31), (int64_t(0xff) << 31));
	VALIDATE_FUNCTION_CALL(int32_t, "Ori", int32_t(0xfff0) | int32_t(0xff), int32_t(0xfff0), int32_t(0xff));
	VALIDATE_FUNCTION_CALL(int64_t, "Orl", (int64_t(0xfff0) << 31) | (int64_t(0xff) << 31), (int64_t(0xfff0) << 31), (int64_t(0xff) << 31));
	VALIDATE_FUNCTION_CALL(int32_t, "Xori", int32_t(0xfff0) ^ int32_t(0xff), int32_t(0xfff0), int32_t(0xff));
	VALIDATE_FUNCTION_CALL(int64_t, "Xorl", (int64_t(0xfff0) << 31) ^ (int64_t(0xff) << 31), (int64_t(0xfff0) << 31), (int64_t(0xff) << 31));
}


DEFINE_VM_TEST(UnaryOperators, "scripts/vm_UnaryOperators.bond")
{
	using namespace Bond;

	VALIDATE_FUNCTION_CALL(int32_t, "Negi", int32_t(0), int32_t(0));
	VALIDATE_FUNCTION_CALL(int32_t, "Negi", int32_t(-1), int32_t(1));
	VALIDATE_FUNCTION_CALL(int32_t, "Negi", int32_t(1), int32_t(-1));
	VALIDATE_FUNCTION_CALL(int32_t, "Negi", int32_t(-BOND_INT_MAX), int32_t(BOND_INT_MAX));
	VALIDATE_FUNCTION_CALL(int32_t, "Negi", int32_t(BOND_INT_MAX), int32_t(-BOND_INT_MAX));

	VALIDATE_FUNCTION_CALL(int64_t, "Negl", int64_t(0), int64_t(0));
	VALIDATE_FUNCTION_CALL(int64_t, "Negl", int64_t(-1), int64_t(1));
	VALIDATE_FUNCTION_CALL(int64_t, "Negl", int64_t(1), int64_t(-1));
	VALIDATE_FUNCTION_CALL(int64_t, "Negl", int64_t(-BOND_LONG_MAX), int64_t(BOND_LONG_MAX));
	VALIDATE_FUNCTION_CALL(int64_t, "Negl", int64_t(BOND_LONG_MAX), int64_t(-BOND_LONG_MAX));

	VALIDATE_FUNCTION_CALL(float, "Negf", 0.0f, 0.0f);
	VALIDATE_FUNCTION_CALL(float, "Negf", -1.0f, 1.0f);
	VALIDATE_FUNCTION_CALL(float, "Negf", 1.0f, -1.0f);
	VALIDATE_FUNCTION_CALL(float, "Negf", -2.34e21f, 2.34e21f);
	VALIDATE_FUNCTION_CALL(float, "Negf", 2.34e21f, -2.34e21f);

	VALIDATE_FUNCTION_CALL(double, "Negd", 0.0, 0.0);
	VALIDATE_FUNCTION_CALL(double, "Negd", -1.0, 1.0);
	VALIDATE_FUNCTION_CALL(double, "Negd", 1.0, -1.0);
	VALIDATE_FUNCTION_CALL(double, "Negd", -2.34e21, 2.34e21);
	VALIDATE_FUNCTION_CALL(double, "Negd", 2.34e21, -2.34e21);

	VALIDATE_FUNCTION_CALL(uint32_t, "Not", uint32_t(1), uint8_t(0));
	VALIDATE_FUNCTION_CALL(uint32_t, "Not", uint32_t(0), uint8_t(1));
	VALIDATE_FUNCTION_CALL(uint32_t, "Not", uint32_t(0), uint8_t(23));

	VALIDATE_FUNCTION_CALL(int32_t, "BitNoti", int32_t(-1), int32_t(0));
	VALIDATE_FUNCTION_CALL(int32_t, "BitNoti", ~int32_t(0x12345678), int32_t(0x12345678));
	VALIDATE_FUNCTION_CALL(uint32_t, "BitNotui", ~uint32_t(0x89abcdef), uint32_t(0x89abcdef));
	VALIDATE_FUNCTION_CALL(int64_t, "BitNotl", int64_t(-1), int64_t(0));
	VALIDATE_FUNCTION_CALL(int64_t, "BitNotl", ~(int64_t(0x12345678) << 24), int64_t(0x12345678) << 24);
	VALIDATE_FUNCTION_CALL(uint64_t, "BitNotul", ~(uint64_t(0x89abcdef) << 24), uint64_t(0x89abcdef) << 24);

	VALIDATE_FUNCTION_CALL(int32_t, "Preinc1", int32_t(6), int32_t(5));
	VALIDATE_FUNCTION_CALL(int32_t, "Preinc2", int32_t(8), int32_t(7));
	VALIDATE_FUNCTION_CALL(int32_t, "Preinc3", int32_t(10), int16_t(9));
	VALIDATE_FUNCTION_CALL(int32_t, "Preinc4", int32_t(12), int32_t(11));

	int32_t value = 13;
	VALIDATE_FUNCTION_CALL(int32_t, "Preinc5", int32_t(14), &value);
	ASSERT_FORMAT(value == int32_t(14), "Expected 14, but was %" BOND_PRId32 ".", value);

	VALIDATE_FUNCTION_CALL(int32_t, "Postinc1", int32_t(6), int32_t(5));
	VALIDATE_FUNCTION_CALL(int32_t, "Postinc2", int32_t(8), int32_t(7));
	VALIDATE_FUNCTION_CALL(int32_t, "Postinc3", int32_t(10), int16_t(9));
	VALIDATE_FUNCTION_CALL(int32_t, "Postinc4", int32_t(12), int32_t(11));

	VALIDATE_FUNCTION_CALL(int32_t, "Postinc5", int32_t(14), &value);
	ASSERT_FORMAT(value == int32_t(15), "Expected 15, but was %" BOND_PRId32 ".", value);

	VALIDATE_FUNCTION_CALL(int32_t, "Predec1", int32_t(4), int32_t(5));
	VALIDATE_FUNCTION_CALL(int32_t, "Predec2", int32_t(6), int32_t(7));
	VALIDATE_FUNCTION_CALL(int32_t, "Predec3", int32_t(8), int16_t(9));
	VALIDATE_FUNCTION_CALL(int32_t, "Predec4", int32_t(10), int32_t(11));

	VALIDATE_FUNCTION_CALL(int32_t, "Predec5", int32_t(14), &value);
	ASSERT_FORMAT(value == int32_t(14), "Expected 14, but was %" BOND_PRId32 ".", value);

	VALIDATE_FUNCTION_CALL(int32_t, "Postdec1", int32_t(4), int32_t(5));
	VALIDATE_FUNCTION_CALL(int32_t, "Postdec2", int32_t(6), int32_t(7));
	VALIDATE_FUNCTION_CALL(int32_t, "Postdec3", int32_t(8), int16_t(9));
	VALIDATE_FUNCTION_CALL(int32_t, "Postdec4", int32_t(10), int32_t(11));

	VALIDATE_FUNCTION_CALL(int32_t, "Postdec5", int32_t(14), &value);
	ASSERT_FORMAT(value == int32_t(13), "Expected 13, but was %" BOND_PRId32 ".", value);

	VALIDATE_FUNCTION_CALL(int32_t, "PreincParamValue", int32_t(21), int32_t(20));
	VALIDATE_FUNCTION_CALL(int32_t, "PostincParamValue", int32_t(20), int32_t(20));
	VALIDATE_FUNCTION_CALL(int32_t, "PredecParamValue", int32_t(19), int32_t(20));
	VALIDATE_FUNCTION_CALL(int32_t, "PostdecParamValue", int32_t(20), int32_t(20));
	VALIDATE_FUNCTION_CALL(int32_t, "IncSecondParam", int32_t(32), int32_t(10), int32_t(30));

	VALIDATE_FUNCTION_CALL(int64_t, "PreincLongParam", (int64_t(5) << 40) + 1, int64_t(5) << 40);
	VALIDATE_FUNCTION_CALL(int64_t, "PostincLongParam", int64_t(5) << 40, int64_t(5) << 40);
	VALIDATE_FUNCTION_CALL(int64_t, "PredecLongParam", (int64_t(5) << 40) - 1, int64_t(5) << 40);
	VALIDATE_FUNCTION_CALL(int64_t, "PostdecLongParam", int64_t(5) << 40, int64_t(5) << 40);
}


DEFINE_VM_TEST(ComparisonOperators, "scripts/vm_ComparisonOperators.bond")
{
	using namespace Bond;

	VALIDATE_FUNCTION_CALL(int32_t, "Cmpeqi", int32_t(1), int32_t(0), int32_t(0));
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpeqi", int32_t(1), int32_t(-1), int32_t(-1));
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpeqi", int32_t(0), int32_t(0), int32_t(1));
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpeqi", int32_t(0), int32_t(8382), int32_t(2398));

	VALIDATE_FUNCTION_CALL(int32_t, "Cmpeql", int32_t(1), int64_t(0), int64_t(0));
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpeql", int32_t(1), int64_t(-1), int64_t(-1));
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpeql", int32_t(1), int64_t(8382) << 31, int64_t(8382) << 31);
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpeql", int32_t(0), int64_t(0), int64_t(1));
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpeql", int32_t(0), int64_t(8382) << 31, int64_t(2398) << 31);

	VALIDATE_FUNCTION_CALL(int32_t, "Cmpeqf", int32_t(1), 0.0f, 0.0f);
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpeqf", int32_t(1), -1.0f, -1.0f);
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpeqf", int32_t(1), 5.67e27f, 5.67e27f);
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpeqf", int32_t(0), 0.0f, 1.0f);
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpeqf", int32_t(0), 5.67e27f, 5.68e27f);

	VALIDATE_FUNCTION_CALL(int32_t, "Cmpeqd", int32_t(1), 0.0, 0.0);
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpeqd", int32_t(1), -1.0, -1.0);
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpeqd", int32_t(1), 5.67e27, 5.67e27);
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpeqd", int32_t(0), 0.0, 1.0);
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpeqd", int32_t(0), 5.67e27, 5.68e27);

	VALIDATE_FUNCTION_CALL(int32_t, "Cmpneqi", int32_t(0), int32_t(0), int32_t(0));
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpneqi", int32_t(0), int32_t(-1), int32_t(-1));
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpneqi", int32_t(1), int32_t(0), int32_t(1));
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpneqi", int32_t(1), int32_t(8382), int32_t(2398));

	VALIDATE_FUNCTION_CALL(int32_t, "Cmpneql", int32_t(0), int64_t(0), int64_t(0));
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpneql", int32_t(0), int64_t(-1), int64_t(-1));
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpneql", int32_t(0), int64_t(8382) << 31, int64_t(8382) << 31);
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpneql", int32_t(1), int64_t(0), int64_t(1));
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpneql", int32_t(1), int64_t(8382) << 31, int64_t(2398) << 31);

	VALIDATE_FUNCTION_CALL(int32_t, "Cmpneqf", int32_t(0), 0.0f, 0.0f);
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpneqf", int32_t(0), -1.0f, -1.0f);
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpneqf", int32_t(0), 5.67e27f, 5.67e27f);
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpneqf", int32_t(1), 0.0f, 1.0f);
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpneqf", int32_t(1), 5.67e27f, 5.68e27f);

	VALIDATE_FUNCTION_CALL(int32_t, "Cmpneqd", int32_t(0), 0.0, 0.0);
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpneqd", int32_t(0), -1.0, -1.0);
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpneqd", int32_t(0), 5.67e27, 5.67e27);
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpneqd", int32_t(1), 0.0, 1.0);
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpneqd", int32_t(1), 5.67e27, 5.68e27);

	VALIDATE_FUNCTION_CALL(int32_t, "Cmplti", int32_t(0), int32_t(0), int32_t(0));
	VALIDATE_FUNCTION_CALL(int32_t, "Cmplti", int32_t(1), int32_t(0), int32_t(1));
	VALIDATE_FUNCTION_CALL(int32_t, "Cmplti", int32_t(1), int32_t(-2), int32_t(2));
	VALIDATE_FUNCTION_CALL(int32_t, "Cmplti", int32_t(0), int32_t(8382), int32_t(2398));
	VALIDATE_FUNCTION_CALL(int32_t, "Cmplti", int32_t(1), int32_t(2398), int32_t(8382));

	VALIDATE_FUNCTION_CALL(int32_t, "Cmpltui", int32_t(0), uint32_t(0), uint32_t(0));
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpltui", int32_t(1), uint32_t(0), uint32_t(1));
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpltui", int32_t(0), int32_t(-2), uint32_t(2));
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpltui", int32_t(0), uint32_t(8382), uint32_t(2398));
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpltui", int32_t(1), uint32_t(2398), uint32_t(8382));

	VALIDATE_FUNCTION_CALL(int32_t, "Cmpltl", int32_t(0), int64_t(0), int64_t(0));
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpltl", int32_t(1), int64_t(0), int64_t(1));
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpltl", int32_t(1), int64_t(-2), int64_t(2));
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpltl", int32_t(0), int64_t(8382) << 31, int64_t(2398) << 31);
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpltl", int32_t(1), int64_t(2938) << 31, int64_t(8382) << 31);

	VALIDATE_FUNCTION_CALL(int32_t, "Cmpltul", int32_t(0), uint64_t(0), uint64_t(0));
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpltul", int32_t(1), uint64_t(0), uint64_t(1));
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpltul", int32_t(0), int64_t(-2), uint64_t(2));
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpltul", int32_t(0), uint64_t(8382) << 31, uint64_t(2398) << 31);
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpltul", int32_t(1), uint64_t(2938) << 31, uint64_t(8382) << 31);

	VALIDATE_FUNCTION_CALL(int32_t, "Cmpltf", int32_t(0), 0.0f, 0.0f);
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpltf", int32_t(1), 0.0f, 1.0f);
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpltf", int32_t(1), -2.0f, 2.0f);
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpltf", int32_t(0), 5.68e27f, 5.67e27f);
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpltf", int32_t(1), 5.67e27f, 5.68e27f);

	VALIDATE_FUNCTION_CALL(int32_t, "Cmpltd", int32_t(0), 0.0, 0.0);
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpltd", int32_t(1), 0.0, 1.0);
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpltd", int32_t(1), -2.0, 2.0);
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpltd", int32_t(0), 5.68e27, 5.67e27);
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpltd", int32_t(1), 5.67e27, 5.68e27);

	VALIDATE_FUNCTION_CALL(int32_t, "Cmplei", int32_t(1), int32_t(0), int32_t(0));
	VALIDATE_FUNCTION_CALL(int32_t, "Cmplei", int32_t(1), int32_t(0), int32_t(1));
	VALIDATE_FUNCTION_CALL(int32_t, "Cmplei", int32_t(1), int32_t(-2), int32_t(2));
	VALIDATE_FUNCTION_CALL(int32_t, "Cmplei", int32_t(0), int32_t(8382), int32_t(2398));
	VALIDATE_FUNCTION_CALL(int32_t, "Cmplei", int32_t(1), int32_t(2398), int32_t(8382));

	VALIDATE_FUNCTION_CALL(int32_t, "Cmpleui", int32_t(1), uint32_t(0), uint32_t(0));
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpleui", int32_t(1), uint32_t(0), uint32_t(1));
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpleui", int32_t(0), int32_t(-2), uint32_t(2));
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpleui", int32_t(0), uint32_t(8382), uint32_t(2398));
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpleui", int32_t(1), uint32_t(2398), uint32_t(8382));

	VALIDATE_FUNCTION_CALL(int32_t, "Cmplel", int32_t(1), int64_t(0), int64_t(0));
	VALIDATE_FUNCTION_CALL(int32_t, "Cmplel", int32_t(1), int64_t(0), int64_t(1));
	VALIDATE_FUNCTION_CALL(int32_t, "Cmplel", int32_t(1), int64_t(-2), int64_t(2));
	VALIDATE_FUNCTION_CALL(int32_t, "Cmplel", int32_t(0), int64_t(8382) << 31, int64_t(2398) << 31);
	VALIDATE_FUNCTION_CALL(int32_t, "Cmplel", int32_t(1), int64_t(2938) << 31, int64_t(8382) << 31);

	VALIDATE_FUNCTION_CALL(int32_t, "Cmpleul", int32_t(1), uint64_t(0), uint64_t(0));
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpleul", int32_t(1), uint64_t(0), uint64_t(1));
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpleul", int32_t(0), int64_t(-2), uint64_t(2));
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpleul", int32_t(0), uint64_t(8382) << 31, uint64_t(2398) << 31);
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpleul", int32_t(1), uint64_t(2938) << 31, uint64_t(8382) << 31);

	VALIDATE_FUNCTION_CALL(int32_t, "Cmplef", int32_t(1), 0.0f, 0.0f);
	VALIDATE_FUNCTION_CALL(int32_t, "Cmplef", int32_t(1), 0.0f, 1.0f);
	VALIDATE_FUNCTION_CALL(int32_t, "Cmplef", int32_t(1), -2.0f, 2.0f);
	VALIDATE_FUNCTION_CALL(int32_t, "Cmplef", int32_t(0), 5.68e27f, 5.67e27f);
	VALIDATE_FUNCTION_CALL(int32_t, "Cmplef", int32_t(1), 5.67e27f, 5.68e27f);

	VALIDATE_FUNCTION_CALL(int32_t, "Cmpled", int32_t(1), 0.0, 0.0);
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpled", int32_t(1), 0.0, 1.0);
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpled", int32_t(1), -2.0, 2.0);
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpled", int32_t(0), 5.68e27, 5.67e27);
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpled", int32_t(1), 5.67e27, 5.68e27);

	VALIDATE_FUNCTION_CALL(int32_t, "Cmpgti", int32_t(0), int32_t(0), int32_t(0));
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpgti", int32_t(1), int32_t(1), int32_t(0));
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpgti", int32_t(1), int32_t(2), int32_t(-2));
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpgti", int32_t(0), int32_t(2398), int32_t(8382));
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpgti", int32_t(1), int32_t(8382), int32_t(2398));

	VALIDATE_FUNCTION_CALL(int32_t, "Cmpgtui", int32_t(0), uint32_t(0), uint32_t(0));
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpgtui", int32_t(1), uint32_t(1), uint32_t(0));
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpgtui", int32_t(0), uint32_t(2), int32_t(-2));
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpgtui", int32_t(0), uint32_t(2398), uint32_t(8382));
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpgtui", int32_t(1), uint32_t(8382), uint32_t(2398));

	VALIDATE_FUNCTION_CALL(int32_t, "Cmpgtl", int32_t(0), int64_t(0), int64_t(0));
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpgtl", int32_t(1), int64_t(1), int64_t(0));
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpgtl", int32_t(1), int64_t(2), int64_t(-2));
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpgtl", int32_t(0), int64_t(2398) << 31, int64_t(8382) << 31);
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpgtl", int32_t(1), int64_t(8382) << 31, int64_t(2938) << 31);

	VALIDATE_FUNCTION_CALL(int32_t, "Cmpgtul", int32_t(0), uint64_t(0), uint64_t(0));
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpgtul", int32_t(1), uint64_t(1), uint64_t(0));
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpgtul", int32_t(0), uint64_t(2), int64_t(-2));
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpgtul", int32_t(0), uint64_t(2398) << 31, uint64_t(8382) << 31);
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpgtul", int32_t(1), uint64_t(8382) << 31, uint64_t(2938) << 31);

	VALIDATE_FUNCTION_CALL(int32_t, "Cmpgtf", int32_t(0), 0.0f, 0.0f);
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpgtf", int32_t(1), 1.0f, 0.0f);
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpgtf", int32_t(1), 2.0f, -2.0f);
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpgtf", int32_t(0), 5.67e27f, 5.68e27f);
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpgtf", int32_t(1), 5.68e27f, 5.67e27f);

	VALIDATE_FUNCTION_CALL(int32_t, "Cmpgtd", int32_t(0), 0.0, 0.0);
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpgtd", int32_t(1), 1.0, 0.0);
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpgtd", int32_t(1), 2.0, -2.0);
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpgtd", int32_t(0), 5.67e27, 5.68e27);
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpgtd", int32_t(1), 5.68e27, 5.67e27);

	VALIDATE_FUNCTION_CALL(int32_t, "Cmpgei", int32_t(1), int32_t(0), int32_t(0));
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpgei", int32_t(1), int32_t(1), int32_t(0));
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpgei", int32_t(1), int32_t(2), int32_t(-2));
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpgei", int32_t(0), int32_t(2398), int32_t(8382));
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpgei", int32_t(1), int32_t(8382), int32_t(2398));

	VALIDATE_FUNCTION_CALL(int32_t, "Cmpgeui", int32_t(1), uint32_t(0), uint32_t(0));
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpgeui", int32_t(1), uint32_t(1), uint32_t(0));
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpgeui", int32_t(0), uint32_t(2), int32_t(-2));
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpgeui", int32_t(0), uint32_t(2398), uint32_t(8382));
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpgeui", int32_t(1), uint32_t(8382), uint32_t(2398));

	VALIDATE_FUNCTION_CALL(int32_t, "Cmpgel", int32_t(1), int64_t(0), int64_t(0));
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpgel", int32_t(1), int64_t(1), int64_t(0));
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpgel", int32_t(1), int64_t(2), int64_t(-2));
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpgel", int32_t(0), int64_t(2398) << 31, int64_t(8382) << 31);
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpgel", int32_t(1), int64_t(8382) << 31, int64_t(2938) << 31);

	VALIDATE_FUNCTION_CALL(int32_t, "Cmpgeul", int32_t(1), uint64_t(0), uint64_t(0));
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpgeul", int32_t(1), uint64_t(1), uint64_t(0));
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpgeul", int32_t(0), uint64_t(2), int64_t(-2));
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpgeul", int32_t(0), uint64_t(2398) << 31, uint64_t(8382) << 31);
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpgeul", int32_t(1), uint64_t(8382) << 31, uint64_t(2938) << 31);

	VALIDATE_FUNCTION_CALL(int32_t, "Cmpgef", int32_t(1), 0.0f, 0.0f);
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpgef", int32_t(1), 1.0f, 0.0f);
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpgef", int32_t(1), 2.0f, -2.0f);
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpgef", int32_t(0), 5.67e27f, 5.68e27f);
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpgef", int32_t(1), 5.68e27f, 5.67e27f);

	VALIDATE_FUNCTION_CALL(int32_t, "Cmpged", int32_t(1), 0.0, 0.0);
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpged", int32_t(1), 1.0, 0.0);
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpged", int32_t(1), 2.0, -2.0);
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpged", int32_t(0), 5.67e27, 5.68e27);
	VALIDATE_FUNCTION_CALL(int32_t, "Cmpged", int32_t(1), 5.68e27, 5.67e27);
}


DEFINE_VM_TEST(PropertyofOperators, "scripts/vm_PropertyofOperators.bond")
{
	using namespace Bond;

	VALIDATE_FUNCTION_CALL(uint32_t, "GetAlignofType_12_4", uint32_t(4));
	VALIDATE_FUNCTION_CALL(uint32_t, "GetAlignofExpression_12_4", uint32_t(4));
	VALIDATE_FUNCTION_CALL(uint32_t, "GetSizeofType_12_4", uint32_t(12));
	VALIDATE_FUNCTION_CALL(uint32_t, "GetSizeofExpression_12_4", uint32_t(12));

	VALIDATE_FUNCTION_CALL(uint32_t, "GetAlignofType_32_16", uint32_t(16));
	VALIDATE_FUNCTION_CALL(uint32_t, "GetAlignofExpression_32_16", uint32_t(16));
	VALIDATE_FUNCTION_CALL(uint32_t, "GetSizeofType_32_16", uint32_t(32));
	VALIDATE_FUNCTION_CALL(uint32_t, "GetSizeofExpression_32_16", uint32_t(32));

	VALIDATE_FUNCTION_CALL(uint32_t, "GetAlignofType_48_16", uint32_t(16));
	VALIDATE_FUNCTION_CALL(uint32_t, "GetAlignofExpression_48_16", uint32_t(16));
	VALIDATE_FUNCTION_CALL(uint32_t, "GetSizeofType_48_16", uint32_t(48));
	VALIDATE_FUNCTION_CALL(uint32_t, "GetSizeofExpression_48_16", uint32_t(48));

	VALIDATE_FUNCTION_CALL(uint32_t, "GetAlignofArray_48_16", uint32_t(16));
	VALIDATE_FUNCTION_CALL(uint32_t, "GetSizeofArray_48_16", uint32_t(192));
}


DEFINE_VM_TEST(Branches, "scripts/vm_Branches.bond")
{
	using namespace Bond;

	VALIDATE_FUNCTION_CALL(uint32_t, "And", uint32_t(0), uint8_t(0), uint8_t(0));
	VALIDATE_FUNCTION_CALL(uint32_t, "And", uint32_t(0), uint8_t(0), uint8_t(1));
	VALIDATE_FUNCTION_CALL(uint32_t, "And", uint32_t(0), uint8_t(1), uint8_t(0));
	VALIDATE_FUNCTION_CALL(uint32_t, "And", uint32_t(1), uint8_t(1), uint8_t(1));

	VALIDATE_FUNCTION_CALL(uint32_t, "Or", uint32_t(0), uint8_t(0), uint8_t(0));
	VALIDATE_FUNCTION_CALL(uint32_t, "Or", uint32_t(1), uint8_t(0), uint8_t(1));
	VALIDATE_FUNCTION_CALL(uint32_t, "Or", uint32_t(1), uint8_t(1), uint8_t(0));
	VALIDATE_FUNCTION_CALL(uint32_t, "Or", uint32_t(1), uint8_t(1), uint8_t(1));

	VALIDATE_FUNCTION_CALL(int32_t, "AndShortCircuit", int32_t(4), uint8_t(0));
	VALIDATE_FUNCTION_CALL(int32_t, "AndShortCircuit", int32_t(5), uint8_t(1));

	VALIDATE_FUNCTION_CALL(int32_t, "OrShortCircuit", int32_t(5), uint8_t(0));
	VALIDATE_FUNCTION_CALL(int32_t, "OrShortCircuit", int32_t(4), uint8_t(1));

	VALIDATE_FUNCTION_CALL(int32_t, "Ternary", int32_t(5), uint8_t(0));
	VALIDATE_FUNCTION_CALL(int32_t, "Ternary", int32_t(4), uint8_t(1));

	VALIDATE_FUNCTION_CALL(int32_t, "If", int32_t(5), uint8_t(0));
	VALIDATE_FUNCTION_CALL(int32_t, "If", int32_t(4), uint8_t(1));

	VALIDATE_FUNCTION_CALL(int32_t, "IfNot", int32_t(4), uint8_t(0));
	VALIDATE_FUNCTION_CALL(int32_t, "IfNot", int32_t(5), uint8_t(1));

	VALIDATE_FUNCTION_CALL(int32_t, "IfNotAAndNotB", int32_t(4), uint8_t(0), uint8_t(0));
	VALIDATE_FUNCTION_CALL(int32_t, "IfNotAAndNotB", int32_t(5), uint8_t(0), uint8_t(1));
	VALIDATE_FUNCTION_CALL(int32_t, "IfNotAAndNotB", int32_t(5), uint8_t(1), uint8_t(0));
	VALIDATE_FUNCTION_CALL(int32_t, "IfNotAAndNotB", int32_t(5), uint8_t(1), uint8_t(1));

	VALIDATE_FUNCTION_CALL(int32_t, "IfNotAOrNotB", int32_t(4), uint8_t(0), uint8_t(0));
	VALIDATE_FUNCTION_CALL(int32_t, "IfNotAOrNotB", int32_t(4), uint8_t(0), uint8_t(1));
	VALIDATE_FUNCTION_CALL(int32_t, "IfNotAOrNotB", int32_t(4), uint8_t(1), uint8_t(0));
	VALIDATE_FUNCTION_CALL(int32_t, "IfNotAOrNotB", int32_t(5), uint8_t(1), uint8_t(1));

	VALIDATE_FUNCTION_CALL(int32_t, "IfElse", int32_t(5), uint8_t(0));
	VALIDATE_FUNCTION_CALL(int32_t, "IfElse", int32_t(4), uint8_t(1));

	VALIDATE_FUNCTION_CALL(int32_t, "ElseIf", int32_t(6), uint8_t(0), uint8_t(0));
	VALIDATE_FUNCTION_CALL(int32_t, "ElseIf", int32_t(5), uint8_t(0), uint8_t(1));
	VALIDATE_FUNCTION_CALL(int32_t, "ElseIf", int32_t(4), uint8_t(1), uint8_t(0));
	VALIDATE_FUNCTION_CALL(int32_t, "ElseIf", int32_t(4), uint8_t(1), uint8_t(1));

	VALIDATE_FUNCTION_CALL(int32_t, "NestedIf", int32_t(7), uint8_t(0), uint8_t(0));
	VALIDATE_FUNCTION_CALL(int32_t, "NestedIf", int32_t(6), uint8_t(0), uint8_t(1));
	VALIDATE_FUNCTION_CALL(int32_t, "NestedIf", int32_t(5), uint8_t(1), uint8_t(0));
	VALIDATE_FUNCTION_CALL(int32_t, "NestedIf", int32_t(4), uint8_t(1), uint8_t(1));

	VALIDATE_FUNCTION_CALL(int32_t, "LookupSwitchDefault", int32_t(8), int32_t(0));
	VALIDATE_FUNCTION_CALL(int32_t, "LookupSwitchDefault", int32_t(5), int32_t(1));
	VALIDATE_FUNCTION_CALL(int32_t, "LookupSwitchDefault", int32_t(8), int32_t(2));
	VALIDATE_FUNCTION_CALL(int32_t, "LookupSwitchDefault", int32_t(6), int32_t(3));
	VALIDATE_FUNCTION_CALL(int32_t, "LookupSwitchDefault", int32_t(8), int32_t(4));
	VALIDATE_FUNCTION_CALL(int32_t, "LookupSwitchDefault", int32_t(8), int32_t(99));
	VALIDATE_FUNCTION_CALL(int32_t, "LookupSwitchDefault", int32_t(7), int32_t(100));
	VALIDATE_FUNCTION_CALL(int32_t, "LookupSwitchDefault", int32_t(8), int32_t(101));

	VALIDATE_FUNCTION_CALL(int32_t, "LookupSwitchNoDefault", int32_t(4), int32_t(0));
	VALIDATE_FUNCTION_CALL(int32_t, "LookupSwitchNoDefault", int32_t(5), int32_t(1));
	VALIDATE_FUNCTION_CALL(int32_t, "LookupSwitchNoDefault", int32_t(4), int32_t(2));
	VALIDATE_FUNCTION_CALL(int32_t, "LookupSwitchNoDefault", int32_t(6), int32_t(3));
	VALIDATE_FUNCTION_CALL(int32_t, "LookupSwitchNoDefault", int32_t(4), int32_t(4));
	VALIDATE_FUNCTION_CALL(int32_t, "LookupSwitchNoDefault", int32_t(4), int32_t(99));
	VALIDATE_FUNCTION_CALL(int32_t, "LookupSwitchNoDefault", int32_t(7), int32_t(100));
	VALIDATE_FUNCTION_CALL(int32_t, "LookupSwitchNoDefault", int32_t(4), int32_t(101));

	VALIDATE_FUNCTION_CALL(int32_t, "TableSwitchDefault", int32_t(9), int32_t(6));
	VALIDATE_FUNCTION_CALL(int32_t, "TableSwitchDefault", int32_t(5), int32_t(7));
	VALIDATE_FUNCTION_CALL(int32_t, "TableSwitchDefault", int32_t(6), int32_t(8));
	VALIDATE_FUNCTION_CALL(int32_t, "TableSwitchDefault", int32_t(9), int32_t(9));
	VALIDATE_FUNCTION_CALL(int32_t, "TableSwitchDefault", int32_t(7), int32_t(10));
	VALIDATE_FUNCTION_CALL(int32_t, "TableSwitchDefault", int32_t(8), int32_t(11));
	VALIDATE_FUNCTION_CALL(int32_t, "TableSwitchDefault", int32_t(9), int32_t(12));

	VALIDATE_FUNCTION_CALL(int32_t, "TableSwitchNoDefault", int32_t(4), int32_t(6));
	VALIDATE_FUNCTION_CALL(int32_t, "TableSwitchNoDefault", int32_t(5), int32_t(7));
	VALIDATE_FUNCTION_CALL(int32_t, "TableSwitchNoDefault", int32_t(6), int32_t(8));
	VALIDATE_FUNCTION_CALL(int32_t, "TableSwitchNoDefault", int32_t(4), int32_t(9));
	VALIDATE_FUNCTION_CALL(int32_t, "TableSwitchNoDefault", int32_t(7), int32_t(10));
	VALIDATE_FUNCTION_CALL(int32_t, "TableSwitchNoDefault", int32_t(8), int32_t(11));
	VALIDATE_FUNCTION_CALL(int32_t, "TableSwitchNoDefault", int32_t(4), int32_t(12));

	VALIDATE_FUNCTION_CALL(int32_t, "While", int32_t(5), int32_t(5));
	VALIDATE_FUNCTION_CALL(int32_t, "For", int32_t(5), int32_t(5));

	VALIDATE_FUNCTION_CALL(int32_t, "NestedLoopsWithJumps", int32_t(360), int32_t(5), int32_t(8));
}


DEFINE_VM_TEST(Pointers, "scripts/vm_Pointers.bond")
{
	using namespace Bond;

	int32_t oneD[8];
	memset(oneD, 0, sizeof(oneD));

	vm.CallVoidFunction("SetValueAt_3", &oneD, int32_t(7));
	ASSERT_FORMAT(oneD[3] == 7, "Expected 7, but was %" BOND_PRId32 ".", oneD[3]);
	ASSERT_MESSAGE((oneD[1] == 0) && (oneD[2] == 0) && (oneD[4] == 0) && (oneD[5] == 0),
		"Memory stomp by SetValueAt_3() detected.");

	vm.CallVoidFunction("SetValueAt", &oneD, int32_t(5), int32_t(864));
	ASSERT_FORMAT(oneD[5] == 864, "Expected 864, but was %" BOND_PRId32 ".", oneD[5]);
	ASSERT_MESSAGE((oneD[3] == 7) && (oneD[4] == 0) && (oneD[6] == 0) && (oneD[7] == 0),
		"Memory stomp by SetValueAt() detected.");

	oneD[4] = 2468;
	VALIDATE_FUNCTION_CALL(int32_t, "GetValueAt_4", oneD[4], &oneD);

	oneD[6] = 753;
	VALIDATE_FUNCTION_CALL(int32_t, "GetValueAt", oneD[6], &oneD, int32_t(6));

	int32_t twoD[7][13];
	memset(twoD, 0, sizeof(twoD));

	vm.CallVoidFunction("SetValue2DAt_4_9", &twoD, int32_t(901));
	ASSERT_FORMAT(twoD[4][9] == 901, "Expected 901, but was %" BOND_PRId32 ".", twoD[4][9]);
	ASSERT_MESSAGE((twoD[3][8] == 0) && (twoD[3][9] == 0) && (twoD[3][10] == 0) &&
		(twoD[4][8] == 0) && (twoD[4][10] == 0) &&
		(twoD[5][8] == 0) && (twoD[5][9] == 0) && (twoD[5][10] == 0),
		"Memory stomp by SetValue2DAt_4_9() detected.");

	vm.CallVoidFunction("SetValue2DAt", &twoD, int32_t(2), int32_t(4), int32_t(98973));
	ASSERT_FORMAT(twoD[2][4] == 98973, "Expected 98973, but was %" BOND_PRId32 ".", twoD[2][4]);
	ASSERT_MESSAGE((twoD[1][3] == 0) && (twoD[1][4] == 0) && (twoD[1][5] == 0) &&
		(twoD[2][3] == 0) && (twoD[2][5] == 0) &&
		(twoD[3][3] == 0) && (twoD[3][4] == 0) && (twoD[3][5] == 0),
		"Memory stomp by SetValue2DAt() detected.");

	twoD[1][12] = 1357;
	VALIDATE_FUNCTION_CALL(int32_t, "GetValue2DAt_1_12", twoD[1][12], &twoD);

	twoD[6][11] = 9182;
	VALIDATE_FUNCTION_CALL(int32_t, "GetValue2DAt", twoD[6][11], &twoD, int32_t(6), int32_t(11));

	VALIDATE_FUNCTION_CALL(const void *, "PointerOffset", &twoD[1][2] + 19, &twoD[1][2], int32_t(19));
	// The pointer is never dereferenced, so compute the expected address with integer arithmetic.
	const char *uintOffsetBase = reinterpret_cast<const char *>(&twoD[0][0]);
	const char *uintOffsetResult = reinterpret_cast<const char *>(reinterpret_cast<uintptr_t>(uintOffsetBase) + uintptr_t(0x80000000u));
	VALIDATE_FUNCTION_CALL(const void *, "UIntPointerOffset", uintOffsetResult, uintOffsetBase, uint32_t(0x80000000u));
	VALIDATE_FUNCTION_CALL(int32_t, "LocalPointerDifference", int32_t(4));
	VALIDATE_FUNCTION_CALL(int32_t, "PointerDifference", int32_t(&twoD[6][9] - &twoD[1][3]), &twoD[6][9], &twoD[1][3]);

	uint8_t aggregate[500000];
	VALIDATE_FUNCTION_CALL(const void *, "SmallStructPointerOffset", &aggregate[5 * 32], aggregate, int32_t(5));
	VALIDATE_FUNCTION_CALL(int32_t, "SmallStructPointerDifference", 9, &aggregate[9 * 32], aggregate);

	VALIDATE_FUNCTION_CALL(const void *, "LargeStructPointerOffset", &aggregate[4 * 100000], aggregate, int32_t(4));
	VALIDATE_FUNCTION_CALL(int32_t, "LargeStructPointerDifference", 3, &aggregate[3 * 100000], aggregate);

	VALIDATE_FUNCTION_CALL(const void *, "PreincPointerParam", &oneD[5], &oneD[4]);
	VALIDATE_FUNCTION_CALL(const void *, "PostincPointerParam", &oneD[4], &oneD[4]);
	VALIDATE_FUNCTION_CALL(const void *, "PredecPointerParam", &oneD[3], &oneD[4]);
	VALIDATE_FUNCTION_CALL(const void *, "PostdecPointerParam", &oneD[4], &oneD[4]);
	VALIDATE_FUNCTION_CALL(const void *, "AddToPointerParam", &oneD[6], &oneD[4]);
	VALIDATE_FUNCTION_CALL(const void *, "SubFromPointerParam", &oneD[2], &oneD[4]);
	VALIDATE_FUNCTION_CALL(const void *, "PredecLocalPointer", &oneD[3], &oneD[4]);
	VALIDATE_FUNCTION_CALL(const void *, "PostdecLocalPointer", &oneD[3], &oneD[4]);

	const int32_t *oneDPtr = &oneD[4];
	VALIDATE_FUNCTION_CALL(const void *, "PredecIndirectPointer", &oneD[3], &oneDPtr);
	ASSERT_MESSAGE(oneDPtr == &oneD[3], "PredecIndirectPointer() did not store the decremented pointer.");
	VALIDATE_FUNCTION_CALL(const void *, "PostdecIndirectPointer", &oneD[3], &oneDPtr);
	ASSERT_MESSAGE(oneDPtr == &oneD[2], "PostdecIndirectPointer() did not store the decremented pointer.");

	VALIDATE_FUNCTION_CALL(const void *, "PredecLargeStructPointer", &aggregate[2 * 100000], &aggregate[3 * 100000]);
	VALIDATE_FUNCTION_CALL(const void *, "PostdecLargeStructPointer", &aggregate[2 * 100000], &aggregate[3 * 100000]);

	VALIDATE_FUNCTION_CALL(int32_t, "LocalAlignment", int32_t(64));
	VALIDATE_FUNCTION_CALL(int32_t, "LocalOverlap", int32_t(1));
}


DEFINE_VM_TEST(CompoundAssignmentOperators, "scripts/vm_CompoundAssignmentOperators.bond")
{
	using namespace Bond;

	int32_t i = -97; int32_t oi = 27; int32_t ri = i + oi;
	VALIDATE_FUNCTION_CALL(int32_t, "Addi", ri, &i, oi);
	ASSERT_FORMAT(i == ri, "Expected %" BOND_PRId32 ", but was %" BOND_PRId32 ".", ri, i);

	uint32_t u = 97; uint32_t ou = 27; uint32_t ru = u + ou;
	VALIDATE_FUNCTION_CALL(uint32_t, "Addi", ru, &u, ou);
	ASSERT_FORMAT(u == ru, "Expected %" BOND_PRIu32 ", but was %" BOND_PRIu32 ".", ru, u);

	int64_t l = -(int64_t(97) << 40); int64_t ol = int64_t(27) << 40; int64_t rl = l + ol;
	VALIDATE_FUNCTION_CALL(int64_t, "Addl", rl, &l, ol);
	ASSERT_FORMAT(l == rl, "Expected %" BOND_PRId64 ", but was %" BOND_PRId64 ".", rl, l);

	uint64_t ul = uint64_t(97) << 40; uint64_t oul = uint64_t(27) << 40; uint64_t rul = ul + oul;
	VALIDATE_FUNCTION_CALL(uint64_t, "Addl", rul, &ul, oul);
	ASSERT_FORMAT(ul == rul, "Expected %" BOND_PRId64 ", but was %" BOND_PRId64 ".", rul, ul);

	float f = -97.0f; float of = 27.0f; float rf = f + of;
	VALIDATE_FUNCTION_CALL(float, "Addf", rf, &f, of);
	ASSERT_FORMAT(f == rf, "Expected %f, but was %f.", rf, f);

	double d = -97.0; double od = 27.0; double rd = d + od;
	VALIDATE_FUNCTION_CALL(double, "Addd", rd, &d, od);
	ASSERT_FORMAT(d == rd, "Expected %f, but was %.", rd, d);


	i = -85; oi = 39; ri = i - oi;
	VALIDATE_FUNCTION_CALL(int32_t, "Subi", ri, &i, oi);
	ASSERT_FORMAT(i == ri, "Expected %" BOND_PRId32 ", but was %" BOND_PRId32 ".", ri, i);

	u = 85; ou = 39; ru = u - ou;
	VALIDATE_FUNCTION_CALL(uint32_t, "Subi", ru, &u, ou);
	ASSERT_FORMAT(u == ru, "Expected %" BOND_PRIu32 ", but was %" BOND_PRIu32 ".", ru, u);

	l = -(int64_t(85) << 40); ol = int64_t(39) << 40; rl = l - ol;
	VALIDATE_FUNCTION_CALL(int64_t, "Subl", rl, &l, ol);
	ASSERT_FORMAT(l == rl, "Expected %" BOND_PRId64 ", but was %" BOND_PRId64 ".", rl, l);

	ul = uint64_t(85) << 40; oul = uint64_t(39) << 40; rul = ul - oul;
	VALIDATE_FUNCTION_CALL(uint64_t, "Subl", rul, &ul, oul);
	ASSERT_FORMAT(ul == rul, "Expected %" BOND_PRId64 ", but was %" BOND_PRId64 ".", rul, ul);

	f = -85.0f; of = 39.0f; rf = f - of;
	VALIDATE_FUNCTION_CALL(float, "Subf", rf, &f, of);
	ASSERT_FORMAT(f == rf, "Expected %f, but was %f.", rf, f);

	d = -85.0; od = 39.0; rd = d - od;
	VALIDATE_FUNCTION_CALL(double, "Subd", rd, &d, od);
	ASSERT_FORMAT(d == rd, "Expected %f, but was %f.", rd, d);


	i = -48; oi = 135; ri = i * oi;
	VALIDATE_FUNCTION_CALL(int32_t, "Muli", ri, &i, oi);
	ASSERT_FORMAT(i == ri, "Expected %" BOND_PRId32 ", but was %" BOND_PRId32 ".", ri, i);

	u = 48; ou = 135; ru = u * ou;
	VALIDATE_FUNCTION_CALL(uint32_t, "Mului", ru, &u, ou);
	ASSERT_FORMAT(u == ru, "Expected %" BOND_PRIu32 ", but was %" BOND_PRIu32 ".", ru, u);

	u = 0xFFFFFFFFu; ou = 3; ru = u * ou;
	VALIDATE_FUNCTION_CALL(uint32_t, "Mului", ru, &u, ou);
	ASSERT_FORMAT(u == ru, "Expected %" BOND_PRIu32 ", but was %" BOND_PRIu32 ".", ru, u);

	l = -(int64_t(48) << 40); ol = int64_t(135) << 40; rl = l * ol;
	VALIDATE_FUNCTION_CALL(int64_t, "Mull", rl, &l, ol);
	ASSERT_FORMAT(l == rl, "Expected %" BOND_PRId64 ", but was %" BOND_PRId64 ".", rl, l);

	ul = uint64_t(48) << 40; oul = uint64_t(135) << 40; rul = ul * oul;
	VALIDATE_FUNCTION_CALL(uint64_t, "Mulul", rul, &ul, oul);
	ASSERT_FORMAT(ul == rul, "Expected %" BOND_PRId64 ", but was %" BOND_PRId64 ".", rul, ul);

	ul = ~uint64_t(0); oul = 3; rul = ul * oul;
	VALIDATE_FUNCTION_CALL(uint64_t, "Mulul", rul, &ul, oul);
	ASSERT_FORMAT(ul == rul, "Expected %" BOND_PRIu64 ", but was %" BOND_PRIu64 ".", rul, ul);

	f = -48.0f; of = 135.0f; rf = f * of;
	VALIDATE_FUNCTION_CALL(float, "Mulf", rf, &f, of);
	ASSERT_FORMAT(f == rf, "Expected %f, but was %f.", rf, f);

	d = -48.0; od = 135.0; rd = d * od;
	VALIDATE_FUNCTION_CALL(double, "Muld", rd, &d, od);
	ASSERT_FORMAT(d == rd, "Expected %f, but was %f.", rd, d);


	i = -10; oi = 3; ri = i / oi;
	VALIDATE_FUNCTION_CALL(int32_t, "Divi", ri, &i, oi);
	ASSERT_FORMAT(i == ri, "Expected %" BOND_PRId32 ", but was %" BOND_PRId32 ".", ri, i);

	u = 10; ou = 3; ru = u / ou;
	VALIDATE_FUNCTION_CALL(uint32_t, "Divui", ru, &u, ou);
	ASSERT_FORMAT(u == ru, "Expected %" BOND_PRIu32 ", but was %" BOND_PRIu32 ".", ru, u);

	l = -(int64_t(10) << 40); ol = int64_t(3); rl = l / ol;
	VALIDATE_FUNCTION_CALL(int64_t, "Divl", rl, &l, ol);
	ASSERT_FORMAT(l == rl, "Expected %" BOND_PRId64 ", but was %" BOND_PRId64 ".", rl, l);

	ul = uint64_t(10) << 40; oul = uint64_t(3); rul = ul / oul;
	VALIDATE_FUNCTION_CALL(uint64_t, "Divul", rul, &ul, oul);
	ASSERT_FORMAT(ul == rul, "Expected %" BOND_PRId64 ", but was %" BOND_PRId64 ".", rul, ul);

	f = -10.0f; of = 3.0f; rf = f / of;
	VALIDATE_FUNCTION_CALL(float, "Divf", rf, &f, of);
	ASSERT_FORMAT(f == rf, "Expected %f, but was %f.", rf, f);

	d = -10.0; od = 3.0; rd = d / od;
	VALIDATE_FUNCTION_CALL(double, "Divd", rd, &d, od);
	ASSERT_FORMAT(d == rd, "Expected %f, but was %f.", rd, d);


	i = -10; oi = 3; ri = i % oi;
	VALIDATE_FUNCTION_CALL(int32_t, "Remi", ri, &i, oi);
	ASSERT_FORMAT(i == ri, "Expected %" BOND_PRId32 ", but was %" BOND_PRId32 ".", ri, i);

	u = 10; ou = 3; ru = u % ou;
	VALIDATE_FUNCTION_CALL(uint32_t, "Remui", ru, &u, ou);
	ASSERT_FORMAT(u == ru, "Expected %" BOND_PRIu32 ", but was %" BOND_PRIu32 ".", ru, u);

	l = -(int64_t(10) << 40); ol = int64_t(34); rl = l % ol;
	VALIDATE_FUNCTION_CALL(int64_t, "Reml", rl, &l, ol);
	ASSERT_FORMAT(l == rl, "Expected %" BOND_PRId64 ", but was %" BOND_PRId64 ".", rl, l);

	ul = uint64_t(10) << 40; oul = uint64_t(34); rul = ul % oul;
	VALIDATE_FUNCTION_CALL(uint64_t, "Remul", rul, &ul, oul);
	ASSERT_FORMAT(ul == rul, "Expected %" BOND_PRId64 ", but was %" BOND_PRId64 ".", rul, ul);


	i = 7; oi = 7; ri = i << oi;
	VALIDATE_FUNCTION_CALL(int32_t, "Lshi", ri, &i, oi);
	ASSERT_FORMAT(i == ri, "Expected %" BOND_PRId32 ", but was %" BOND_PRId32 ".", ri, i);

	l = 7; ol = 50; rl = l << ol;
	VALIDATE_FUNCTION_CALL(int64_t, "Lshl", rl, &l, ol);
	ASSERT_FORMAT(l == rl, "Expected %" BOND_PRId64 ", but was %" BOND_PRId64 ".", rl, l);


	i = -(int32_t(1) << 20); oi = 16; ri = i >> oi;
	VALIDATE_FUNCTION_CALL(int32_t, "Rshi", ri, &i, oi);
	ASSERT_FORMAT(i == ri, "Expected %" BOND_PRId32 ", but was %" BOND_PRId32 ".", ri, i);

	u = uint32_t(-(int32_t(1) << 20)); ou = 16; ru = u >> ou;
	VALIDATE_FUNCTION_CALL(uint32_t, "Rshui", ru, &u, ou);
	ASSERT_FORMAT(u == ru, "Expected %" BOND_PRIu32 ", but was %" BOND_PRIu32 ".", ru, u);

	l = -(int64_t(1) << 20); ol = 16; rl = l >> ol;
	VALIDATE_FUNCTION_CALL(int64_t, "Rshl", rl, &l, ol);
	ASSERT_FORMAT(l == rl, "Expected %" BOND_PRId64 ", but was %" BOND_PRId64 ".", rl, l);

	ul = uint64_t(-(int64_t(1) << 20)); oul = 16; rul = ul >> oul;
	VALIDATE_FUNCTION_CALL(uint64_t, "Rshul", rul, &ul, oul);
	ASSERT_FORMAT(ul == rul, "Expected %" BOND_PRId64 ", but was %" BOND_PRId64 ".", rul, ul);

	u = 0x80000000; oi = 4; ru = 0x08000000;
	VALIDATE_FUNCTION_CALL(uint32_t, "RshuiInt", ru, &u, oi);
	ASSERT_FORMAT(u == ru, "Expected %" BOND_PRIu32 ", but was %" BOND_PRIu32 ".", ru, u);

	l = 1; oi = 40; rl = int64_t(1) << 40;
	VALIDATE_FUNCTION_CALL(int64_t, "LshlInt", rl, &l, oi);
	ASSERT_FORMAT(l == rl, "Expected %" BOND_PRId64 ", but was %" BOND_PRId64 ".", rl, l);


	i = 0xfff0; oi = 0xff; ri = i & oi;
	VALIDATE_FUNCTION_CALL(int32_t, "Andi", ri, &i, oi);
	ASSERT_FORMAT(i == ri, "Expected %" BOND_PRId32 ", but was %" BOND_PRId32 ".", ri, i);

	l = int64_t(0xfff0) << 31; ol = int64_t(0xff) << 31; rl = l & ol;
	VALIDATE_FUNCTION_CALL(int64_t, "Andl", rl, &l, ol);
	ASSERT_FORMAT(l == rl, "Expected %" BOND_PRId64 ", but was %" BOND_PRId64 ".", rl, l);


	i = 0xfff0; oi = 0xff; ri = i | oi;
	VALIDATE_FUNCTION_CALL(int32_t, "Ori", ri, &i, oi);
	ASSERT_FORMAT(i == ri, "Expected %" BOND_PRId32 ", but was %" BOND_PRId32 ".", ri, i);

	l = int64_t(0xfff0) << 31; ol = int64_t(0xff) << 31; rl = l | ol;
	VALIDATE_FUNCTION_CALL(int64_t, "Orl", rl, &l, ol);
	ASSERT_FORMAT(l == rl, "Expected %" BOND_PRId64 ", but was %" BOND_PRId64 ".", rl, l);


	i = 0xfff0; oi = 0xff; ri = i ^ oi;
	VALIDATE_FUNCTION_CALL(int32_t, "Xori", ri, &i, oi);
	ASSERT_FORMAT(i == ri, "Expected %" BOND_PRId32 ", but was %" BOND_PRId32 ".", ri, i);

	l = int64_t(0xfff0) << 31; ol = int64_t(0xff) << 31; rl = l ^ ol;
	VALIDATE_FUNCTION_CALL(int64_t, "Xorl", rl, &l, ol);
	ASSERT_FORMAT(l == rl, "Expected %" BOND_PRId64 ", but was %" BOND_PRId64 ".", rl, l);


	VALIDATE_FUNCTION_CALL(int32_t, "AddFloatToLocal", int32_t(-3 + 0.5f), int32_t(-3));
	VALIDATE_FUNCTION_CALL(int32_t, "SubDoubleFromLocal", int32_t(3 - 0.5), int32_t(3));

	VALIDATE_FUNCTION_CALL(int32_t, "AddToParam", int32_t(2), int32_t(-3));
	VALIDATE_FUNCTION_CALL(int32_t, "SubFromParamValue", int32_t(-10), int32_t(-3));
	VALIDATE_FUNCTION_CALL(int64_t, "AddToLongParam", (int64_t(3) << 40) + 100, int64_t(3) << 40);
	VALIDATE_FUNCTION_CALL(int64_t, "SubFromLongParamValue", (int64_t(3) << 40) - 100, int32_t(1), int64_t(3) << 40);
}


DEFINE_VM_TEST(FunctionCalls, "scripts/vm_FunctionCalls.bond")
{
	using namespace Bond;

	VALIDATE_FUNCTION_CALL(uint32_t, "FibonacciRecursive", uint32_t(6765), uint32_t(20));
	VALIDATE_FUNCTION_CALL(uint32_t, "TestSetCharValue", uint32_t(1));
	VALIDATE_FUNCTION_CALL(int32_t, "StrangeMath", int32_t((10 + 40) - (30 + 80)), int32_t(10), int32_t(40), int32_t(30), int32_t(80));
	VALIDATE_FUNCTION_CALL(int32_t, "TestOverAlignedFrames", int32_t(
		(-3 + 1000 - 200 + 50000) +
		(1 + (-4 + 1000 - 300 + 60000)) +
		(-5 - 400 + 70000) +
		(2 + (3 + (-6 - 500 + 80000)))));
}


DEFINE_VM_TEST(Structs, "scripts/vm_Structs.bond")
{
	using namespace Bond;

	Char3 c3;
	memset(&c3, 0, sizeof(c3));
	vm.CallVoidFunction("SetChar3", &c3, int8_t(45), int8_t(-56), int8_t(67));
	ASSERT_FORMAT(c3.x == 45, "Expected 45, but was %" BOND_PRId32 ".", c3.x);
	ASSERT_FORMAT(c3.y == -56, "Expected -56, but was %" BOND_PRId32 ".", c3.y);
	ASSERT_FORMAT(c3.z == 67, "Expected 67, but was %" BOND_PRId32 ".", c3.z);
	ASSERT_FORMAT(c3.padding == 0, "Expected 0, but was %" BOND_PRId32 ".", c3.padding);

	VALIDATE_FUNCTION_CALL(int32_t, "GetChar3X", int32_t(45), c3);
	VALIDATE_FUNCTION_CALL(int32_t, "GetChar3Y", int32_t(-56), c3);
	VALIDATE_FUNCTION_CALL(int32_t, "GetChar3Z", int32_t(67), c3);

	Char3Pair char3Pair;
	vm.CallVoidFunction("SetChar3PairX", &char3Pair, int8_t(23), int8_t(32));
	ASSERT_FORMAT(char3Pair.first.x == 23, "Expected 23, but was %" BOND_PRId32 ".", char3Pair.first.x);
	ASSERT_FORMAT(char3Pair.second.x == 32, "Expected 32, but was %" BOND_PRId32 ".", char3Pair.second.x);

	VALIDATE_FUNCTION_CALL(uint32_t, "InitializeChar3Pair", uint32_t(1));

	Vector3 v3a;
	Vector3 v3b;
	Vector3 v3c;
	memset(&v3a, 0, sizeof(v3a));
	vm.CallVoidFunction("SetVector3", &v3a, int32_t(11), int32_t(-12), int32_t(13));
	ASSERT_FORMAT(v3a.x == 11, "Expected 11, but was %" BOND_PRId32 ".", v3a.x);
	ASSERT_FORMAT(v3a.y == -12, "Expected -12, but was %" BOND_PRId32 ".", v3a.y);
	ASSERT_FORMAT(v3a.z == 13, "Expected 13, but was %" BOND_PRId32 ".", v3a.z);

	v3a.x = 88;
	v3a.y = 888;
	v3a.z = 8888;
	const char *vector3GetX[] = {"Vector3", "GetX", nullptr};
	VALIDATE_FUNCTION_CALL(int32_t, "GetVector3X", v3a.x, v3a);
	VALIDATE_FUNCTION_CALL(int32_t, QualifiedName(vector3GetX), v3a.x, &v3a);
	VALIDATE_FUNCTION_CALL(int32_t, "GetVector3Y", v3a.y, v3a);
	VALIDATE_FUNCTION_CALL(int32_t, "Vector3.GetY", v3a.y, &v3a);
	VALIDATE_FUNCTION_CALL(int32_t, "GetVector3Z", v3a.z, v3a);
	VALIDATE_FUNCTION_CALL(int32_t, "Vector3.GetZ", v3a.z, &v3a);

	v3a.x = 2;
	v3a.y = 3;
	v3a.z = 4;
	v3b.x = 5;
	v3b.y = -6;
	v3b.z = 7;
	VALIDATE_FUNCTION_CALL(int32_t, "Vector3.Dot", int32_t(20), &v3a, &v3b);

	vm.CallFunction("ConstructVector3", &v3a, int32_t(99), int32_t(-100), int32_t(101));
	ASSERT_FORMAT(v3a.x == 99, "Expected 99, but was %" BOND_PRId32 ".", v3a.x);
	ASSERT_FORMAT(v3a.y == -100, "Expected -100, but was %" BOND_PRId32 ".", v3a.y);
	ASSERT_FORMAT(v3a.z == 101, "Expected 101, but was %" BOND_PRId32 ".", v3a.z);

	vm.CallFunction("ConstructVector3Indirect", &v3a, int32_t(102), int32_t(-103), int32_t(104));
	ASSERT_FORMAT(v3a.x == 102, "Expected 102, but was %" BOND_PRId32 ".", v3a.x);
	ASSERT_FORMAT(v3a.y == -103, "Expected -103, but was %" BOND_PRId32 ".", v3a.y);
	ASSERT_FORMAT(v3a.z == 104, "Expected 104, but was %" BOND_PRId32 ".", v3a.z);

	VALIDATE_FUNCTION_CALL(int32_t, "ConstructVector3AndGetY", int32_t(12321), int32_t(12321));

	v3a.x = 31;
	v3a.y = 32;
	v3a.z = 33;
	v3b.x = 41;
	v3b.y = 42;
	v3b.z = 43;
	v3c.x = 51;
	v3c.y = 52;
	v3c.z = 53;

	vm.CallFunction("SelectVector3", &v3c, uint8_t(1), v3a, v3b);
	ASSERT_FORMAT(v3c.x == 31, "Expected 31, but was %" BOND_PRId32 ".", v3c.x);
	ASSERT_FORMAT(v3c.y == 32, "Expected 32, but was %" BOND_PRId32 ".", v3c.y);
	ASSERT_FORMAT(v3c.z == 33, "Expected 33, but was %" BOND_PRId32 ".", v3c.z);

	vm.CallFunction("SelectVector3", &v3c, uint8_t(0), v3a, v3b);
	ASSERT_FORMAT(v3c.x == 41, "Expected 41, but was %" BOND_PRId32 ".", v3c.x);
	ASSERT_FORMAT(v3c.y == 42, "Expected 42, but was %" BOND_PRId32 ".", v3c.y);
	ASSERT_FORMAT(v3c.z == 43, "Expected 43, but was %" BOND_PRId32 ".", v3c.z);
}


DEFINE_VM_TEST(Initializers, "scripts/vm_Initializers.bond")
{
	using namespace Bond;

	VALIDATE_FUNCTION_CALL(int32_t, "GetVecXAt", int32_t(121), uint32_t(0));
	VALIDATE_FUNCTION_CALL(int32_t, "GetVecYAt", int32_t(123), uint32_t(0));

	VALIDATE_FUNCTION_CALL(int32_t, "GetVecXAt", int32_t(125), uint32_t(1));
	VALIDATE_FUNCTION_CALL(int32_t, "GetVecYAt", int32_t(0), uint32_t(1));

	VALIDATE_FUNCTION_CALL(int32_t, "GetVecXAt", int32_t(127), uint32_t(2));
	VALIDATE_FUNCTION_CALL(int32_t, "GetVecYAt", int32_t(129), uint32_t(2));

	VALIDATE_FUNCTION_CALL(int32_t, "GetVecXAt", int32_t(0), uint32_t(3));
	VALIDATE_FUNCTION_CALL(int32_t, "GetVecYAt", int32_t(0), uint32_t(3));

	VALIDATE_FUNCTION_CALL(uint32_t, "GetCollectionSize", int32_t(4));

	const int32_t *vectors = nullptr;
	vm.CallFunction("GetCollectionVectors", &vectors);

	ASSERT_FORMAT(vectors[0] == 121, "Expected 121, but was %" BOND_PRId32 ".", vectors[0]);
	ASSERT_FORMAT(vectors[1] == 123, "Expected 123, but was %" BOND_PRId32 ".", vectors[1]);
	ASSERT_FORMAT(vectors[2] == 125, "Expected 125, but was %" BOND_PRId32 ".", vectors[2]);
	ASSERT_FORMAT(vectors[3] == 0,   "Expected 0, but was %" BOND_PRId32 ".",   vectors[3]);
	ASSERT_FORMAT(vectors[4] == 127, "Expected 127, but was %" BOND_PRId32 ".", vectors[4]);
	ASSERT_FORMAT(vectors[5] == 129, "Expected 129, but was %" BOND_PRId32 ".", vectors[5]);
	ASSERT_FORMAT(vectors[6] == 0,   "Expected 0, but was %" BOND_PRId32 ".",   vectors[6]);
	ASSERT_FORMAT(vectors[7] == 0,   "Expected 0, but was %" BOND_PRId32 ".",   vectors[7]);

	const char *testStr = nullptr;
	vm.CallFunction("GetTestString", &testStr);
	ASSERT_MESSAGE((testStr != nullptr) && (strcmp(testStr, "Hooray! The test passed!") == 0),
		"GetTestString() return an incorrect string.");

	vm.CallFunction("GetTestString2", &testStr);
	ASSERT_MESSAGE((testStr != nullptr) && (strcmp(testStr, "Hooray! This test passed too!") == 0),
		"GetTestString2() return an incorrect string.");

	VALIDATE_FUNCTION_CALL(int32_t, "TestLocalStringInitializer", int32_t(1));
}


DEFINE_VM_TEST(Enums, "scripts/vm_Enums.bond")
{
	using namespace Bond;

	VALIDATE_FUNCTION_CALL(int32_t, "GetImplicitTwo", int32_t(2));
	VALIDATE_FUNCTION_CALL(int32_t, "GetExplicitA", int32_t(3));
	VALIDATE_FUNCTION_CALL(int32_t, "GetExplicitB", int32_t(4));
	VALIDATE_FUNCTION_CALL(int32_t, "GetExplicitC", int32_t(11));
	VALIDATE_FUNCTION_CALL(int32_t, "GetExplicitD", int32_t(-1));
	VALIDATE_FUNCTION_CALL(int32_t, "GetExplicitE", int32_t(0));
	VALIDATE_FUNCTION_CALL(int32_t, "GetNestedB", int32_t(40));
	VALIDATE_FUNCTION_CALL(int32_t, "SwitchOnEnum", int32_t(1), int32_t(3));
	VALIDATE_FUNCTION_CALL(int32_t, "SwitchOnEnum", int32_t(2), int32_t(11));
	VALIDATE_FUNCTION_CALL(int32_t, "SwitchOnEnum", int32_t(3), int32_t(40));
	VALIDATE_FUNCTION_CALL(int32_t, "SwitchOnEnum", int32_t(0), int32_t(5));
	VALIDATE_FUNCTION_CALL(uint32_t, "GetArrayLength", uint32_t(11));
	VALIDATE_FUNCTION_CALL(int32_t, "GetEnumLocal", int32_t(11));
	VALIDATE_FUNCTION_CALL(int32_t, "GetEnumParameterAndReturn", int32_t(6));
	VALIDATE_FUNCTION_CALL(int32_t, "GetEnumStructMember", int32_t(-1));
}


DEFINE_VM_TEST(ArithmeticConversions, "scripts/vm_ArithmeticConversions.bond")
{
	using namespace Bond;

	// The usual arithmetic conversions: an int operand converts to the unsigned type of the other operand.
	VALIDATE_FUNCTION_CALL(uint32_t, "DivuiByIntConstant", uint32_t(1500000000u), uint32_t(3000000000u));
	VALIDATE_FUNCTION_CALL(uint32_t, "RemuiByIntConstant", uint32_t(0), uint32_t(3000000000u));
	VALIDATE_FUNCTION_CALL(int32_t, "CmpgtuiIntConstant", int32_t(1), uint32_t(3000000000u));
	VALIDATE_FUNCTION_CALL(uint32_t, "DivuiAssignIntConstant", uint32_t(1500000000u), uint32_t(3000000000u));
	VALIDATE_FUNCTION_CALL(int64_t, "AdduiIntConstantToLong", int64_t(3000000001), uint32_t(3000000000u));
	VALIDATE_FUNCTION_CALL(int32_t, "CmpgtulLongConstant", int32_t(1), uint64_t(10000000000000000000u));
	VALIDATE_FUNCTION_CALL(uint64_t, "DivulByIntConstant", uint64_t(5000000000000000000u), uint64_t(10000000000000000000u));
	VALIDATE_FUNCTION_CALL(int64_t, "AdducUiToLong", int64_t(0), uint8_t(1), BOND_UINT_MAX);

	// The integer promotions: uchar and ushort promote to int.
	VALIDATE_FUNCTION_CALL(int64_t, "SubucToLong", int64_t(-1), uint8_t(1), uint8_t(2));
	VALIDATE_FUNCTION_CALL(double, "SubusToDouble", -1.0, uint16_t(1), uint16_t(2));
	VALIDATE_FUNCTION_CALL(int32_t, "DivSubucByIntConstant", int32_t(-2), uint8_t(1), uint8_t(5));
	VALIDATE_FUNCTION_CALL(int32_t, "CmpltSubuc", int32_t(1), uint8_t(1), uint8_t(2));
	VALIDATE_FUNCTION_CALL(int64_t, "OffsetBySubuc", int64_t(-1), uint8_t(1), uint8_t(2));

	// Unary operators promote their operand.
	VALIDATE_FUNCTION_CALL(int64_t, "NegucToLong", int64_t(-1), uint8_t(1));
	VALIDATE_FUNCTION_CALL(uint32_t, "NegucToUs", uint32_t(65535), uint8_t(1));
	VALIDATE_FUNCTION_CALL(int64_t, "BitNotucToLong", int64_t(-2), uint8_t(1));
	VALIDATE_FUNCTION_CALL(int64_t, "BitNotusToLong", int64_t(-2), uint16_t(1));
	VALIDATE_FUNCTION_CALL(int64_t, "TernaryNegucToLong", int64_t(-1), true, uint8_t(1), uint8_t(2));
	VALIDATE_FUNCTION_CALL(int64_t, "TernaryNegucToLong", int64_t(2), false, uint8_t(1), uint8_t(2));

	// Constant folding of unary operators on promoted operands.
	VALIDATE_FUNCTION_CALL(int32_t, "NegcConstant", int32_t(-97));
	VALIDATE_FUNCTION_CALL(int32_t, "NegsConstant", int32_t(-5));
	VALIDATE_FUNCTION_CALL(int32_t, "BitNotucConstant", int32_t(-2));
	VALIDATE_FUNCTION_CALL(int32_t, "PluscConstant", int32_t(-7));
	VALIDATE_FUNCTION_CALL(int32_t, "NegiMinConstant", BOND_INT_MIN);
}


#define TEST_ITEMS                              \
  TEST_ITEM(Constants)                          \
  TEST_ITEM(StackOperations)                    \
  TEST_ITEM(MemoryOperations)                   \
  TEST_ITEM(TypeConversions)                    \
  TEST_ITEM(BinaryOperators)                    \
  TEST_ITEM(UnaryOperators)                     \
  TEST_ITEM(ComparisonOperators)                \
  TEST_ITEM(PropertyofOperators)                \
  TEST_ITEM(Branches)                           \
  TEST_ITEM(Pointers)                           \
  TEST_ITEM(CompoundAssignmentOperators)        \
  TEST_ITEM(FunctionCalls)                      \
  TEST_ITEM(Structs)                            \
  TEST_ITEM(Initializers)                       \
  TEST_ITEM(Enums)                              \
  TEST_ITEM(ArithmeticConversions)              \

RUN_TESTS(VM, TEST_ITEMS)
