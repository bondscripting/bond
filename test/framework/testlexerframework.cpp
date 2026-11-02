#include "framework/testlexerframework.h"
#include "framework/asserts.h"
#include "bond/io/memoryinputstream.h"
#include "bond/io/stdioinputstream.h"
#include "bond/systems/defaultallocator.h"
#include "bond/systems/exception.h"

namespace TestFramework
{

static void LexStream(
	const char *assertFile,
	int assertLine,
	const char *scriptName,
	Bond::InputStream &scriptStream,
	LexerValidationFunction *validationFunction)
{
	Bond::DefaultAllocator lexerAllocator;
	AllocationLeakGuard<Bond::DefaultAllocator> lexerLeakGuard(lexerAllocator, "Lexer", assertFile, assertLine);

	Bond::CompilerErrorBuffer errorBuffer;
	Bond::Lexer lexer(lexerAllocator, errorBuffer);
	auto tokenCollectionHandle = lexer.Lex(scriptName, scriptStream);
	validationFunction(errorBuffer, *tokenCollectionHandle);
}


void RunLexerTest(
	const char *assertFile,
	int assertLine,
	const char *scriptName,
	LexerValidationFunction *validationFunction)
{
	Assert(scriptName != nullptr, assertFile, assertLine, "Script name is NULL.");
	Assert(validationFunction != nullptr, assertFile, assertLine, "Validation function is NULL.");

	try
	{
		Bond::StdioInputStream scriptStream(scriptName);
		LexStream(assertFile, assertLine, scriptName, scriptStream, validationFunction);
	}
	catch (const Bond::Exception &e)
	{
		throw AssertionFailure(assertFile, assertLine, "%s", e.GetMessage());
	}
}


void RunLexerTest(
	const char *assertFile,
	int assertLine,
	const char *scriptName,
	const char *buffer,
	size_t length,
	LexerValidationFunction *validationFunction)
{
	Assert(scriptName != nullptr, assertFile, assertLine, "Script name is NULL.");
	Assert(buffer != nullptr, assertFile, assertLine, "Buffer is NULL.");
	Assert(validationFunction != nullptr, assertFile, assertLine, "Validation function is NULL.");

	try
	{
		Bond::MemoryInputStream scriptStream(buffer, Bond::Stream::pos_t(length));
		LexStream(assertFile, assertLine, scriptName, scriptStream, validationFunction);
	}
	catch (const Bond::Exception &e)
	{
		throw AssertionFailure(assertFile, assertLine, "%s", e.GetMessage());
	}
}

}
