#include "framework/testframework.h"
#include "bond/compiler/charstream.h"

DEFINE_TEST(ReadAndReposition)
{
	const char TEXT[] = "ab\n\nc\nd";
	const size_t LENGTH = sizeof(TEXT) / sizeof(*TEXT) - 1;
	Bond::CharStream cs(TEXT, sizeof(TEXT) - 1);

	for (size_t i = 0; i < LENGTH; ++i)
	{
		const Bond::StreamPos pos = cs.GetStreamPos();
		ASSERT_FORMAT(pos.index == i, ("Expected index to be %d, but was %d.", i, pos.index));
		ASSERT_MESSAGE(cs.HasNext(), "Unexpected end of stream.");
		const char c = cs.Next();
		ASSERT_FORMAT(c == TEXT[i], ("Expected character %d to be '%c', but was '%c'.", i, TEXT[i], c));
	}
	{
		const Bond::StreamPos pos = cs.GetStreamPos();
		ASSERT_FORMAT(pos.line == 4, ("Expected line to be 4, but was %d.", pos.line));
		ASSERT_FORMAT(pos.column == 2, ("Expected column to be 2, but was %d.", pos.column));
		ASSERT_MESSAGE(cs.HasNext(), "Unexpected end of stream.");
		const char c = cs.Next();
		ASSERT_FORMAT(c == ' ', ("Expected final character to be space, but was '%c'.", c));
		ASSERT_MESSAGE(!cs.HasNext(), "Expected end of stream.");
	}
	{
		cs.Unget(2);
		const Bond::StreamPos pos = cs.GetStreamPos();
		ASSERT_FORMAT(pos.line == 4, ("Expected line to be 4, but was %d.", pos.line));
		ASSERT_FORMAT(pos.column == 1, ("Expected column to be 2, but was %d.", pos.column));
		const char c = cs.Peek();
		ASSERT_FORMAT(c == 'd', ("Expected character to be 'd', but was '%c'.", c));
	}
	{
		cs.Unget(1);
		const Bond::StreamPos pos = cs.GetStreamPos();
		ASSERT_FORMAT(pos.line == 3, ("Expected line to be 3, but was %d.", pos.line));
		ASSERT_FORMAT(pos.column == 2, ("Expected column to be 2, but was %d.", pos.column));
		const char c = cs.Peek();
		ASSERT_FORMAT(c == '\n', ("Expected character to be 'newline', but was '%c'.", c));
	}
	{
		cs.Unget(1);
		const Bond::StreamPos pos = cs.GetStreamPos();
		ASSERT_FORMAT(pos.line == 3, ("Expected line to be 3, but was %d.", pos.line));
		ASSERT_FORMAT(pos.column == 1, ("Expected column to be 1, but was %d.", pos.column));
		const char c = cs.Peek();
		ASSERT_FORMAT(c == 'c', ("Expected character to be 'c', but was '%c'.", c));
	}
	{
		cs.Unget(3);
		const Bond::StreamPos pos = cs.GetStreamPos();
		ASSERT_FORMAT(pos.line == 1, ("Expected line to be 1, but was %d.", pos.line));
		ASSERT_FORMAT(pos.column == 2, ("Expected column to be 2, but was %d.", pos.column));
		const char c = cs.Peek();
		ASSERT_FORMAT(c == 'b', ("Expected character to be 'b', but was '%c'.", c));
	}

	return true;
}

#define TEST_ITEMS             \
  TEST_ITEM(ReadAndReposition) \

RUN_TESTS(CharStream, TEST_ITEMS)
