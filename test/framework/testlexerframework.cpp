#include "framework/testlexerframework.h"
#include "framework/utils.h"
#include "bond/defaultallocator.h"
#include "bond/textwriter.h"

namespace TestFramework
{

static bool RunLexerTest(
	Bond::TextWriter &logger,
	const char *assertFile,
	int assertLine,
	const FileData &script,
	LexerValidationFunction *validationFunction);


bool RunLexerTest(
	Bond::TextWriter &logger,
	const char *assertFile,
	int assertLine,
	const char *scriptName,
	LexerValidationFunction *validationFunction)
{
	__ASSERT_FORMAT__(scriptName != 0, logger, assertFile, assertLine, ("Script name is NULL."));
	__ASSERT_FORMAT__(validationFunction != 0, logger, assertFile, assertLine, ("Validation function is NULL."));

	FileData script = ReadFile(scriptName);
	__ASSERT_FORMAT__(script.length >= 0, logger, assertFile, assertLine,
		("Failed to load file '%s'.", scriptName));

	// Delegate to another function so we can still clean up even if something bails during the test.
	const bool result = RunLexerTest(logger, assertFile, assertLine, script, validationFunction);

	DisposeFile(script);

	return result;
}


static bool RunLexerTest(
	Bond::TextWriter &logger,
	const char *assertFile,
	int assertLine,
	const FileData &script,
	LexerValidationFunction *validationFunction)
{
	bool result = true;

	Bond::DefaultAllocator allocator;
	{
		Bond::Lexer lexer(allocator);
		lexer.Lex(script.data, script.length);
		result = validationFunction(logger, lexer);
	}

	__ASSERT_FORMAT__(allocator.GetNumAllocations() == 0, logger, assertFile, assertLine,
		("Lexer leaked %d chunks of memory.", allocator.GetNumAllocations()));

	return result;
}

}
