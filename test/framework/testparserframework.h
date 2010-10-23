#ifndef TEST_FRAMEWORK_TESTPARSERFRAMEWORK_H
#define TEST_FRAMEWORK_TESTPARSERFRAMEWORK_H

#include "framework/testframework.h"
#include "framework/parsenodecounter.h"
#include "bond/parser.h"

#define DEFINE_PARSER_TEST(testName, scriptName)                                                               \
  bool __Validate ## testName ## __(Bond::TextWriter &logger, Bond::Parser &parser);                           \
                                                                                                               \
  bool __Test ## testName ## __(Bond::TextWriter &logger)                                                      \
  {                                                                                                            \
    return TestFramework::RunParserTest(logger, __FILE__, __LINE__, scriptName, &__Validate ## testName ## __);\
  }                                                                                                            \
                                                                                                               \
  bool __Validate ## testName ## __(Bond::TextWriter &logger, Bond::Parser &parser)                            \

 #define ASSERT_PARSE_NODE_COUNT(parseNode, expectedCount)                                                  \
  if (!TestFramework::AssertParseNodeCount(logger, __FILE__, __LINE__, root, expectedCount)) return false;

#define ASSERT_NO_PARSE_ERRORS()                                                             \
	if (!TestFramework::AssertNoParseErrors(logger, __FILE__, __LINE__, parser)) return false;


namespace TestFramework
{

typedef bool ParserValidationFunction(Bond::TextWriter &logger, Bond::Parser &parser);

bool RunParserTest(
	Bond::TextWriter &logger,
	const char *assertFile,
	int assertLine,
	const char *scriptName,
	ParserValidationFunction *validationFunction);

bool AssertParseNodeCount(
	Bond::TextWriter &logger,
	const char *assertFile,
	int assertLine,
	const Bond::ParseNode *root,
	const ParseNodeCount &expectedCount);

bool AssertNoParseErrors(
	Bond::TextWriter &logger,
	const char *assertFile,
	int assertLine,
	const Bond::Parser &parser);

}

#endif
