#include "framework/testlexerframework.h"
#include "bond/defaultallocator.h"
#include "bond/defaultfileloader.h"
#include "bond/exception.h"

namespace TestFramework
{

bool RunLexerTest(
	Bond::TextWriter &logger,
	const char *assertFile,
	int assertLine,
	const char *scriptName,
	LexerValidationFunction *validationFunction)
{
	__ASSERT_FORMAT__(scriptName != 0, logger, assertFile, assertLine, ("Script name is NULL."));
	__ASSERT_FORMAT__(validationFunction != 0, logger, assertFile, assertLine, ("Validation function is NULL."));

	Bond::DefaultAllocator fileLoaderAllocator;
	Bond::DefaultAllocator lexerAllocator;
	Bond::DefaultFileLoader fileLoader(fileLoaderAllocator);
	Bond::FileData script;
	bool result = false;

	try
	{
		script = fileLoader.LoadFile(scriptName);
		Bond::CompilerErrorBuffer errorBuffer;
		Bond::Lexer lexer(lexerAllocator, errorBuffer);
		lexer.Lex(reinterpret_cast<const char *>(script.mData), script.mLength);
		result = validationFunction(logger, errorBuffer, lexer);
	}
	catch (const Bond::Exception &e)
	{
		logger.Write("line %u in %s: %s", assertLine, assertFile, e.GetMessage());
	}

	fileLoader.DisposeFile(script);

	__ASSERT_FORMAT__(lexerAllocator.GetNumAllocations() == 0, logger, assertFile, assertLine,
		("Lexer leaked %d chunks of memory.", lexerAllocator.GetNumAllocations()));

	return result;
}

}
