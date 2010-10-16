#ifndef TEST_FRAMEWORK_TESTLEXERFRAMEWORK_H
#define TEST_FRAMEWORK_TESTLEXERFRAMEWORK_H

#include "framework/testframework.h"
#include "bond/lexer.h"

#define DEFINE_LEXER_TEST(testName, scriptName)                                                 \
  bool __Validate ## testName ## __(TestFramework::Logger &logger, Bond::Lexer &lexer);         \
                                                                                                \
  bool __Test ## testName ## __(TestFramework::Logger &logger)                                  \
  {                                                                                             \
    return RunLexerTest(logger, __FILE__, __LINE__, scriptName, &__Validate ## testName ## __); \
  }                                                                                             \
                                                                                                \
  bool __Validate ## testName ## __(TestFramework::Logger &logger, Bond::Lexer &lexer)          \


namespace TestFramework
{

typedef bool LexerValidationFunction(Logger &logger, Bond::Lexer &lexer);

bool RunLexerTest(
	Logger &logger,
	const char *assertFile,
	int assertLine,
	const char *scriptName,
	LexerValidationFunction *validationFunction);

}

#endif
