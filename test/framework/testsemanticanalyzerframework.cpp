#include "framework/testsemanticanalyzerframework.h"
#include "framework/asserts.h"
#include "bond/api/libinclude.h"
#include "bond/compiler/compilererror.h"
#include "bond/compiler/frontend.h"
#include "bond/compiler/lexer.h"
#include "bond/compiler/parser.h"
#include "bond/io/memorystreamfactory.h"
#include "bond/io/stdiostreamfactory.h"
#include "bond/systems/defaultallocator.h"
#include "bond/systems/exception.h"

namespace TestFramework
{

void RunSemanticAnalyzerTest(
	const char *assertFile,
	int assertLine,
	const char *scriptName,
	SemanticAnalyzerValidationFunction *validationFunction)
{
	Assert(scriptName != nullptr, assertFile, assertLine, "Script name is NULL.");
	Assert(validationFunction != nullptr, assertFile, assertLine, "Validation function is NULL.");

	Bond::DefaultAllocator lexerAllocator;
	Bond::DefaultAllocator parserAllocator;
	Bond::DefaultAllocator streamFactoryAllocator;
	Bond::DefaultAllocator frontEndAllocator;
	AllocationLeakGuard<Bond::DefaultAllocator> lexerLeakGuard(lexerAllocator, "Lexer", assertFile, assertLine);
	AllocationLeakGuard<Bond::DefaultAllocator> parserLeakGuard(parserAllocator, "Parser", assertFile, assertLine);
	AllocationLeakGuard<Bond::DefaultAllocator> streamFactoryLeakGuard(
		streamFactoryAllocator, "Stream factory", assertFile, assertLine);
	AllocationLeakGuard<Bond::DefaultAllocator> frontEndLeakGuard(
		frontEndAllocator, "Front end", assertFile, assertLine);

	try
	{
		Bond::CompilerErrorBuffer errorBuffer;
		Bond::TokenCollectionStore tokenStore((Bond::TokenCollectionStore::allocator_type(&lexerAllocator)));
		Bond::ParseNodeStore parseNodeStore((Bond::ParseNodeStore::allocator_type(&parserAllocator)));
		Bond::Lexer lexer(lexerAllocator, errorBuffer);
		Bond::Parser parser(parserAllocator, errorBuffer, parseNodeStore);
		Bond::SemanticAnalyzer analyzer(errorBuffer);
		Bond::StdioStreamFactory streamFactory(streamFactoryAllocator);
		Bond::MemoryStreamFactory stdLibFactory(streamFactoryAllocator, Bond::INCLUDE_FILE_INDEX, &streamFactory);
		Bond::FrontEnd frontEnd(frontEndAllocator, tokenStore, lexer, parser, analyzer, stdLibFactory);

		frontEnd.AddInputFile(scriptName);
		frontEnd.Analyze();
		validationFunction(errorBuffer, analyzer);
	}
	catch (const Bond::Exception &e)
	{
		throw AssertionFailure(assertFile, assertLine, "%s", e.GetMessage());
	}
}

}
