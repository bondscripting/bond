#include "framework/testvmframework.h"
#include "bond/api/libruntime.h"
#include "bond/systems/defaultallocator.h"
#include "bond/vm/cboloader.h"
#include "bond/vm/codesegment.h"
#include <cctype>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <limits>

const char *const TEMP_FILE_NAME = "vm_LibIO_Temp.bin";
const char *const INPUT_FILE_NAME = "scripts/vm_Collector_Input.txt";
const char *const MISSING_FILE_NAME = "scripts/vm_LibIO_NoSuchFile.txt";


// Removes the temporary file before and after a test, whether or not the test passes.
class TempFileGuard
{
public:
	TempFileGuard() { remove(TEMP_FILE_NAME); }
	~TempFileGuard() { remove(TEMP_FILE_NAME); }
};


// RunVMTest always gives the VM a DefaultAllocator.
int32_t GetNumVMAllocations(Bond::VM &vm)
{
	return int32_t(static_cast<Bond::DefaultAllocator &>(vm.GetAllocator()).GetNumAllocations());
}


void GetNumAllocations(Bond::StackFrame &frame)
{
	frame.SetReturnValue(GetNumVMAllocations(frame.GetVM()));
}


const Bond::NativeFunctionBinding ALLOCATION_COUNT_BINDING_FUNCTIONS[] =
{
	{"GetNumAllocations", GetNumAllocations},
	{nullptr, nullptr}
};

const Bond::NativeBindingCollection ALLOCATION_COUNT_BINDING_COLLECTION = {ALLOCATION_COUNT_BINDING_FUNCTIONS, 1};


// Calls a Bond function that takes a file name and asserts that it returns 0, then that all of the
// memory it allocated was released when it returned.
#define VALIDATE_RELEASED_ON_RETURN(functionName, fileName)                                    \
  {                                                                                            \
    const int32_t baseline = GetNumVMAllocations(vm);                                          \
    VALIDATE_FUNCTION_CALL(int32_t, functionName, int32_t(0), (fileName));                     \
    ASSERT_FORMAT(GetNumVMAllocations(vm) == baseline, "%s did not release its allocations.", functionName); \
  }


template <typename T>
bool IsAligned(T *ptr, size_t alignment)
{
	return (reinterpret_cast<uintptr_t>(ptr) % alignment) == 0;
}


