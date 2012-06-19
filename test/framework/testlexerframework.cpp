#include "framework/testlexerframework.h"
#include "bond/defaultallocator.h"
#include "bond/defaultfileloader.h"

namespace TestFramework
{

static bool RunLexerTest(
	Bond::TextWriter &logger,
	const char *assertFile,
	int assertLine,
	const Bond::FileData &script,
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

	Bond::DefaultAllocator allocator;
	Bond::DefaultFileLoader fileLoader(allocator);
	Bond::FileData script = fileLoader.LoadFile(scriptName);
	__ASSERT_FORMAT__(script.mValid, logger, assertFile, assertLine,
		("Failed to load file '%s'.", scriptName));

	// Delegate to another function so we can still clean up even if something bails during the test.
	const bool result = RunLexerTest(logger, assertFile, assertLine, script, validationFunction);

	fileLoader.DisposeFile(script);

	return result;
}


static bool RunLexerTest(
	Bond::TextWriter &logger,
	const char *assertFile,
	int assertLine,
	const Bond::FileData &script,
	LexerValidationFunction *validationFunction)
{
	bool result = true;

	Bond::DefaultAllocator allocator;
	{
		Bond::CompilerErrorBuffer errorBuffer;
		Bond::Lexer lexer(allocator, errorBuffer);
		lexer.Lex(reinterpret_cast<const char *>(script.mData), script.mLength);
		result = validationFunction(logger, errorBuffer, lexer);
	}

	__ASSERT_FORMAT__(allocator.GetNumAllocations() == 0, logger, assertFile, assertLine,
		("Lexer leaked %d chunks of memory.", allocator.GetNumAllocations()));

	return result;
}

}
