#include "framework/testframework.h"
#include <stdio.h>
#include <stdarg.h>

namespace TestFramework
{

Logger::Logger():
	mIndex(0)
{
	mBuffer[0] = '\0';
}


void Logger::Log(const char *format, ...)
{
	unsigned length = BUFFER_SIZE - mIndex;
	char *buffer = mBuffer + mIndex;
	va_list argList;
	va_start(argList, format);
	int n = vsnprintf(buffer, length, format, argList);

	if (n > 0)
	{
		mIndex += n;
		if (mIndex >= BUFFER_SIZE)
		{
			mIndex = BUFFER_SIZE - 1;
		}
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
		Logger logger;
		const TestItem &item = testGroup.items[i];
		bool result = item.testFunction(logger);
		const char *resultStr = "";

		if (result)
		{
			++numPassed;
			resultStr = "PASS";
		}
		else
		{
			++numFailed;
			resultStr = "FAIL";
		}

		printf("\t%u/%u/%u %s: [%s] %s\n", numFailed, numPassed, numTests, item.testName, resultStr, logger.GetLog());
	}

	printf("Results for %s: %u/%u/%u\n", testGroup.groupName, numFailed, numPassed, numTests);

	return numPassed == numTests;
}

}
