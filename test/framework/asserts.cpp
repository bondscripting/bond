#include "framework/asserts.h"

namespace TestFramework
{

bool AssertNoCompilerErrors(
	Bond::OutputStream &logger,
	const char *assertFile,
	size_t assertLine,
	const Bond::CompilerErrorBuffer &errorBuffer)
{
	if (errorBuffer.HasErrors())
	{
		logger.Print("line %u in %s:\n", assertLine, assertFile);

		for (size_t i = 0; i < errorBuffer.GetNumErrors(); ++i)
		{
			logger.Print("\t\t");
			const Bond::CompilerError &error = errorBuffer.GetError(i);
			error.Print(logger);
			logger.Print("\n");
		}
		return false;
	}

	return true;
}


bool AssertCompilerErrors(
	Bond::OutputStream &logger,
	const char *assertFile,
	size_t assertLine,
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
		__ASSERT_FORMAT__(expected->errorType == actual.GetType(), logger, assertFile, assertLine,
			("Expected type of error %d to be %s but was %s.", i,
			Bond::CompilerError::GetErrorName(expected->errorType),
			actual.GetErrorName()));
		__ASSERT_FORMAT__(expected->context == context->GetTokenType(), logger, assertFile, assertLine,
			("Expected context of error %d to be %s but was %s.", i,
			Bond::Token::GetTokenName(expected->context), context->GetTokenName()));
		__ASSERT_FORMAT__(expected->line == pos.line, logger, assertFile, assertLine,
			("Expected error %d to be on line %d but was on %d.", i, expected->line, pos.line));
	}

	__ASSERT_FORMAT__(errorBuffer.GetNumErrors() == numErrors, logger, assertFile, assertLine,
		("Expected %d errors but found %d.", numErrors, errorBuffer.GetNumErrors()));

	return true;
}

}