DEFINE_VM_TEST(Math, "scripts/sanalyzer_IncludeLibraries.bond")
{
	using namespace Bond;

	const double PI = 3.14159265358979323846;
	const double TWO_PI = PI * 2.0;
	const double HALF_PI = PI / 2.0;
	const double THIRD_PI = PI / 3.0;
	const double SIXTH_PI = PI / 6.0;

	//const float PI_F = float(3.14159265358979323846);
	const float TWO_PI_F = float(PI * 2.0);
	const float HALF_PI_F = float(PI / 2.0);
	const float THIRD_PI_F = float(PI / 3.0);
	const float SIXTH_PI_F = float(PI / 6.0);

	VALIDATE_FUNCTION_CALL(double, "Bond.SinD", 0.0, 0.0);
	VALIDATE_FUNCTION_CALL(double, "Bond.SinD", 1.0, HALF_PI);
	VALIDATE_FUNCTION_CALL(double, "Bond.SinD", 0.5, SIXTH_PI);
	VALIDATE_FUNCTION_CALL(double, "Bond.SinD", -1.0, -HALF_PI);
	VALIDATE_FUNCTION_CALL(double, "Bond.SinD", -0.5, TWO_PI - SIXTH_PI);

	VALIDATE_FUNCTION_CALL(float, "Bond.SinF", 0.0f, 0.0f);
	VALIDATE_FUNCTION_CALL(float, "Bond.SinF", 1.0f, HALF_PI_F);
	VALIDATE_FUNCTION_CALL(float, "Bond.SinF", 0.5f, SIXTH_PI_F);
	VALIDATE_FUNCTION_CALL(float, "Bond.SinF", -1.0f, -HALF_PI_F);
	VALIDATE_FUNCTION_CALL(float, "Bond.SinF", -0.5f, TWO_PI_F - SIXTH_PI_F);

	VALIDATE_FUNCTION_CALL(double, "Bond.CosD", 1.0, 0.0);
	VALIDATE_FUNCTION_CALL(double, "Bond.CosD", 0.0, HALF_PI);
	VALIDATE_FUNCTION_CALL(double, "Bond.CosD", 0.5, THIRD_PI);
	VALIDATE_FUNCTION_CALL(double, "Bond.CosD", 0.0, -HALF_PI);
	VALIDATE_FUNCTION_CALL(double, "Bond.CosD", 0.5, TWO_PI - THIRD_PI);
	VALIDATE_FUNCTION_CALL(double, "Bond.CosD", -0.5, HALF_PI + SIXTH_PI);

	VALIDATE_FUNCTION_CALL(float, "Bond.CosF", 1.0f, 0.0f);
	VALIDATE_FUNCTION_CALL(float, "Bond.CosF", 0.0f, HALF_PI_F);
	VALIDATE_FUNCTION_CALL(float, "Bond.CosF", 0.5f, THIRD_PI_F);
	VALIDATE_FUNCTION_CALL(float, "Bond.CosF", 0.0f, -HALF_PI_F);
	VALIDATE_FUNCTION_CALL(float, "Bond.CosF", 0.5f, TWO_PI_F - THIRD_PI_F);
	VALIDATE_FUNCTION_CALL(float, "Bond.CosF", -0.5f, HALF_PI_F + SIXTH_PI_F);

	VALIDATE_FUNCTION_CALL(double, "Bond.CoshD", 1.0, 0.0);
	VALIDATE_FUNCTION_CALL(float, "Bond.CoshF", 1.0f, 0.0f);

	VALIDATE_FUNCTION_CALL(double, "Bond.SqrtD", 4.0, 16.0);
	VALIDATE_FUNCTION_CALL(float, "Bond.SqrtF", 4.0f, 16.0f);
	VALIDATE_FUNCTION_CALL(double, "Bond.CbrtD", -3.0, -27.0);
	VALIDATE_FUNCTION_CALL(float, "Bond.CbrtF", -3.0f, -27.0f);
	VALIDATE_FUNCTION_CALL(double, "Bond.HypotD", 5.0, 3.0, 4.0);
	VALIDATE_FUNCTION_CALL(float, "Bond.HypotF", 5.0f, 3.0f, 4.0f);

	VALIDATE_FUNCTION_CALL(double, "Bond.CeilD", 4.0, PI);
	VALIDATE_FUNCTION_CALL(float, "Bond.CeilF", 4.0f, float(PI));
	VALIDATE_FUNCTION_CALL(double, "Bond.FloorD", -2.0, -1.5);
	VALIDATE_FUNCTION_CALL(float, "Bond.FloorF", -2.0f, -1.5f);
	VALIDATE_FUNCTION_CALL(double, "Bond.TruncD", -1.0, -1.5);
	VALIDATE_FUNCTION_CALL(float, "Bond.TruncF", -1.0f, -1.5f);
	VALIDATE_FUNCTION_CALL(double, "Bond.RoundD", 3.0, 2.5);
	VALIDATE_FUNCTION_CALL(float, "Bond.RoundF", -3.0f, -2.5f);
	VALIDATE_FUNCTION_CALL(double, "Bond.RintD", 2.0, 2.5);
	VALIDATE_FUNCTION_CALL(float, "Bond.NearbyintF", 4.0f, 3.5f);

	VALIDATE_FUNCTION_CALL(double, "Bond.AbsD", 2.5, -2.5);
	VALIDATE_FUNCTION_CALL(float, "Bond.AbsF", 2.5f, -2.5f);
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.AbsS", 7, int16_t(-7));
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.AbsI", 7, int32_t(-7));
	VALIDATE_FUNCTION_CALL(int64_t, "Bond.AbsL", 7, int64_t(-7));

	VALIDATE_FUNCTION_CALL(double, "Bond.LdexpD", 12.0, 1.5, int32_t(3));
	VALIDATE_FUNCTION_CALL(float, "Bond.LdexpF", 12.0f, 1.5f, int32_t(3));
	VALIDATE_FUNCTION_CALL(double, "Bond.FmodD", 1.5, 7.5, 2.0);
	VALIDATE_FUNCTION_CALL(float, "Bond.FmodF", 1.5f, 7.5f, 2.0f);
	VALIDATE_FUNCTION_CALL(double, "Bond.CopySignD", -3.0, 3.0, -0.0);
	VALIDATE_FUNCTION_CALL(float, "Bond.CopySignF", 3.0f, -3.0f, 1.0f);

	VALIDATE_FUNCTION_CALL(int32_t, "Bond.IsNaND", int32_t(1), std::nan(""));
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.IsNaND", int32_t(0), 1.0);
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.IsInfF", int32_t(1), std::numeric_limits<float>::infinity());
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.IsFiniteD", int32_t(0), std::numeric_limits<double>::infinity());
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.SignBitD", int32_t(1), -0.0);

	VALIDATE_FUNCTION_CALL(double, "Bond.MinD", 1.0, 1.0, 2.0);
	VALIDATE_FUNCTION_CALL(float, "Bond.MaxF", 2.0f, 1.0f, 2.0f);
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.MinI", -3, -3, 5);
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.MaxI", 5, -3, 5);
	VALIDATE_FUNCTION_CALL(int64_t, "Bond.MinL", -3, int64_t(-3), int64_t(5));
	VALIDATE_FUNCTION_CALL(uint64_t, "Bond.MaxUL", uint64_t(5), uint64_t(3), uint64_t(5));
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.MinUC", 3, uint8_t(3), uint8_t(200));
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.MaxUS", 200, uint16_t(3), uint16_t(200));

	VALIDATE_FUNCTION_CALL(double, "Bond.ClampD", 1.0, 5.0, 0.0, 1.0);
	VALIDATE_FUNCTION_CALL(float, "Bond.ClampF", 0.5f, 0.5f, 0.0f, 1.0f);
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.ClampI", 0, -5, 0, 10);
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.ClampS", 10, int16_t(15), int16_t(0), int16_t(10));
}


