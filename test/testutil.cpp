#include "framework/testframework.h"
#include "bond/io/path.h"
#include "bond/systems/defaultallocator.h"
#include "bond/private/types.h"
#include <cstring>

DEFINE_TEST(NormalizePath)
{
	struct Case
	{
		const char *directory;
		const char *path;
		const char *expected;
	};

	const Case CASES[] =
	{
		{"", "a/b", "a/b"},
		{"", "a/./b", "a/b"},
		{"", "./a", "a"},
		{"", "a//b", "a/b"},
		{"", "a/b/", "a/b"},
		{"", "a\\b", "a/b"},
		{"", "a/b/../c", "a/c"},
		{"", "../a", "../a"},
		{"", "../../a", "../../a"},
		{"", "a/../../b", "../b"},
		{"", "/../a", "/a"},
		{"", "/a/..", "/"},
		{"", "C:\\x\\..\\y", "C:/y"},
		{"", "C:x", "C:x"},
		{"", "C:..\\x", "C:../x"},
		{"", "//server/share/./f", "//server/share/f"},
		{"", "\\\\server\\share\\f", "//server/share/f"},
		{"", "", "."},
		{"", ".", "."},
		{"", "a/..", "."},
		{"scripts/", "x.bond", "scripts/x.bond"},
		{"scripts", "x.bond", "scripts/x.bond"},
		{"scripts/", "../lib/x.bond", "lib/x.bond"},
		{"scripts/", "../../x.bond", "../x.bond"},
		{"/root/", "../../x.bond", "/x.bond"},
		{"C:\\dir\\", "sub/x.bond", "C:/dir/sub/x.bond"},
	};
	const size_t NUM_CASES = sizeof(CASES) / sizeof(*CASES);

	Bond::DefaultAllocator allocator;
	TestFramework::AllocationLeakGuard<Bond::DefaultAllocator> leakGuard(allocator, "NormalizePath", __FILE__, __LINE__);
	{
		Bond::Vector<char> result((Bond::Vector<char>::allocator_type(&allocator)));
		for (size_t i = 0; i < NUM_CASES; ++i)
		{
			const Case &c = CASES[i];
			Bond::NormalizePath(c.directory, c.path, result);
			const size_t expectedLength = strlen(c.expected);
			ASSERT_FORMAT((result.size() == (expectedLength + 1)) && (result.back() == '\0'),
				"Expected \"%s\" + \"%s\" to give a null terminated result of length %d.",
				c.directory, c.path, int(expectedLength));
			ASSERT_FORMAT(strcmp(result.data(), c.expected) == 0,
				"Expected \"%s\" + \"%s\" to normalize to \"%s\", but was \"%s\".",
				c.directory, c.path, c.expected, result.data());
		}
	}
}


DEFINE_TEST(IsAbsolutePath)
{
	const char *const ABSOLUTE_PATHS[] = {"/a", "\\a", "//server/share", "C:a", "C:/a", "c:\\a"};
	const char *const RELATIVE_PATHS[] = {"", "a", "./a", "../a", "a/b", "1:a", ":a"};

	for (const char *path: ABSOLUTE_PATHS)
	{
		ASSERT_FORMAT(Bond::IsAbsolutePath(path), "Expected \"%s\" to be absolute.", path);
	}

	for (const char *path: RELATIVE_PATHS)
	{
		ASSERT_FORMAT(!Bond::IsAbsolutePath(path), "Expected \"%s\" to be relative.", path);
	}
}


DEFINE_TEST(GetDirectory)
{
	struct Case
	{
		const char *path;
		const char *expected;
	};

	const Case CASES[] =
	{
		{"a/b/c.bond", "a/b/"},
		{"c.bond", ""},
		{"", ""},
		{"/c.bond", "/"},
		{"a\\c.bond", "a\\"},
		{"a/b\\c.bond", "a/b\\"},
		{"a/b/", "a/b/"},
	};

	for (const Case &c: CASES)
	{
		const Bond::StringView actual = Bond::GetDirectory(c.path);
		ASSERT_FORMAT(actual == Bond::StringView(c.expected),
			"Expected the directory of \"%s\" to be \"%s\", but was \"%.*s\".",
			c.path, c.expected, int(actual.size()), actual.data());
	}
}


