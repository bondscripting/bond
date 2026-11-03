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
	RunSemanticAnalyzerTest(assertFile, assertLine, &scriptName, 1, validationFunction);
}


void RunSemanticAnalyzerTest(
	const char *assertFile,
	int assertLine,
	const char *const *scriptNames,
	size_t numScripts,
	SemanticAnalyzerValidationFunction *validationFunction)
{
	Assert(validationFunction != nullptr, assertFile, assertLine, "Validation function is NULL.");
	for (size_t i = 0; i < numScripts; ++i)
	{
		Assert(scriptNames[i] != nullptr, assertFile, assertLine, "Script name is NULL.");
	}

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
		Bond::StdioStreamFactory streamFactory(streamFactoryAllocator, nullptr, nullptr, false);
		Bond::MemoryStreamFactory stdLibFactory(streamFactoryAllocator, Bond::INCLUDE_FILE_INDEX, &streamFactory, false);
		Bond::FrontEnd frontEnd(frontEndAllocator, tokenStore, lexer, parser, analyzer, stdLibFactory);

		for (size_t i = 0; i < numScripts; ++i)
		{
			frontEnd.AddInputFile(scriptNames[i]);
		}
		frontEnd.Analyze();
		validationFunction(errorBuffer, analyzer);
	}
	catch (const Bond::Exception &e)
	{
		throw AssertionFailure(assertFile, assertLine, "%s", e.GetMessage());
	}
}

}