DEFINE_VM_TEST(Type, "scripts/sanalyzer_IncludeLibraries.bond")
{
	using namespace Bond;

	VALIDATE_FUNCTION_CALL(int32_t, "Bond.IsAlnum", int32_t(isalnum('w') != 0), int32_t('w'));
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.IsAlnum", int32_t(isalnum('4') != 0), int32_t('4'));
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.IsAlnum", int32_t(isalnum(' ') != 0), int32_t(' '));

	VALIDATE_FUNCTION_CALL(int32_t, "Bond.IsAlpha", int32_t(isalpha('w') != 0), int32_t('w'));
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.IsAlpha", int32_t(isalpha('4') != 0), int32_t('4'));

	VALIDATE_FUNCTION_CALL(int32_t, "Bond.IsCntrl", int32_t(iscntrl('w') != 0), int32_t('w'));
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.IsCntrl", int32_t(iscntrl(0x07) != 0), int32_t(0x07));

	VALIDATE_FUNCTION_CALL(int32_t, "Bond.IsDigit", int32_t(isdigit('w') != 0), int32_t('w'));
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.IsDigit", int32_t(isdigit('4') != 0), int32_t('4'));

	VALIDATE_FUNCTION_CALL(int32_t, "Bond.IsGraph", int32_t(isgraph('w') != 0), int32_t('w'));
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.IsGraph", int32_t(isgraph(' ') != 0), int32_t(' '));

	VALIDATE_FUNCTION_CALL(int32_t, "Bond.IsLower", int32_t(islower('w') != 0), int32_t('w'));
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.IsLower", int32_t(islower('W') != 0), int32_t('W'));
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.IsLower", int32_t(islower('4') != 0), int32_t('4'));

	VALIDATE_FUNCTION_CALL(int32_t, "Bond.IsPrint", int32_t(isprint('w') != 0), int32_t('w'));
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.IsPrint", int32_t(isprint(0x07) != 0), int32_t(0x07));

	VALIDATE_FUNCTION_CALL(int32_t, "Bond.IsPunct", int32_t(ispunct('w') != 0), int32_t('w'));
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.IsPunct", int32_t(ispunct('.') != 0), int32_t('.'));

	VALIDATE_FUNCTION_CALL(int32_t, "Bond.IsSpace", int32_t(isspace('w') != 0), int32_t('w'));
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.IsSpace", int32_t(isspace(' ') != 0), int32_t(' '));

	VALIDATE_FUNCTION_CALL(int32_t, "Bond.IsUpper", int32_t(isupper('w') != 0), int32_t('w'));
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.IsUpper", int32_t(isupper('W') != 0), int32_t('W'));
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.IsUpper", int32_t(isupper('4') != 0), int32_t('4'));

	VALIDATE_FUNCTION_CALL(int32_t, "Bond.IsXDigit", int32_t(isxdigit('w') != 0), int32_t('w'));
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.IsXDigit", int32_t(isxdigit('a') != 0), int32_t('a'));

	VALIDATE_FUNCTION_CALL(int32_t, "Bond.ToLower", int32_t(tolower('w')), int32_t('w'));
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.ToLower", int32_t(tolower('W')), int32_t('W'));
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.ToLower", int32_t(tolower('4')), int32_t('4'));

	VALIDATE_FUNCTION_CALL(int32_t, "Bond.ToUpper", int32_t(toupper('w')), int32_t('w'));
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.ToUpper", int32_t(toupper('W')), int32_t('W'));
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.ToUpper", int32_t(toupper('4')), int32_t('4'));
}


DEFINE_VM_TEST(Collector, "scripts/vm_Collector.bond")
{
	using namespace Bond;

	VALIDATE_FUNCTION_CALL(int32_t, "RunCollectorTest", int32_t(1));
}


