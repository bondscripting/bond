#ifndef TEST_FRAMEWORK_TESTLEXERFRAMEWORK_H
#define TEST_FRAMEWORK_TESTLEXERFRAMEWORK_H

#include "framework/testframework.h"
#include "bond/compiler/compilererror.h"
#include "bond/compiler/lexer.h"

#define DEFINE_LEXER_TEST(testName, scriptName)                               \
  bool __Validate ## testName ## __(                                          \
    Bond::OutputStream &logger,                                               \
    Bond::CompilerErrorBuffer &errorBuffer,                                   \
    const Bond::TokenCollection &tokenCollection);                            \
                                                                              \
  bool __Test ## testName ## __(Bond::OutputStream &logger)                   \
  {                                                                           \
    return TestFramework::RunLexerTest(                                       \
      logger, __FILE__, __LINE__, scriptName, &__Validate ## testName ## __); \
  }                                                                           \
                                                                              \
  bool __Validate ## testName ## __(                                          \
    Bond::OutputStream &logger,                                               \
    Bond::CompilerErrorBuffer &errorBuffer,                                   \
    const Bond::TokenCollection &tokenCollection)                             \


namespace TestFramework
{

typedef bool LexerValidationFunction(
	Bond::OutputStream &logger,
	Bond::CompilerErrorBuffer &errorBuffer,
	const Bond::TokenCollection &tokenCollection);

bool RunLexerTest(
	Bond::OutputStream &logger,
	const char *assertFile,
	int assertLine,
	const char *scriptName,
	LexerValidationFunction *validationFunction);

}

#endif
