#include "framework/asserts.h"
#include "bond/io/memoryoutputstream.h"

namespace TestFramework
{

void AssertNoCompilerErrors(
	const char *assertFile,
	int assertLine,
	const Bond::CompilerErrorBuffer &errorBuffer)
{
	if (errorBuffer.HasErrors())
	{
		char buffer[768];
		Bond::MemoryOutputStream stream(buffer, sizeof(buffer));
		errorBuffer.Print(stream);
		Assert(false, assertFile, assertLine, "Unexpected compiler errors:\n\t\t%s", buffer);
	}
}


void AssertCompilerErrors(
	const char *assertFile,
	int assertLine,
	const Bond::CompilerErrorBuffer &errorBuffer,
	const ExpectedCompilerError *expectedErrors,
	size_t numErrors)
{
	const size_t count = (errorBuffer.GetNumErrors() < numErrors) ? errorBuffer.GetNumErrors() : numErrors;

	for (size_t i = 0; i < count; ++i)
	{
		const ExpectedCompilerError *expected = expectedErrors + i;
		const Bond::CompilerError &actual = errorBuffer.GetError(i);
		const Bond::Token *context = actual.GetContext();
		const Bond::StreamPos &pos = context->GetStartPos();
		Assert(expected->errorType == actual.GetType(), assertFile, assertLine,
			"Expected type of error %d to be %s but was %s.", i,
			Bond::CompilerError::GetErrorName(expected->errorType),
			actual.GetErrorName());
		Assert(expected->context == context->GetTokenType(), assertFile, assertLine,
			"Expected context of error %d to be %s but was %s.", i,
			Bond::Token::GetTokenName(expected->context), context->GetTokenName());
		Assert(expected->line == pos.line, assertFile, assertLine,
			"Expected error %d to be on line %d but was on %d.", i, expected->line, pos.line);
	}

	Assert(errorBuffer.GetNumErrors() == numErrors, assertFile, assertLine,
		"Expected %d errors but found %d.", numErrors, errorBuffer.GetNumErrors());
}

}
