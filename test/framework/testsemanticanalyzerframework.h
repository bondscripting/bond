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

}

#endif
