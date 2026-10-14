#include "framework/testframework.h"
#include "bond/systems/exception.h"
#include <cstdio>

namespace TestFramework
{

void AssertionFailure::Format(const char *file, int line, const char *format, va_list args)
{
	const int prefixLength = snprintf(mMessage, MESSAGE_BUFFER_LENGTH, "line %d in %s: ", line, file);
	if ((prefixLength >= 0) && (size_t(prefixLength) < MESSAGE_BUFFER_LENGTH))
	{
		vsnprintf(mMessage + prefixLength, MESSAGE_BUFFER_LENGTH - size_t(prefixLength), format, args);
	}
}


AssertionFailure::AssertionFailure(const char *file, int line, const char *format, ...)
{
	va_list args;
	va_start(args, format);
	Format(file, line, format, args);
	va_end(args);
}


AssertionFailure::AssertionFailure(const char *file, int line, const char *format, va_list args)
{
	Format(file, line, format, args);
}


void Assert(bool condition, const char *file, int line, const char *format, ...)
{
	if (!condition)
	{
		va_list args;
		va_start(args, format);
		AssertionFailure failure(file, line, format, args);
		va_end(args);
		throw failure;
	}
}


bool RunTests(const TestGroup &testGroup)
{
	const unsigned numTests = testGroup.numItems;
	unsigned numPassed = 0;
	unsigned numFailed = 0;

	printf("Testing %s\n", testGroup.groupName);

	for (unsigned i = 0; i < numTests; ++i)
	{
		const TestItem &item = testGroup.items[i];
		const char *resultStr = "PASS";
		char message[Bond::Exception::MESSAGE_BUFFER_LENGTH] = "";

		try
		{
			item.testFunction();
			++numPassed;
		}
		catch (const AssertionFailure &e)
		{
			++numFailed;
			resultStr = "FAIL";
			snprintf(message, sizeof(message), "%s", e.what());
		}
		catch (const Bond::Exception &e)
		{
			++numFailed;
			resultStr = "FAIL";
			snprintf(message, sizeof(message), "%s", e.GetMessage());
		}
		catch (const std::exception &e)
		{
			++numFailed;
			resultStr = "FAIL";
			snprintf(message, sizeof(message), "%s", e.what());
		}
		catch (...)
		{
			++numFailed;
			resultStr = "FAIL";
			snprintf(message, sizeof(message), "Unknown exception.");
		}

		printf("\t%u/%u/%u %s: [%s] %s\n", numFailed, numPassed, numTests, item.testName, resultStr, message);
	}

	printf("Results for %s: %u/%u/%u\n", testGroup.groupName, numFailed, numPassed, numTests);

	return numPassed == numTests;
}

}
