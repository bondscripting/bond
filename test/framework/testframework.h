#ifndef TEST_FRAMEWORK_TESTFRAMEWORK_H
#define TEST_FRAMEWORK_TESTFRAMEWORK_H

#include <cstdarg>
#include <cstddef>
#include <exception>

#define DEFINE_TEST(testName) void __Test ## testName ## __()

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

// Thin pass-through macros whose only job is to automatically capture the call site's
// __FILE__/__LINE__ (C++17 has no std::source_location). They contain no branching or
// early-return logic; all of that lives in TestFramework::Assert, which throws on failure.
#define ASSERT_MESSAGE(condition, message)                 \
  TestFramework::Assert((condition), __FILE__, __LINE__, "%s", (message))

#define ASSERT_FORMAT(condition, ...)                      \
  TestFramework::Assert((condition), __FILE__, __LINE__, __VA_ARGS__)


namespace TestFramework
{

/// \brief Thrown by TestFramework::Assert (and related assertion helpers) when a test
/// condition fails. Carries a fully formatted message, including the file and line of the
/// failed assertion, so the harness doesn't need any extra context to report the failure.
class AssertionFailure : public std::exception
{
public:
	static const size_t MESSAGE_BUFFER_LENGTH = 1024;

	AssertionFailure(const char *file, int line, const char *format, ...);
	AssertionFailure(const char *file, int line, const char *format, va_list args);

	const char *what() const noexcept override { return mMessage; }

private:
	void Format(const char *file, int line, const char *format, va_list args);

	char mMessage[MESSAGE_BUFFER_LENGTH];
};


/// \brief Throws an AssertionFailure if condition is false; otherwise returns normally.
void Assert(bool condition, const char *file, int line, const char *format, ...);

typedef void TestFunction();

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


/// \brief RAII helper that asserts an allocator has no outstanding allocations when it goes
/// out of scope. If a test is already failing (an exception is propagating), the leak check is
/// skipped so the original failure is the one reported instead of masking it or causing a
/// std::terminate from throwing during unwinding.
template <typename Allocator>
class AllocationLeakGuard
{
public:
	AllocationLeakGuard(Allocator &allocator, const char *name, const char *file, int line):
		mAllocator(allocator),
		mName(name),
		mFile(file),
		mLine(line),
		mUncaughtExceptions(std::uncaught_exceptions())
	{}

	~AllocationLeakGuard() noexcept(false)
	{
		if (std::uncaught_exceptions() == mUncaughtExceptions)
		{
			Assert(mAllocator.GetNumAllocations() == 0, mFile, mLine,
				"%s leaked %d chunks of memory.", mName, mAllocator.GetNumAllocations());
		}
	}

private:
	Allocator &mAllocator;
	const char *mName;
	const char *mFile;
	int mLine;
	int mUncaughtExceptions;
};

}

#endif