DEFINE_VM_TEST(String, "scripts/sanalyzer_IncludeLibraries.bond")
{
	using namespace Bond;

	const char *const NULL_STRING = nullptr;
	const int32_t SUCCESS = 1;
	const int32_t FAILURE = 0;

	VALIDATE_FUNCTION_CALL(uint32_t, "Bond.Strlen", uint32_t(0), "");
	VALIDATE_FUNCTION_CALL(uint32_t, "Bond.Strlen", uint32_t(1), "a");
	VALIDATE_FUNCTION_CALL(uint32_t, "Bond.Strlen", uint32_t(11), "Hello World");

	// The magnitude of the result of Strcmp is implementation defined.
	const auto strcmpResult = [&vm](const char *str1, const char *str2)
	{
		int32_t result = 0;
		vm.CallFunction("Bond.Strcmp", &result, str1, str2);
		return result;
	};
	ASSERT_MESSAGE(strcmpResult("abc", "abd") < 0, "Expected \"abc\" < \"abd\".");
	ASSERT_MESSAGE(strcmpResult("abd", "abc") > 0, "Expected \"abd\" > \"abc\".");
	ASSERT_MESSAGE(strcmpResult("abc", "abc") == 0, "Expected \"abc\" == \"abc\".");
	ASSERT_MESSAGE(strcmpResult("", "") == 0, "Expected \"\" == \"\".");
	ASSERT_MESSAGE(strcmpResult("ab", "abc") < 0, "Expected \"ab\" < \"abc\".");
	ASSERT_MESSAGE(strcmpResult("abc", "ab") > 0, "Expected \"abc\" > \"ab\".");

	int32_t intResult = -1;
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.TryParseInt", SUCCESS, "42", &intResult);
	ASSERT_FORMAT(intResult == 42, "Expected 42, but was %" BOND_PRId32 ".", intResult);
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.TryParseInt", SUCCESS, "-42", &intResult);
	ASSERT_FORMAT(intResult == -42, "Expected -42, but was %" BOND_PRId32 ".", intResult);
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.TryParseInt", SUCCESS, "2147483647", &intResult);
	ASSERT_FORMAT(intResult == BOND_INT_MAX, "Expected INT_MAX, but was %" BOND_PRId32 ".", intResult);
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.TryParseInt", SUCCESS, "-2147483648", &intResult);
	ASSERT_FORMAT(intResult == BOND_INT_MIN, "Expected INT_MIN, but was %" BOND_PRId32 ".", intResult);

	intResult = 99;
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.TryParseInt", FAILURE, "2147483648", &intResult);
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.TryParseInt", FAILURE, "-2147483649", &intResult);
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.TryParseInt", FAILURE, "", &intResult);
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.TryParseInt", FAILURE, "abc", &intResult);
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.TryParseInt", FAILURE, NULL_STRING, &intResult);
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.TryParseInt", FAILURE, "42", static_cast<int32_t *>(nullptr));
	ASSERT_FORMAT(intResult == 99, "Expected result to be unchanged, but was %" BOND_PRId32 ".", intResult);

	int64_t longResult = -1;
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.TryParseLong", SUCCESS, "-42", &longResult);
	ASSERT_FORMAT(longResult == -42, "Expected -42, but was %" BOND_PRId64 ".", longResult);
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.TryParseLong", SUCCESS, "9223372036854775807", &longResult);
	ASSERT_FORMAT(longResult == BOND_LONG_MAX, "Expected LONG_MAX, but was %" BOND_PRId64 ".", longResult);
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.TryParseLong", SUCCESS, "-9223372036854775808", &longResult);
	ASSERT_FORMAT(longResult == BOND_LONG_MIN, "Expected LONG_MIN, but was %" BOND_PRId64 ".", longResult);

	longResult = 99;
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.TryParseLong", FAILURE, "9223372036854775808", &longResult);
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.TryParseLong", FAILURE, "-9223372036854775809", &longResult);
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.TryParseLong", FAILURE, "", &longResult);
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.TryParseLong", FAILURE, "abc", &longResult);
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.TryParseLong", FAILURE, NULL_STRING, &longResult);
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.TryParseLong", FAILURE, "42", static_cast<int64_t *>(nullptr));
	ASSERT_FORMAT(longResult == 99, "Expected result to be unchanged, but was %" BOND_PRId64 ".", longResult);

	float floatResult = -1.0f;
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.TryParseFloat", SUCCESS, "-1.5", &floatResult);
	ASSERT_FORMAT(floatResult == -1.5f, "Expected -1.5, but was %f.", double(floatResult));

	floatResult = 99.0f;
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.TryParseFloat", FAILURE, "", &floatResult);
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.TryParseFloat", FAILURE, "abc", &floatResult);
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.TryParseFloat", FAILURE, NULL_STRING, &floatResult);
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.TryParseFloat", FAILURE, "1.5", static_cast<float *>(nullptr));
	ASSERT_FORMAT(floatResult == 99.0f, "Expected result to be unchanged, but was %f.", double(floatResult));

	double doubleResult = -1.0;
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.TryParseDouble", SUCCESS, "2.25", &doubleResult);
	ASSERT_FORMAT(doubleResult == 2.25, "Expected 2.25, but was %f.", doubleResult);

	doubleResult = 99.0;
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.TryParseDouble", FAILURE, "", &doubleResult);
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.TryParseDouble", FAILURE, "abc", &doubleResult);
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.TryParseDouble", FAILURE, NULL_STRING, &doubleResult);
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.TryParseDouble", FAILURE, "1.5", static_cast<double *>(nullptr));
	ASSERT_FORMAT(doubleResult == 99.0, "Expected result to be unchanged, but was %f.", doubleResult);
}


DEFINE_VM_TEST_WITH_BINDING(MemoryRoundTrip, "scripts/vm_LibMemory.bond", &ALLOCATION_COUNT_BINDING_COLLECTION)
{
	using namespace Bond;

	VALIDATE_FUNCTION_CALL(int32_t, "AllocateWriteReadFree", int32_t(1));
	VALIDATE_FUNCTION_CALL(int32_t, "CopyMemory", int32_t(1));
}


