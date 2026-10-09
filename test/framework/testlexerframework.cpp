#include "framework/testlexerframework.h"
#include "framework/asserts.h"
#include "bond/io/stdioinputstream.h"
#include "bond/systems/defaultallocator.h"
#include "bond/systems/exception.h"

namespace TestFramework
{

void RunLexerTest(
	const char *assertFile,
	int assertLine,
	const char *scriptName,
	LexerValidationFunction *validationFunction)
{
	Assert(scriptName != nullptr, assertFile, assertLine, "Script name is NULL.");
	Assert(validationFunction != nullptr, assertFile, assertLine, "Validation function is NULL.");

	Bond::DefaultAllocator fileLoaderAllocator;
	Bond::DefaultAllocator lexerAllocator;
	AllocationLeakGuard<Bond::DefaultAllocator> fileLoaderLeakGuard(
		fileLoaderAllocator, "File loader", assertFile, assertLine);
	AllocationLeakGuard<Bond::DefaultAllocator> lexerLeakGuard(lexerAllocator, "Lexer", assertFile, assertLine);

	try
	{
		Bond::StdioInputStream scriptStream(scriptName);
		Bond::CompilerErrorBuffer errorBuffer;
		Bond::Lexer lexer(lexerAllocator, errorBuffer);
		auto tokenCollectionHandle = lexer.Lex(scriptName, scriptStream);
		validationFunction(errorBuffer, *tokenCollectionHandle);
	}
	catch (const Bond::Exception &e)
	{
		throw AssertionFailure(assertFile, assertLine, "%s", e.GetMessage());
	}
}

}
