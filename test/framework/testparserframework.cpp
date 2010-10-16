#include "framework/testparserframework.h"
#include "framework/utils.h"
#include "bond/defaultallocator.h"
#include "bond/lexer.h"

namespace TestFramework
{

static bool RunParserTest(
	Logger &logger,
	const char *assertFile,
	int assertLine,
	const FileData &script,
	ParserValidationFunction *validationFunction);


bool RunParserTest(
	Logger &logger,
	const char *assertFile,
	int assertLine,
	const char *scriptName,
	ParserValidationFunction *validationFunction)
{
	__ASSERT_FORMAT__(scriptName != 0, logger, assertFile, assertLine, ("Script name is NULL."));
	__ASSERT_FORMAT__(validationFunction != 0, logger, assertFile, assertLine, ("Validation function is NULL."));

	FileData script = ReadFile(scriptName);
	__ASSERT_FORMAT__(script.length >= 0, logger, assertFile, assertLine,
		("Failed to load file '%s'.", scriptName));

	// Delegate to another function so we can still clean up even if something bails during the test.
	const bool result = RunParserTest(logger, assertFile, assertLine, script, validationFunction);

	DisposeFile(script);

	return result;
}


static bool RunParserTest(
	Logger &logger,
	const char *assertFile,
	int assertLine,
	const FileData &script,
	ParserValidationFunction *validationFunction)
{
	bool result = true;

	Bond::DefaultAllocator lexerAllocator;
	Bond::DefaultAllocator parserAllocator;
	{
		Bond::Lexer lexer(lexerAllocator);
		lexer.Lex(script.data, script.length);
		Bond::TokenStream stream = lexer.GetTokenStream();
		Bond::Parser parser(parserAllocator);
		parser.Parse(stream);
		result = validationFunction(logger, parser);
	}

	__ASSERT_FORMAT__(lexerAllocator.GetNumAllocations() == 0, logger, assertFile, assertLine,
		("Lexer leaked %d chunks of memory.", parserAllocator.GetNumAllocations()));
	__ASSERT_FORMAT__(parserAllocator.GetNumAllocations() == 0, logger, assertFile, assertLine,
		("Parser leaked %d chunks of memory.", parserAllocator.GetNumAllocations()));

	return result;
}

}
