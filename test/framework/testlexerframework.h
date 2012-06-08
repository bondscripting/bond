#ifndef TEST_FRAMEWORK_TESTLEXERFRAMEWORK_H
#define TEST_FRAMEWORK_TESTLEXERFRAMEWORK_H

#include "framework/testframework.h"
#include "bond/compilererror.h"
#include "bond/lexer.h"

#define DEFINE_LEXER_TEST(testName, scriptName)                               \
  bool __Validate ## testName ## __(                                          \
    Bond::TextWriter &logger,                                                 \
    Bond::CompilerErrorBuffer &errorBuffer,                                   \
    Bond::Lexer &lexer);                                                      \
                                                                              \
  bool __Test ## testName ## __(Bond::TextWriter &logger)                     \
  {                                                                           \
    return TestFramework::RunLexerTest(                                       \
      logger, __FILE__, __LINE__, scriptName, &__Validate ## testName ## __); \
  }                                                                           \
                                                                              \
  bool __Validate ## testName ## __(                                          \
    Bond::TextWriter &logger,                                                 \
    Bond::CompilerErrorBuffer &errorBuffer,                                   \
    Bond::Lexer &lexer)                                                       \


namespace TestFramework
{

typedef bool LexerValidationFunction(
	Bond::TextWriter &logger,
	Bond::CompilerErrorBuffer &errorBuffer,
	Bond::Lexer &lexer);

bool RunLexerTest(
	Bond::TextWriter &logger,
	const char *assertFile,
	int assertLine,
	const char *scriptName,
	LexerValidationFunction *validationFunction);

}

#endif
