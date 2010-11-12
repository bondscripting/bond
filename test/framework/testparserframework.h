#ifndef TEST_FRAMEWORK_TESTPARSERFRAMEWORK_H
#define TEST_FRAMEWORK_TESTPARSERFRAMEWORK_H

#include "framework/testframework.h"
#include "bond/parsenodecounter.h"
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
  if (!TestFramework::AssertParseNodeCount(logger, __FILE__, __LINE__, root, expectedCount)) return false; \

#define ASSERT_NO_PARSE_ERRORS(errorBuffer)                                                       \
	if (!TestFramework::AssertNoParseErrors(logger, __FILE__, __LINE__, errorBuffer)) return false; \

#define ASSERT_PARSE_ERRORS(errorBuffer, expectedErrors, expectedErrorCount) \
  if (!TestFramework::AssertParseErrors(logger, __FILE__, __LINE__,          \
      errorBuffer, expectedErrors, expectedErrorCount))                      \
  {                                                                          \
		return false;                                                            \
  }                                                                          \


namespace TestFramework
{

struct ExpectedParseError
{
	Bond::ParseError::Type errorType;
	Bond::Token::TokenType context;
	int line;
};


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
	const Bond::ListParseNode *root,
	const Bond::ParseNodeCount &expectedCount);

bool AssertNoParseErrors(
	Bond::TextWriter &logger,
	const char *assertFile,
	int assertLine,
	const Bond::ParseErrorBuffer &errorBuffer);

bool AssertParseErrors(
	Bond::TextWriter &logger,
	const char *assertFile,
	int assertLine,
	const Bond::ParseErrorBuffer &errorBuffer,
	const ExpectedParseError *expectedErrors,
	int numErrors);

}

#endif
