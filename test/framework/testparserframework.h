#ifndef TEST_FRAMEWORK_TESTPARSERFRAMEWORK_H
#define TEST_FRAMEWORK_TESTPARSERFRAMEWORK_H

#include "framework/testframework.h"
#include "bond/compiler/compilererror.h"
#include "bond/compiler/parsenodes.h"
#include "bond/tools/parsenodecounter.h"

#define DEFINE_PARSER_TEST(testName, scriptName)                              \
  bool __Validate ## testName ## __(                                          \
    Bond::OutputStream &logger,                                               \
    Bond::CompilerErrorBuffer &errorBuffer,                                   \
    Bond::TranslationUnit *translationUnit);                                  \
                                                                              \
  bool __Test ## testName ## __(Bond::OutputStream &logger)                   \
  {                                                                           \
    return TestFramework::RunParserTest(                                      \
      logger, __FILE__, __LINE__, scriptName, &__Validate ## testName ## __); \
  }                                                                           \
                                                                              \
  bool __Validate ## testName ## __(                                          \
    Bond::OutputStream &logger,                                               \
    Bond::CompilerErrorBuffer &errorBuffer,                                   \
    Bond::TranslationUnit *translationUnit)                                   \

#define ASSERT_PARSE_NODE_COUNT(parseNode, expectedCount)                                                       \
  if (!TestFramework::AssertParseNodeCount(logger, __FILE__, __LINE__, parseNode, expectedCount)) return false; \


namespace TestFramework
{

typedef bool ParserValidationFunction(
	Bond::OutputStream &logger,
	Bond::CompilerErrorBuffer &errorBuffer,
	Bond::TranslationUnit *translationUnit);

bool RunParserTest(
	Bond::OutputStream &logger,
	const char *assertFile,
	int assertLine,
	const char *scriptName,
	ParserValidationFunction *validationFunction);

bool AssertParseNodeCount(
	Bond::OutputStream &logger,
	const char *assertFile,
	int assertLine,
	const Bond::ListParseNode *root,
	const Bond::ParseNodeCount &expectedCount);

}

#endif
