#ifndef TEST_FRAMEWORK_TESTPARSERFRAMEWORK_H
#define TEST_FRAMEWORK_TESTPARSERFRAMEWORK_H

#include "framework/testframework.h"
#include "bond/compilererror.h"
#include "bond/parsenodecounter.h"
#include "bond/parser.h"

#define DEFINE_PARSER_TEST(testName, scriptName)                                                               \
  bool __Validate ## testName ## __(                                                                           \
    Bond::TextWriter &logger,                                                                                  \
    Bond::CompilerErrorBuffer &errorBuffer,                                                                       \
    Bond::Parser &parser);                                                                                     \
                                                                                                               \
  bool __Test ## testName ## __(Bond::TextWriter &logger)                                                      \
  {                                                                                                            \
    return TestFramework::RunParserTest(logger, __FILE__, __LINE__, scriptName, &__Validate ## testName ## __);\
  }                                                                                                            \
                                                                                                               \
  bool __Validate ## testName ## __(                                                                           \
    Bond::TextWriter &logger,                                                                                  \
    Bond::CompilerErrorBuffer &errorBuffer,                                                                       \
    Bond::Parser &parser)                                                                                      \

#define ASSERT_PARSE_NODE_COUNT(parseNode, expectedCount)                                                  \
  if (!TestFramework::AssertParseNodeCount(logger, __FILE__, __LINE__, root, expectedCount)) return false; \

#define ASSERT_NO_PARSE_ERRORS(errorBuffer)                                                       \
	if (!TestFramework::AssertNoCompilerErrors(logger, __FILE__, __LINE__, errorBuffer)) return false; \

#define ASSERT_PARSE_ERRORS(errorBuffer, expectedErrors, expectedErrorCount) \
  if (!TestFramework::AssertCompilerErrors(logger, __FILE__, __LINE__,          \
      errorBuffer, expectedErrors, expectedErrorCount))                      \
  {                                                                          \
		return false;                                                            \
  }                                                                          \


namespace TestFramework
{

struct ExpectedCompilerError
{
	Bond::CompilerError::Type errorType;
	Bond::Token::TokenType context;
	int line;
};


typedef bool ParserValidationFunction(
	Bond::TextWriter &logger,
	Bond::CompilerErrorBuffer &errorBuffer,
	Bond::Parser &parser);

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

bool AssertNoCompilerErrors(
	Bond::TextWriter &logger,
	const char *assertFile,
	int assertLine,
	const Bond::CompilerErrorBuffer &errorBuffer);

bool AssertCompilerErrors(
	Bond::TextWriter &logger,
	const char *assertFile,
	int assertLine,
	const Bond::CompilerErrorBuffer &errorBuffer,
	const ExpectedCompilerError *expectedErrors,
	int numErrors);

}

#endif