DEFINE_VM_TEST_WITH_BINDING(MemoryAligned, "scripts/vm_LibMemory.bond", &ALLOCATION_COUNT_BINDING_COLLECTION)
{
	using namespace Bond;

	const int32_t baseline = GetNumVMAllocations(vm);

	const uint32_t ALIGNMENTS[] = {uint32_t(4), uint32_t(16), uint32_t(64), uint32_t(256), uint32_t(4096)};
	for (const uint32_t alignment: ALIGNMENTS)
	{
		void *ptr = nullptr;
		vm.CallFunction("Bond.AllocateAligned", &ptr, uint64_t(32), alignment);
		ASSERT_FORMAT(IsAligned(ptr, alignment), "Pointer %p is not aligned to %" BOND_PRIu32 ".", ptr, alignment);
		memset(ptr, 0xcd, 32);
		vm.CallVoidFunction("Bond.FreeAligned", ptr);
		ASSERT_MESSAGE(GetNumVMAllocations(vm) == baseline, "FreeAligned did not release the allocation.");
	}

	// Called from C++, these use the VM's outermost collector, which is released with the VM.
	Collector *collector = nullptr;
	vm.CallFunction("Bond.GetCollector", &collector);
	ASSERT_MESSAGE(collector != nullptr, "Expected a collector.");

	for (const uint32_t alignment: ALIGNMENTS)
	{
		void *ptr = nullptr;
		vm.CallFunction("Bond.AllocateAlignedCollected", &ptr, uint64_t(32), alignment);
		ASSERT_FORMAT(IsAligned(ptr, alignment), "Pointer %p is not aligned to %" BOND_PRIu32 ".", ptr, alignment);
		memset(ptr, 0xcd, 32);

		ptr = nullptr;
		vm.CallFunction("Bond.AllocateAlignedWithCollector", &ptr, collector, uint64_t(32), alignment);
		ASSERT_FORMAT(IsAligned(ptr, alignment), "Pointer %p is not aligned to %" BOND_PRIu32 ".", ptr, alignment);
		memset(ptr, 0xcd, 32);
	}
}


DEFINE_VM_TEST_WITH_BINDING(MemoryCollector, "scripts/vm_LibMemory.bond", &ALLOCATION_COUNT_BINDING_COLLECTION)
{
	using namespace Bond;

	const char *const FUNCTIONS[] =
	{
		"AllocateCollectedHeldByFrame",
		"CollectorOutlivesCallee",
		"NullCollectorUsesCaller",
		"AlignedCollectedHeldByFrame"
	};

	for (const char *function: FUNCTIONS)
	{
		const int32_t baseline = GetNumVMAllocations(vm);
		VALIDATE_FUNCTION_CALL(int32_t, function, int32_t(1));
		ASSERT_FORMAT(GetNumVMAllocations(vm) == baseline, "%s did not release its allocations.", function);
	}
}


DEFINE_VM_TEST_WITH_BINDING(IOPrint, "scripts/vm_LibIO.bond", &ALLOCATION_COUNT_BINDING_COLLECTION)
{
	using namespace Bond;

	vm.CallVoidFunction("PrintText");
	ASSERT_CAPTURED_OUTPUT(vm.GetStdOut(), "abc def");

	vm.CallVoidFunction("PrintBools");
	ASSERT_CAPTURED_OUTPUT(vm.GetStdOut(), "10true false");

	vm.CallVoidFunction("PrintIntegers");
	ASSERT_CAPTURED_OUTPUT(vm.GetStdOut(), "-2147483648 4294967295 -9223372036854775808 18446744073709551615");

	vm.CallVoidFunction("PrintBases");
	ASSERT_CAPTURED_OUTPUT(vm.GetStdOut(), "ff 0xff 17 017 ff");

	vm.CallVoidFunction("PrintWidths");
	ASSERT_CAPTURED_OUTPUT(vm.GetStdOut(), "00042|42   |   42|   ab7");

	vm.CallVoidFunction("PrintFloats");
	ASSERT_CAPTURED_OUTPUT(vm.GetStdOut(), "1.5 0.25 1.500 1.50e+03 2.00");

	vm.CallVoidFunction("PrintBytes");
	ASSERT_CAPTURED_OUTPUT(vm.GetStdOut(), "wxyz");

	VALIDATE_FUNCTION_CALL(int32_t, "CheckFormatState", int32_t(0));
	ASSERT_CAPTURED_OUTPUT(vm.GetStdOut(), ".        ");

	vm.CallVoidFunction("PrintError");
	ASSERT_CAPTURED_OUTPUT(vm.GetStdErr(), "err");
	ASSERT_CAPTURED_OUTPUT(vm.GetStdOut(), "");
}


DEFINE_VM_TEST_WITH_BINDING(IOMissingFile, "scripts/vm_LibIO.bond", &ALLOCATION_COUNT_BINDING_COLLECTION)
{
	using namespace Bond;

	VALIDATE_FUNCTION_CALL(int32_t, "MissingInputStream", int32_t(1), MISSING_FILE_NAME);
	VALIDATE_FUNCTION_CALL(int32_t, "MissingInputStreamCollected", int32_t(1), MISSING_FILE_NAME);
	VALIDATE_FUNCTION_CALL(int32_t, "MissingInputStreamWithCollector", int32_t(1), MISSING_FILE_NAME);
}


DEFINE_VM_TEST_WITH_BINDING(IOFile, "scripts/vm_LibIO.bond", &ALLOCATION_COUNT_BINDING_COLLECTION)
{
	using namespace Bond;

	TempFileGuard tempFileGuard;

	VALIDATE_FUNCTION_CALL(int32_t, "WriteFile", int32_t(0), TEMP_FILE_NAME);
	VALIDATE_FUNCTION_CALL(int32_t, "ReadFile", int32_t(0), TEMP_FILE_NAME);
	VALIDATE_FUNCTION_CALL(int32_t, "AppendToFile", int32_t(0), TEMP_FILE_NAME);
	VALIDATE_FUNCTION_CALL(int32_t, "TruncateFile", int32_t(0), TEMP_FILE_NAME);
}


