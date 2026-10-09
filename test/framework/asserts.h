#ifndef TEST_FRAMEWORK_ASSERTS_H
#define TEST_FRAMEWORK_ASSERTS_H

#include "framework/testframework.h"
#include "bond/compiler/compilererror.h"
#include "bond/compiler/token.h"

// Thin pass-through macros that only capture __FILE__/__LINE__; all branching/throwing logic
// lives in the TestFramework::Assert* functions below.
#define ASSERT_NO_COMPILER_ERRORS(errorBuffer)                                \
  TestFramework::AssertNoCompilerErrors(__FILE__, __LINE__, errorBuffer)

#define ASSERT_COMPILER_ERRORS(errorBuffer, expectedErrors, expectedErrorCount)        \
  TestFramework::AssertCompilerErrors(__FILE__, __LINE__,                             \
    errorBuffer, expectedErrors, expectedErrorCount)


namespace TestFramework
{

struct ExpectedCompilerError
{
	Bond::CompilerError::Type errorType;
	Bond::Token::TokenType context;
	size_t line;
};

/// \brief Throws an AssertionFailure if errorBuffer contains any compiler errors.
void AssertNoCompilerErrors(
	const char *assertFile,
	int assertLine,
	const Bond::CompilerErrorBuffer &errorBuffer);

/// \brief Throws an AssertionFailure if errorBuffer's contents don't match the expected errors.
void AssertCompilerErrors(
	const char *assertFile,
	int assertLine,
	const Bond::CompilerErrorBuffer &errorBuffer,
	const ExpectedCompilerError *expectedErrors,
	size_t numErrors);

}

#endif
