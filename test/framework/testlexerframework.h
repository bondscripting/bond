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

/// \brief Like DEFINE_LEXER_TEST, but lexes the given string literal rather than a script file.
/// Use it for input that can't be reliably checked in, such as CRLF line endings or a file with
/// no trailing newline.
#define DEFINE_LEXER_BUFFER_TEST(testName, bufferLiteral)                     \
  void __Validate ## testName ## __(                                          \
    Bond::CompilerErrorBuffer &errorBuffer,                                   \
    const Bond::TokenCollection &tokenCollection);                            \
                                                                              \
  void __Test ## testName ## __()                                            \
  {                                                                           \
    TestFramework::RunLexerTest(                                              \
      __FILE__, __LINE__, #testName, bufferLiteral,                           \
      sizeof(bufferLiteral) - 1, &__Validate ## testName ## __);              \
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

void RunLexerTest(
	const char *assertFile,
	int assertLine,
	const char *scriptName,
	const char *buffer,
	size_t length,
	LexerValidationFunction *validationFunction);

}

#endif
