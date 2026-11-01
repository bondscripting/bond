#ifndef TEST_FRAMEWORK_ASSERTS_H
#define TEST_FRAMEWORK_ASSERTS_H

#include "framework/testframework.h"
#include "bond/compiler/compilererror.h"
#include "bond/compiler/token.h"
#include "bond/systems/exception.h"
#include <cstring>

// Thin pass-through macros that only capture __FILE__/__LINE__; all branching/throwing logic
// lives in the TestFramework::Assert* functions below.
#define ASSERT_NO_COMPILER_ERRORS(errorBuffer)                                \
  TestFramework::AssertNoCompilerErrors(__FILE__, __LINE__, errorBuffer)

#define ASSERT_COMPILER_ERRORS(errorBuffer, expectedErrors, expectedErrorCount)        \
  TestFramework::AssertCompilerErrors(__FILE__, __LINE__,                             \
    errorBuffer, expectedErrors, expectedErrorCount)

// Thin pass-through macro that captures __FILE__/__LINE__ and wraps the statements in a lambda
// for TestFramework::AssertThrowsBondException. A null prefix accepts any message.
#define ASSERT_BOND_EXCEPTION(expectedMessagePrefix, ...)                                      \
  TestFramework::AssertThrowsBondException(                                                    \
    __FILE__, __LINE__, (expectedMessagePrefix), [&]() { __VA_ARGS__; })


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


/// \brief Calls function and asserts that it throws a Bond::Exception whose message starts with
/// expectedPrefix. A null expectedPrefix accepts any message. Throws an AssertionFailure if no
/// Bond::Exception is thrown or the message does not match.
template <typename FunctionT>
void AssertThrowsBondException(
	const char *assertFile,
	int assertLine,
	const char *expectedPrefix,
	FunctionT function)
{
	try
	{
		function();
	}
	catch (const Bond::Exception &e)
	{
		const char *message = e.GetMessage();
		Assert((expectedPrefix == nullptr) || (strncmp(message, expectedPrefix, strlen(expectedPrefix)) == 0),
			assertFile, assertLine, "Expected an exception starting with \"%s\", but was \"%s\".",
			(expectedPrefix != nullptr) ? expectedPrefix : "", message);
		return;
	}
	throw AssertionFailure(assertFile, assertLine, "Expected an exception starting with \"%s\", but none was thrown.",
		(expectedPrefix != nullptr) ? expectedPrefix : "");
}

}

#endif
