#ifndef TEST_FRAMEWORK_TESTFRAMEWORK_H
#define TEST_FRAMEWORK_TESTFRAMEWORK_H

#include "bond/textwriter.h"

#define DEFINE_TEST(testName) bool __Test ## testName ## __(Bond::TextWriter &logger)

#define TEST_ITEM(testName) {#testName, &__Test ## testName ## __ },

#define RUN_TESTS(groupName, TEST_ITEMS)                               \
  int main()                                                           \
  {                                                                    \
    const TestFramework::TestItem groupName ## _Items[] =              \
    {                                                                  \
      TEST_ITEMS                                                       \
    };                                                                 \
                                                                       \
    const TestFramework::TestGroup groupName ## _Group =               \
    {                                                                  \
      #groupName,                                                      \
      groupName ## _Items,                                             \
      sizeof(groupName ## _Items) / sizeof(*groupName ## _Items)       \
    };                                                                 \
                                                                       \
    const bool success = TestFramework::RunTests(groupName ## _Group); \
    return success ? 0 : 1;                                            \
  }                                                                    \

#define ASSERT_MESSAGE(condition, message)                                  \
  __ASSERT_FORMAT__(condition, logger, __FILE__, __LINE__, ("%s", message)) \

#define ASSERT_FORMAT(condition, format)                                    \
  __ASSERT_FORMAT__(condition, logger, __FILE__, __LINE__, format)          \

#define __ASSERT_FORMAT__(condition, logger, file, line, format)            \
  if (!(condition))                                                         \
  {                                                                         \
    logger.Write("line %u in %s: ", line, file);                            \
    logger.Write format;                                                    \
    return false;                                                           \
  }                                                                         \


namespace TestFramework
{

typedef bool TestFunction(Bond::TextWriter &logger);

struct TestItem
{
	const char *testName;
	const TestFunction *testFunction;
};

struct TestGroup
{
	const char *groupName;
	const TestItem *items;
	const unsigned numItems;
};

bool RunTests(const TestGroup &testGroup);

}

#endif