DEFINE_VM_TEST_WITH_BINDING(IOCollectedStreams, "scripts/vm_LibIO.bond", &ALLOCATION_COUNT_BINDING_COLLECTION)
{
	using namespace Bond;

	TempFileGuard tempFileGuard;

	VALIDATE_RELEASED_ON_RETURN("InputCollected", INPUT_FILE_NAME);
	VALIDATE_RELEASED_ON_RETURN("InputWithCollector", INPUT_FILE_NAME);
	VALIDATE_RELEASED_ON_RETURN("InputWithNullCollector", INPUT_FILE_NAME);

	// Releasing a collected output stream must also close it, flushing what was written.
	VALIDATE_RELEASED_ON_RETURN("OutputCollected", TEMP_FILE_NAME);
	VALIDATE_FUNCTION_CALL(int32_t, "ReadFirstByte", int32_t('c'), TEMP_FILE_NAME);
	VALIDATE_RELEASED_ON_RETURN("OutputWithCollector", TEMP_FILE_NAME);
	VALIDATE_FUNCTION_CALL(int32_t, "ReadFirstByte", int32_t('w'), TEMP_FILE_NAME);
	VALIDATE_RELEASED_ON_RETURN("OutputWithNullCollector", TEMP_FILE_NAME);
	VALIDATE_FUNCTION_CALL(int32_t, "ReadFirstByte", int32_t('n'), TEMP_FILE_NAME);
}


#if BOND_RUNTIME_CHECKS_ENABLED

const char *const NULL_STRING_ERROR = "Attempt to pass a null string.";
const char *const NULL_MEMORY_ERROR = "Attempt to copy memory using a null pointer.";
const char *const NULL_STREAM_ERROR = "Attempt to use a null or closed stream.";
const char *const NULL_FACTORY_ERROR = "Attempt to open a stream without a stream factory.";
const char *const NULL_FILE_NAME_ERROR = "Attempt to open a stream with a null file name.";
const char *const SCAN_BUFFER_ERROR = "Attempt to scan into an invalid buffer.";
const char *const NULL_BYTES_ERROR = "Attempt to transfer bytes using a null pointer.";
const char *const FREE_STANDARD_STREAM_ERROR = "Attempt to free a standard stream.";

