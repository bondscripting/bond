#ifndef TEST_FRAMEWORK_TEST_H
#define TEST_FRAMEWORK_TEST_H

#define DEFINE_TEST(testName) bool testName(TestFramework::Logger &logger)

#define TEST_ITEM(testName) {#testName, &testName},    

#define TEST_GROUP(groupName, TEST_ITEMS)                      \
  const TestFramework::TestItem groupName ## _Items[] =        \
  {                                                            \
    TEST_ITEMS                                                 \
  };                                                           \
                                                               \
  const TestFramework::TestGroup groupName ## _Group =         \
  {                                                            \
    #groupName,                                                \
    groupName ## _Items,                                       \
    sizeof(groupName ## _Items) / sizeof(*groupName ## _Items) \
  }                                                            \

#define RUN_TESTS(groupName) TestFramework::RunTests(groupName ## _Group)

#define ASSERT_MESSAGE(condition, message)                                  \
  __ASSERT_FORMAT__(condition, logger, __FILE__, __LINE__, ("%s", message)) \

#define ASSERT_FORMAT(condition, format)                                    \
  __ASSERT_FORMAT__(condition, logger, __FILE__, __LINE__, format)          \

#define __ASSERT_FORMAT__(condition, logger, file, line, format)            \
  if (!(condition))                                                         \
  {                                                                         \
    logger.Log("line %u in %s: ", line, file);                              \
    logger.Log format;                                                      \
    return false;                                                           \
  }                                                                         \

namespace TestFramework
{

class Logger
{
public:
	Logger();

	void Log(const char *format, ...);
	const char *GetLog() const { return mBuffer; }

private:
	static const unsigned BUFFER_SIZE = 1024;
	char mBuffer[BUFFER_SIZE];
	unsigned mIndex;
};

typedef bool TestFunction(Logger &logger);

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

void RunTests(const TestGroup &testGroup);

}

#endif
