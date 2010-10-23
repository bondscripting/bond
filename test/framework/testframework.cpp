#include "framework/testframework.h"
#include "bond/bufferedtextwriter.h"
#include <stdio.h>

namespace TestFramework
{

bool RunTests(const TestGroup &testGroup)
{
	const unsigned numTests = testGroup.numItems;
	unsigned numPassed = 0;
	unsigned numFailed = 0;

	printf("Testing %s\n", testGroup.groupName);

	for (unsigned i = 0; i < numTests; ++i)
	{
		const int BUFFER_SIZE = 1024;
		char buffer[BUFFER_SIZE];
		Bond::BufferedTextWriter logger(buffer, BUFFER_SIZE);
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

		printf("\t%u/%u/%u %s: [%s] %s\n", numFailed, numPassed, numTests, item.testName, resultStr, buffer);
	}

	printf("Results for %s: %u/%u/%u\n", testGroup.groupName, numFailed, numPassed, numTests);

	return numPassed == numTests;
}

}