DEFINE_VM_TEST_WITH_BINDING(RuntimeValidation, "scripts/vm_LibIO.bond", &ALLOCATION_COUNT_BINDING_COLLECTION)
{
	using namespace Bond;

	TempFileGuard tempFileGuard;

	const char *const NULL_STRING = nullptr;
	char *const NULL_BUFFER = nullptr;
	void *const NULL_POINTER = nullptr;
	uint8_t *const NULL_BYTES = nullptr;
	Collector *const NULL_COLLECTOR = nullptr;
	InputStreamAdaptor *const NULL_INPUT = nullptr;
	OutputStreamAdaptor *const NULL_OUTPUT = nullptr;

	uint32_t uintResult = 0;
	int32_t intResult = 0;
	InputStreamAdaptor *inputResult = nullptr;
	OutputStreamAdaptor *outputResult = nullptr;
	OutputStreamAdaptor *stdOut = &vm.GetStdOut();

	// string
	ASSERT_BOND_EXCEPTION(NULL_STRING_ERROR, vm.CallFunction("Bond.Strlen", &uintResult, NULL_STRING));
	ASSERT_BOND_EXCEPTION(NULL_STRING_ERROR, vm.CallFunction("Bond.Strcmp", &intResult, NULL_STRING, "a"));
	ASSERT_BOND_EXCEPTION(NULL_STRING_ERROR, vm.CallFunction("Bond.Strcmp", &intResult, "a", NULL_STRING));
	VALIDATE_FUNCTION_CALL(uint32_t, "Bond.Strlen", uint32_t(3), "abc");

	// memory
	char dest = 'd';
	char src = 's';
	void *destPointer = &dest;
	void *srcPointer = &src;
	ASSERT_BOND_EXCEPTION(NULL_MEMORY_ERROR, vm.CallVoidFunction("Bond.Memcpy", NULL_POINTER, srcPointer, uint64_t(1)));
	ASSERT_BOND_EXCEPTION(NULL_MEMORY_ERROR, vm.CallVoidFunction("Bond.Memcpy", destPointer, NULL_POINTER, uint64_t(1)));
	vm.CallVoidFunction("Bond.Memcpy", NULL_POINTER, NULL_POINTER, uint64_t(0));
	ASSERT_MESSAGE(dest == 'd', "Memcpy of zero bytes changed the destination.");
	vm.CallVoidFunction("Bond.Memcpy", destPointer, srcPointer, uint64_t(1));
	ASSERT_MESSAGE(dest == 's', "Memcpy did not copy.");

	// Null streams.
	ASSERT_BOND_EXCEPTION(NULL_STREAM_ERROR, vm.CallVoidFunction("Bond.InputStream.Close", NULL_INPUT));
	ASSERT_BOND_EXCEPTION(NULL_STREAM_ERROR, vm.CallFunction("Bond.InputStream.Get", &intResult, NULL_INPUT));
	ASSERT_BOND_EXCEPTION(NULL_STREAM_ERROR, vm.CallFunction("Bond.InputStream.ScanI", &intResult, NULL_INPUT));
	ASSERT_BOND_EXCEPTION(NULL_STREAM_ERROR, vm.CallFunction("Bond.InputStream.GetFlags", &intResult, NULL_INPUT));
	ASSERT_BOND_EXCEPTION(NULL_STREAM_ERROR, vm.CallFunction("Bond.InputStream.GetPosition", &intResult, NULL_INPUT));
	ASSERT_BOND_EXCEPTION(NULL_STREAM_ERROR, vm.CallVoidFunction("Bond.OutputStream.Close", NULL_OUTPUT));
	ASSERT_BOND_EXCEPTION(NULL_STREAM_ERROR, vm.CallVoidFunction("Bond.OutputStream.Flush", NULL_OUTPUT));
	ASSERT_BOND_EXCEPTION(NULL_STREAM_ERROR, vm.CallFunction("Bond.OutputStream.PrintI", &outputResult, NULL_OUTPUT, int32_t(1)));
	ASSERT_BOND_EXCEPTION(NULL_STREAM_ERROR, vm.CallFunction("Bond.OutputStream.GetFlags", &intResult, NULL_OUTPUT));
	ASSERT_BOND_EXCEPTION(NULL_STREAM_ERROR, vm.CallFunction("Bond.OutputStream.GetPosition", &intResult, NULL_OUTPUT));

	// RunVMTest gives the VM stdout and stderr, but not stdin.
	InputStreamAdaptor *stdIn = nullptr;
	vm.CallFunction("Bond.StdIn", &stdIn);
	ASSERT_BOND_EXCEPTION(NULL_STREAM_ERROR, vm.CallFunction("Bond.InputStream.Get", &intResult, stdIn));

	// Closed streams.
	vm.CallVoidFunction("CloseInputTwice", INPUT_FILE_NAME);
	vm.CallVoidFunction("CloseOutputTwice", TEMP_FILE_NAME);
	ASSERT_BOND_EXCEPTION(NULL_STREAM_ERROR, vm.CallFunction("UseClosedInput", &intResult, INPUT_FILE_NAME));
	ASSERT_BOND_EXCEPTION(NULL_STREAM_ERROR, vm.CallVoidFunction("UseClosedOutput", TEMP_FILE_NAME));

	// Null file names.
	ASSERT_BOND_EXCEPTION(NULL_FILE_NAME_ERROR, vm.CallFunction("Bond.CreateInputStream", &inputResult, NULL_STRING));
	ASSERT_BOND_EXCEPTION(NULL_FILE_NAME_ERROR, vm.CallFunction("Bond.CreateInputStreamCollected", &inputResult, NULL_STRING));
	ASSERT_BOND_EXCEPTION(NULL_FILE_NAME_ERROR, vm.CallFunction("Bond.CreateInputStreamWithCollector", &inputResult, NULL_COLLECTOR, NULL_STRING));
	ASSERT_BOND_EXCEPTION(NULL_FILE_NAME_ERROR, vm.CallFunction("Bond.CreateOutputStream", &outputResult, NULL_STRING, false));
	ASSERT_BOND_EXCEPTION(NULL_FILE_NAME_ERROR, vm.CallFunction("Bond.CreateOutputStreamCollected", &outputResult, NULL_STRING, false));
	ASSERT_BOND_EXCEPTION(NULL_FILE_NAME_ERROR, vm.CallFunction("Bond.CreateOutputStreamWithCollector", &outputResult, NULL_COLLECTOR, NULL_STRING, false));

	// Null buffers.
	ASSERT_BOND_EXCEPTION(NULL_STRING_ERROR, vm.CallFunction("Bond.OutputStream.PrintStr", &outputResult, stdOut, NULL_STRING));
	ASSERT_BOND_EXCEPTION(NULL_BYTES_ERROR, vm.CallFunction("Bond.OutputStream.Write", &outputResult, stdOut, NULL_BYTES, uint32_t(1)));
	vm.CallFunction("Bond.OutputStream.Write", &outputResult, stdOut, NULL_BYTES, uint32_t(0));
	ASSERT_CAPTURED_OUTPUT(vm.GetStdOut(), "");

	InputStreamAdaptor *input = nullptr;
	vm.CallFunction("Bond.CreateInputStream", &input, INPUT_FILE_NAME);
	ASSERT_MESSAGE(input != nullptr, "Failed to open the input file.");

	char buffer[4];
	char *bufferPointer = buffer;
	ASSERT_BOND_EXCEPTION(SCAN_BUFFER_ERROR, vm.CallVoidFunction("Bond.InputStream.ScanStr", input, NULL_BUFFER, uint32_t(4)));
	ASSERT_BOND_EXCEPTION(SCAN_BUFFER_ERROR, vm.CallVoidFunction("Bond.InputStream.ScanStr", input, bufferPointer, uint32_t(0)));
	ASSERT_BOND_EXCEPTION(NULL_BYTES_ERROR, vm.CallFunction("Bond.InputStream.Read", &uintResult, input, NULL_BYTES, uint32_t(1)));
	VALIDATE_FUNCTION_CALL(uint32_t, "Bond.InputStream.Read", uint32_t(0), input, NULL_BYTES, uint32_t(0));

	vm.CallVoidFunction("Bond.FreeInputStream", input);

	// Standard streams.
	ASSERT_BOND_EXCEPTION(FREE_STANDARD_STREAM_ERROR, vm.CallVoidFunction("Bond.FreeInputStream", &vm.GetStdIn()));
	ASSERT_BOND_EXCEPTION(FREE_STANDARD_STREAM_ERROR, vm.CallVoidFunction("Bond.FreeOutputStream", &vm.GetStdOut()));
	ASSERT_BOND_EXCEPTION(FREE_STANDARD_STREAM_ERROR, vm.CallVoidFunction("Bond.FreeOutputStream", &vm.GetStdErr()));

	// Null is accepted, and the VM's stdout still works after all of the above.
	vm.CallVoidFunction("Bond.FreeInputStream", NULL_INPUT);
	vm.CallVoidFunction("Bond.FreeOutputStream", NULL_OUTPUT);
	vm.CallVoidFunction("PrintText");
	ASSERT_CAPTURED_OUTPUT(vm.GetStdOut(), "abc def");
}


