#include "framework/testframework.h"
#include "bond/io/path.h"
#include "bond/systems/defaultallocator.h"
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


#define TEST_ITEMS                              \
  TEST_ITEM(NormalizePath)                      \
  TEST_ITEM(IsAbsolutePath)                     \
  TEST_ITEM(GetDirectory)                       \

RUN_TESTS(Util, TEST_ITEMS)
