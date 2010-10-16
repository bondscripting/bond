#ifndef TEST_FRAMEWORK_TESTPARSERFRAMEWORK_H
#define TEST_FRAMEWORK_TESTPARSERFRAMEWORK_H

#include "framework/testframework.h"
#include "bond/parser.h"

#define DEFINE_PARSER_TEST(testName, scriptName)                                                 \
  bool __Validate ## testName ## __(TestFramework::Logger &logger, Bond::Parser &parser);         \
                                                                                                \
  bool __Test ## testName ## __(TestFramework::Logger &logger)                                  \
  {                                                                                             \
    return RunParserTest(logger, __FILE__, __LINE__, scriptName, &__Validate ## testName ## __); \
  }                                                                                             \
                                                                                                \
  bool __Validate ## testName ## __(TestFramework::Logger &logger, Bond::Parser &parser)          \


namespace TestFramework
{

typedef bool ParserValidationFunction(Logger &logger, Bond::Parser &parser);

bool RunParserTest(
	Logger &logger,
	const char *assertFile,
	int assertLine,
	const char *scriptName,
	ParserValidationFunction *validationFunction);

}

#endif