DEFINE_TEST(VMWithoutStreams)
{
	using namespace Bond;
	using namespace TestFramework;

	uint8_t cboBuffer[MAX_CBO_SIZE];
	const size_t cboSize = CompileScriptToCbo(
		__FILE__, __LINE__, "scripts/sanalyzer_IncludeLibraries.bond", cboBuffer, MAX_CBO_SIZE);

	DefaultAllocator cboLoaderAllocator;
	DefaultAllocator vmAllocator;
	AllocationLeakGuard<DefaultAllocator> cboLoaderLeakGuard(cboLoaderAllocator, "CBO loader", __FILE__, __LINE__);
	AllocationLeakGuard<DefaultAllocator> vmLeakGuard(vmAllocator, "VM", __FILE__, __LINE__);

	try
	{
		CboLoader cboLoader(cboLoaderAllocator);
		LoadAllLibs(cboLoader);
		cboLoader.AddCboFile(cboBuffer, cboSize);
		CodeSegmentHandle codeSegmentHandle = cboLoader.Load();
		VM vm(vmAllocator, *codeSegmentHandle.get(), 96 * 1024);
		codeSegmentHandle.get()->CallStaticInitializers(vm);

		int32_t intResult = 0;
		InputStreamAdaptor *input = nullptr;
		OutputStreamAdaptor *output = nullptr;
		Collector *const NULL_COLLECTOR = nullptr;

		vm.CallFunction("Bond.StdIn", &input);
		ASSERT_BOND_EXCEPTION(NULL_STREAM_ERROR, vm.CallFunction("Bond.InputStream.Get", &intResult, input));

		vm.CallFunction("Bond.StdOut", &output);
		ASSERT_BOND_EXCEPTION(NULL_STREAM_ERROR, vm.CallFunction("Bond.OutputStream.PrintI", &output, output, int32_t(1)));

		vm.CallFunction("Bond.StdErr", &output);
		ASSERT_BOND_EXCEPTION(NULL_STREAM_ERROR, vm.CallFunction("Bond.OutputStream.PrintI", &output, output, int32_t(1)));

		ASSERT_BOND_EXCEPTION(NULL_FACTORY_ERROR, vm.CallFunction("Bond.CreateInputStream", &input, INPUT_FILE_NAME));
		ASSERT_BOND_EXCEPTION(NULL_FACTORY_ERROR, vm.CallFunction("Bond.CreateInputStreamCollected", &input, INPUT_FILE_NAME));
		ASSERT_BOND_EXCEPTION(NULL_FACTORY_ERROR, vm.CallFunction("Bond.CreateInputStreamWithCollector", &input, NULL_COLLECTOR, INPUT_FILE_NAME));
		ASSERT_BOND_EXCEPTION(NULL_FACTORY_ERROR, vm.CallFunction("Bond.CreateOutputStream", &output, TEMP_FILE_NAME, false));
		ASSERT_BOND_EXCEPTION(NULL_FACTORY_ERROR, vm.CallFunction("Bond.CreateOutputStreamCollected", &output, TEMP_FILE_NAME, false));
		ASSERT_BOND_EXCEPTION(NULL_FACTORY_ERROR, vm.CallFunction("Bond.CreateOutputStreamWithCollector", &output, NULL_COLLECTOR, TEMP_FILE_NAME, false));
	}
	catch (const Exception &e)
	{
		throw AssertionFailure(__FILE__, __LINE__, "%s", e.GetMessage());
	}
}

#define RUNTIME_CHECK_TEST_ITEMS                \
  TEST_ITEM(RuntimeValidation)                  \
  TEST_ITEM(VMWithoutStreams)                   \

#else

#define RUNTIME_CHECK_TEST_ITEMS

#endif


#define TEST_ITEMS                              \
  TEST_ITEM(Math)                               \
  TEST_ITEM(Type)                               \
  TEST_ITEM(Collector)                          \
  TEST_ITEM(String)                             \
  TEST_ITEM(MemoryRoundTrip)                    \
  TEST_ITEM(MemoryAligned)                      \
  TEST_ITEM(MemoryCollector)                    \
  TEST_ITEM(IOPrint)                            \
  TEST_ITEM(IOMissingFile)                      \
  TEST_ITEM(IOFile)                             \
  TEST_ITEM(IOCollectedStreams)                 \
  RUNTIME_CHECK_TEST_ITEMS                      \

RUN_TESTS(Libraries, TEST_ITEMS)
