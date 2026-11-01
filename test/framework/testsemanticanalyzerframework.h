#ifndef TEST_FRAMEWORK_TESTSEMANTICANALYZERFRAMEWORK_H
#define TEST_FRAMEWORK_TESTSEMANTICANALYZERFRAMEWORK_H

#include "framework/testframework.h"
#include "bond/compiler/semanticanalyzer.h"

#define DEFINE_SEMANTICANALYZER_TEST(testName, scriptName)                    \
  void __Validate ## testName ## __(                                          \
    Bond::CompilerErrorBuffer &errorBuffer,                                   \
    Bond::SemanticAnalyzer &analyzer);                                        \
                                                                              \
  void __Test ## testName ## __()                                            \
  {                                                                           \
    TestFramework::RunSemanticAnalyzerTest(                                   \
      __FILE__, __LINE__, scriptName, &__Validate ## testName ## __);         \
  }                                                                           \
                                                                              \
  void __Validate ## testName ## __(                                          \
    Bond::CompilerErrorBuffer &errorBuffer,                                   \
    Bond::SemanticAnalyzer &analyzer)                                         \


// Analyzes all of the scripts named by the remaining arguments together.
#define DEFINE_SEMANTICANALYZER_MULTI_TEST(testName, ...)                     \
  void __Validate ## testName ## __(                                          \
    Bond::CompilerErrorBuffer &errorBuffer,                                   \
    Bond::SemanticAnalyzer &analyzer);                                        \
                                                                              \
  void __Test ## testName ## __()                                            \
  {                                                                           \
    const char *const scriptNames[] = {__VA_ARGS__};                          \
    TestFramework::RunSemanticAnalyzerTest(                                   \
      __FILE__, __LINE__, scriptNames, sizeof(scriptNames) / sizeof(*scriptNames), \
      &__Validate ## testName ## __);                                         \
  }                                                                           \
                                                                              \
  void __Validate ## testName ## __(                                          \
    Bond::CompilerErrorBuffer &errorBuffer,                                   \
    Bond::SemanticAnalyzer &analyzer)                                         \


// Thin pass-through macro that captures __FILE__/__LINE__ and forwards to
// TestFramework::AssertAnalysisThrows.
#define ASSERT_ANALYSIS_THROWS(scriptName, expectedMessagePrefix)             \
  TestFramework::AssertAnalysisThrows(__FILE__, __LINE__, (scriptName), (expectedMessagePrefix))


namespace TestFramework
{

typedef void SemanticAnalyzerValidationFunction(
	Bond::CompilerErrorBuffer &errorBuffer,
	Bond::SemanticAnalyzer &analyzer);

void RunSemanticAnalyzerTest(
	const char *assertFile,
	int assertLine,
	const char *scriptName,
	SemanticAnalyzerValidationFunction *validationFunction);


void RunSemanticAnalyzerTest(
	const char *assertFile,
	int assertLine,
	const char *const *scriptNames,
	size_t numScripts,
	SemanticAnalyzerValidationFunction *validationFunction);


/// \brief Analyzes scriptName and asserts that it throws a Bond::Exception whose message starts
/// with expectedPrefix. Throws an AssertionFailure if it does not, or if memory leaks.
void AssertAnalysisThrows(
	const char *assertFile,
	int assertLine,
	const char *scriptName,
	const char *expectedPrefix);

}

#endif