#define ASSERT_IN_RANGE(RangeType, value, expected)                                  \
  ASSERT_MESSAGE(Bond::IsInRange<RangeType>(value) == (expected),                    \
    "Expected IsInRange<" #RangeType ">(" #value ") to be " #expected ".")

DEFINE_TEST(IsInRange)
{
	const float FLOAT_NAN = std::numeric_limits<float>::quiet_NaN();
	const float FLOAT_INF = std::numeric_limits<float>::infinity();
	const double DOUBLE_NAN = std::numeric_limits<double>::quiet_NaN();
	const double DOUBLE_INF = std::numeric_limits<double>::infinity();

	// Integral to integral.
	ASSERT_IN_RANGE(int8_t, int32_t(127), true);
	ASSERT_IN_RANGE(int8_t, int32_t(128), false);
	ASSERT_IN_RANGE(int8_t, int32_t(-128), true);
	ASSERT_IN_RANGE(int8_t, int32_t(-129), false);
	ASSERT_IN_RANGE(int8_t, uint32_t(127), true);
	ASSERT_IN_RANGE(int8_t, uint32_t(128), false);
	ASSERT_IN_RANGE(int8_t, int64_t(-129), false);
	ASSERT_IN_RANGE(int8_t, Bond::BOND_ULONG_MAX, false);
	ASSERT_IN_RANGE(uint8_t, int32_t(-1), false);
	ASSERT_IN_RANGE(uint8_t, int32_t(255), true);
	ASSERT_IN_RANGE(uint8_t, int32_t(256), false);
	ASSERT_IN_RANGE(uint8_t, uint32_t(255), true);
	ASSERT_IN_RANGE(uint8_t, uint32_t(256), false);
	ASSERT_IN_RANGE(uint8_t, Bond::BOND_LONG_MIN, false);
	ASSERT_IN_RANGE(uint16_t, size_t(65535), true);
	ASSERT_IN_RANGE(uint16_t, size_t(65536), false);
	ASSERT_IN_RANGE(int32_t, uint32_t(0x7FFFFFFF), true);
	ASSERT_IN_RANGE(int32_t, uint32_t(0x80000000), false);
	ASSERT_IN_RANGE(int32_t, int64_t(Bond::BOND_INT_MIN), true);
	ASSERT_IN_RANGE(int32_t, int64_t(Bond::BOND_INT_MIN) - 1, false);
	ASSERT_IN_RANGE(uint32_t, int32_t(-1), false);
	ASSERT_IN_RANGE(uint32_t, Bond::BOND_INT_MAX, true);
	ASSERT_IN_RANGE(uint32_t, int64_t(0xFFFFFFFF), true);
	ASSERT_IN_RANGE(uint32_t, int64_t(0x100000000), false);
	ASSERT_IN_RANGE(int64_t, uint64_t(Bond::BOND_LONG_MAX), true);
	ASSERT_IN_RANGE(int64_t, uint64_t(Bond::BOND_LONG_MAX) + 1, false);
	ASSERT_IN_RANGE(int64_t, Bond::BOND_LONG_MIN, true);
	ASSERT_IN_RANGE(uint64_t, int64_t(-1), false);
	ASSERT_IN_RANGE(uint64_t, Bond::BOND_LONG_MAX, true);
	ASSERT_IN_RANGE(uint64_t, int32_t(-1), false);
	ASSERT_IN_RANGE(uint64_t, Bond::BOND_ULONG_MAX, true);

	// Floating point to integral.
	ASSERT_IN_RANGE(int8_t, 127.0f, true);
	ASSERT_IN_RANGE(int8_t, 128.0f, false);
	ASSERT_IN_RANGE(int8_t, -128.0f, true);
	ASSERT_IN_RANGE(int8_t, -129.0f, false);
	ASSERT_IN_RANGE(int8_t, 0.5f, false);
	ASSERT_IN_RANGE(int8_t, -0.0f, true);
	ASSERT_IN_RANGE(int8_t, FLOAT_NAN, false);
	ASSERT_IN_RANGE(int8_t, FLOAT_INF, false);
	ASSERT_IN_RANGE(int8_t, -FLOAT_INF, false);
	ASSERT_IN_RANGE(uint8_t, -1.0, false);
	ASSERT_IN_RANGE(uint8_t, -0.5, false);
	ASSERT_IN_RANGE(uint8_t, 255.0, true);
	ASSERT_IN_RANGE(uint8_t, 256.0, false);
	ASSERT_IN_RANGE(uint8_t, std::numeric_limits<double>::denorm_min(), false);
	ASSERT_IN_RANGE(int32_t, 2147483520.0f, true);
	ASSERT_IN_RANGE(int32_t, 2147483648.0f, false);
	ASSERT_IN_RANGE(int32_t, -2147483648.0f, true);
	ASSERT_IN_RANGE(int32_t, 2147483647.0, true);
	ASSERT_IN_RANGE(int32_t, 2147483648.0, false);
	ASSERT_IN_RANGE(int32_t, -2147483649.0, false);
	ASSERT_IN_RANGE(uint32_t, 4294967040.0f, true);
	ASSERT_IN_RANGE(uint32_t, 4294967296.0f, false);
	ASSERT_IN_RANGE(uint32_t, 4294967295.0, true);
	ASSERT_IN_RANGE(uint32_t, 4294967296.0, false);
	ASSERT_IN_RANGE(int64_t, 9223372036854774784.0, true);
	ASSERT_IN_RANGE(int64_t, 9223372036854775808.0, false);
	ASSERT_IN_RANGE(int64_t, -9223372036854775808.0, true);
	ASSERT_IN_RANGE(uint64_t, 18446744073709549568.0, true);
	ASSERT_IN_RANGE(uint64_t, 18446744073709551616.0, false);
	ASSERT_IN_RANGE(uint64_t, 1.0e300, false);
	ASSERT_IN_RANGE(uint64_t, DOUBLE_NAN, false);

	// Integral to floating point.
	ASSERT_IN_RANGE(float, int32_t(16777216), true);
	ASSERT_IN_RANGE(float, int32_t(16777217), false);
	ASSERT_IN_RANGE(float, Bond::BOND_INT_MIN, true);
	ASSERT_IN_RANGE(float, Bond::BOND_INT_MAX, false);
	ASSERT_IN_RANGE(float, uint32_t(0xFFFFFF00), true);
	ASSERT_IN_RANGE(float, Bond::BOND_UINT_MAX, false);
	ASSERT_IN_RANGE(float, Bond::BOND_LONG_MIN, true);
	ASSERT_IN_RANGE(float, Bond::BOND_LONG_MAX, false);
	ASSERT_IN_RANGE(float, uint64_t(1) << 63, true);
	ASSERT_IN_RANGE(float, Bond::BOND_ULONG_MAX, false);
	ASSERT_IN_RANGE(double, int64_t(1) << 53, true);
	ASSERT_IN_RANGE(double, (int64_t(1) << 53) + 1, false);
	ASSERT_IN_RANGE(double, Bond::BOND_LONG_MIN, true);
	ASSERT_IN_RANGE(double, Bond::BOND_LONG_MAX, false);
	ASSERT_IN_RANGE(double, uint64_t(0xFFFFFFFFFFFFF800), true);
	ASSERT_IN_RANGE(double, Bond::BOND_ULONG_MAX, false);
	ASSERT_IN_RANGE(double, uint64_t(0), true);

	// Floating point to floating point.
	ASSERT_IN_RANGE(float, 0.5, true);
	ASSERT_IN_RANGE(float, 0.1, false);
	ASSERT_IN_RANGE(float, 1.0e300, false);
	ASSERT_IN_RANGE(float, double(std::numeric_limits<float>::max()), true);
	ASSERT_IN_RANGE(float, -double(std::numeric_limits<float>::max()), true);
	ASSERT_IN_RANGE(float, double(std::numeric_limits<float>::denorm_min()), true);
	ASSERT_IN_RANGE(float, std::numeric_limits<double>::denorm_min(), false);
	ASSERT_IN_RANGE(float, DOUBLE_NAN, false);
	ASSERT_IN_RANGE(float, DOUBLE_INF, false);
	ASSERT_IN_RANGE(double, 0.1f, true);
}

#undef ASSERT_IN_RANGE


#define TEST_ITEMS                              \
  TEST_ITEM(NormalizePath)                      \
  TEST_ITEM(IsAbsolutePath)                     \
  TEST_ITEM(GetDirectory)                       \
  TEST_ITEM(IsInRange)                          \

RUN_TESTS(Util, TEST_ITEMS)
