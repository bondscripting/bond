#include "framework/asserts.h"

namespace TestFramework
{

bool AssertNoCompilerErrors(
	Bond::TextWriter &logger,
	const char *assertFile,
	int assertLine,
	const Bond::CompilerErrorBuffer &errorBuffer)
{
	if (errorBuffer.HasErrors())
	{
		logger.Write("line %u in %s:\n", assertLine, assertFile);

		for (int i = 0; i < errorBuffer.GetNumErrors(); ++i)
		{
			logger.Write("\t\t");
			const Bond::CompilerError &error = errorBuffer.GetError(i);
			error.Print(logger);
			logger.Write("\n");
		}
		return false;
	}

	return true;
}


bool AssertCompilerErrors(
	Bond::TextWriter &logger,
	const char *assertFile,
	int assertLine,
	const Bond::CompilerErrorBuffer &errorBuffer,
	const ExpectedCompilerError *expectedErrors,
	int numErrors)
{
	const int count = (errorBuffer.GetNumErrors() < numErrors) ? errorBuffer.GetNumErrors() : numErrors;

	for (int i = 0; i < count; ++i)
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
