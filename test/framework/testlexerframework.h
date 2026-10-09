#ifndef TEST_FRAMEWORK_TESTLEXERFRAMEWORK_H
#define TEST_FRAMEWORK_TESTLEXERFRAMEWORK_H

#include "framework/testframework.h"
#include "bond/compiler/compilererror.h"
#include "bond/compiler/lexer.h"

#define DEFINE_LEXER_TEST(testName, scriptName)                               \
  void __Validate ## testName ## __(                                          \
    Bond::CompilerErrorBuffer &errorBuffer,                                   \
    const Bond::TokenCollection &tokenCollection);                            \
                                                                              \
  void __Test ## testName ## __()                                            \
  {                                                                           \
    TestFramework::RunLexerTest(                                              \
      __FILE__, __LINE__, scriptName, &__Validate ## testName ## __);         \
  }                                                                           \
                                                                              \
  void __Validate ## testName ## __(                                          \
    Bond::CompilerErrorBuffer &errorBuffer,                                   \
    const Bond::TokenCollection &tokenCollection)                             \


namespace TestFramework
{

typedef void LexerValidationFunction(
	Bond::CompilerErrorBuffer &errorBuffer,
	const Bond::TokenCollection &tokenCollection);

void RunLexerTest(
	const char *assertFile,
	int assertLine,
	const char *scriptName,
	LexerValidationFunction *validationFunction);

}

#endif
