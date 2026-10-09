#ifndef TEST_FRAMEWORK_TESTPARSERFRAMEWORK_H
#define TEST_FRAMEWORK_TESTPARSERFRAMEWORK_H

#include "framework/testframework.h"
#include "bond/compiler/compilererror.h"
#include "bond/compiler/parsenodes.h"
#include "bond/tools/parsenodecounter.h"

#define DEFINE_PARSER_TEST(testName, scriptName)                              \
  void __Validate ## testName ## __(                                          \
    Bond::CompilerErrorBuffer &errorBuffer,                                   \
    Bond::TranslationUnit *translationUnit);                                  \
                                                                              \
  void __Test ## testName ## __()                                            \
  {                                                                           \
    TestFramework::RunParserTest(                                             \
      __FILE__, __LINE__, scriptName, &__Validate ## testName ## __);         \
  }                                                                           \
                                                                              \
  void __Validate ## testName ## __(                                          \
    Bond::CompilerErrorBuffer &errorBuffer,                                   \
    Bond::TranslationUnit *translationUnit)                                   \

#define ASSERT_PARSE_NODE_COUNT(parseNode, expectedCount)                     \
  TestFramework::AssertParseNodeCount(__FILE__, __LINE__, parseNode, expectedCount)


namespace TestFramework
{

typedef void ParserValidationFunction(
	Bond::CompilerErrorBuffer &errorBuffer,
	Bond::TranslationUnit *translationUnit);

void RunParserTest(
	const char *assertFile,
	int assertLine,
	const char *scriptName,
	ParserValidationFunction *validationFunction);

/// \brief Throws an AssertionFailure if the parse node counts rooted at parseNode don't match
/// expectedCount (a count of -1 for a given field means "don't care").
void AssertParseNodeCount(
	const char *assertFile,
	int assertLine,
	const Bond::ListParseNode *parseNode,
	const Bond::ParseNodeCount &expectedCount);

}

#endif
