#ifndef TEST_FRAMEWORK_ASSERTS_H
#define TEST_FRAMEWORK_ASSERTS_H

#include "framework/testframework.h"
#include "bond/compilererror.h"
#include "bond/token.h"

#define ASSERT_NO_COMPILER_ERRORS(errorBuffer)                                         \
  if (!TestFramework::AssertNoCompilerErrors(logger, __FILE__, __LINE__, errorBuffer)) \
  {                                                                                    \
    return false;                                                                      \
  }                                                                                    \

#define ASSERT_COMPILER_ERRORS(errorBuffer, expectedErrors, expectedErrorCount)        \
  if (!TestFramework::AssertCompilerErrors(logger, __FILE__, __LINE__,                 \
      errorBuffer, expectedErrors, expectedErrorCount))                                \
  {                                                                                    \
    return false;                                                                      \
  }                                                                                    \


namespace TestFramework
{

struct ExpectedCompilerError
{
	Bond::CompilerError::Type errorType;
	Bond::Token::TokenType context;
	size_t line;
};

bool AssertNoCompilerErrors(
	Bond::TextWriter &logger,
	const char *assertFile,
	size_t assertLine,
	const Bond::CompilerErrorBuffer &errorBuffer);

bool AssertCompilerErrors(
	Bond::TextWriter &logger,
	const char *assertFile,
	size_t assertLine,
	const Bond::CompilerErrorBuffer &errorBuffer,
	const ExpectedCompilerError *expectedErrors,
	size_t numErrors);

}

#endif
