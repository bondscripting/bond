#ifndef TEST_FRAMEWORK_TESTFRAMEWORK_H
#define TEST_FRAMEWORK_TESTFRAMEWORK_H

#include "bond/io/outputstream.h"

#define DEFINE_TEST(testName) bool __Test ## testName ## __(Bond::OutputStream &logger)

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

#define ASSERT_MESSAGE(condition, message)                                   \
  __ASSERT_FORMAT__(condition, logger, __FILE__, __LINE__, "%s", message)  \

#define ASSERT_FORMAT(condition, ...)                                       \
  __ASSERT_FORMAT__(condition, logger, __FILE__, __LINE__, __VA_ARGS__)     \

#define __ASSERT_FORMAT__(condition, logger, file, line, ...)                \
  do                                                                        \
  {                                                                         \
    if (!(condition))                                                       \
    {                                                                       \
      logger.Print("line %u in %s: ", line, file);                         \
      logger.Print(__VA_ARGS__);                                            \
      return false;                                                         \
    }                                                                       \
  } while (false)


#define ERROR_MESSAGE(message)                                               \
  __ERROR_FORMAT__(logger, __FILE__, __LINE__, "%s", message)              \

#define ERROR_FORMAT(...)                                                    \
  __ERROR_FORMAT__(logger, __FILE__, __LINE__, __VA_ARGS__)                  \

#define __ERROR_FORMAT__(logger, file, line, ...)                             \
  do                                                                        \
  {                                                                         \
    logger.Print("line %u in %s: ", line, file);                            \
    logger.Print(__VA_ARGS__);                                               \
  } while (false)


namespace TestFramework
{

typedef bool TestFunction(Bond::OutputStream &logger);

struct TestItem
{
	const char *testName;
	TestFunction *testFunction;
};

struct TestGroup
{
	const char *groupName;
	const TestItem *items;
	unsigned numItems;
};

bool RunTests(const TestGroup &testGroup);

}

#endif
