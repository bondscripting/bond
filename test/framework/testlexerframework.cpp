#include "framework/testlexerframework.h"
#include "bond/io/stdioinputstream.h"
#include "bond/systems/defaultallocator.h"
#include "bond/systems/exception.h"

namespace TestFramework
{

bool RunLexerTest(
	Bond::OutputStream &logger,
	const char *assertFile,
	int assertLine,
	const char *scriptName,
	LexerValidationFunction *validationFunction)
{
	__ASSERT_FORMAT__(scriptName != nullptr, logger, assertFile, assertLine, ("Script name is NULL."));
	__ASSERT_FORMAT__(validationFunction != nullptr, logger, assertFile, assertLine, ("Validation function is NULL."));

	Bond::DefaultAllocator fileLoaderAllocator;
	Bond::DefaultAllocator lexerAllocator;
	bool result = false;

	try
	{
		Bond::StdioInputStream scriptStream(scriptName);
		Bond::CompilerErrorBuffer errorBuffer;
		Bond::Lexer lexer(lexerAllocator, errorBuffer);
		auto tokenCollectionHandle = lexer.Lex(scriptName, scriptStream);
		result = validationFunction(logger, errorBuffer, *tokenCollectionHandle);
	}
	catch (const Bond::Exception &e)
	{
		logger.Print("line %u in %s: %s\n", assertLine, assertFile, e.GetMessage());
	}

	__ASSERT_FORMAT__(fileLoaderAllocator.GetNumAllocations() == 0, logger, assertFile, assertLine,
		("File loader leaked %d chunks of memory.", fileLoaderAllocator.GetNumAllocations()));
	__ASSERT_FORMAT__(lexerAllocator.GetNumAllocations() == 0, logger, assertFile, assertLine,
		("Lexer leaked %d chunks of memory.", lexerAllocator.GetNumAllocations()));

	return result;
}

}
