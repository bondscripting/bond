#ifndef TEST_FRAMEWORK_TESTSEMANTICANALYZERFRAMEWORK_H
#define TEST_FRAMEWORK_TESTSEMANTICANALYZERFRAMEWORK_H

#include "framework/testframework.h"
#include "bond/semanticanalyzer.h"

#define DEFINE_SEMANTICANALYZER_TEST(testName, scriptName)                                       \
  bool __Validate ## testName ## __(Bond::TextWriter &logger, Bond::SemanticAnalyzer &analyzer); \
                                                                                                 \
  bool __Test ## testName ## __(Bond::TextWriter &logger)                                        \
  {                                                                                              \
    return TestFramework::RunSemanticAnalyzerTest(                                               \
      logger, __FILE__, __LINE__, scriptName, &__Validate ## testName ## __);                    \
  }                                                                                              \
                                                                                                 \
  bool __Validate ## testName ## __(Bond::TextWriter &logger, Bond::SemanticAnalyzer &analyzer)  \


namespace TestFramework
{

typedef bool SemanticAnalyzerValidationFunction(Bond::TextWriter &logger, Bond::SemanticAnalyzer &analyzer);

bool RunSemanticAnalyzerTest(
	Bond::TextWriter &logger,
	const char *assertFile,
	int assertLine,
	const char *scriptName,
	SemanticAnalyzerValidationFunction *validationFunction);

}

#endif