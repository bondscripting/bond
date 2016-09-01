#include "framework/testsemanticanalyzerframework.h"
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

bool RunSemanticAnalyzerTest(
	Bond::OutputStream &logger,
	const char *assertFile,
	int assertLine,
	const char *scriptName,
	SemanticAnalyzerValidationFunction *validationFunction)
{
	__ASSERT_FORMAT__(scriptName != nullptr, logger, assertFile, assertLine, ("Script name is NULL."));
	__ASSERT_FORMAT__(validationFunction != nullptr, logger, assertFile, assertLine, ("Validation function is NULL."));

	Bond::DefaultAllocator lexerAllocator;
	Bond::DefaultAllocator parserAllocator;
	Bond::DefaultAllocator streamFactoryAllocator;
	Bond::DefaultAllocator frontEndAllocator;
	bool result = false;

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
		result = validationFunction(logger, errorBuffer, analyzer);
	}
	catch (const Bond::Exception &e)
	{
		logger.Print("line %u in %s: %s\n", assertLine, assertFile, e.GetMessage());
	}

	__ASSERT_FORMAT__(lexerAllocator.GetNumAllocations() == 0, logger, assertFile, assertLine,
		("Lexer leaked %d chunks of memory.", lexerAllocator.GetNumAllocations()));
	__ASSERT_FORMAT__(parserAllocator.GetNumAllocations() == 0, logger, assertFile, assertLine,
		("Parser leaked %d chunks of memory.", parserAllocator.GetNumAllocations()));
	__ASSERT_FORMAT__(streamFactoryAllocator.GetNumAllocations() == 0, logger, assertFile, assertLine,
		("Stream factory leaked %d chunks of memory.", streamFactoryAllocator.GetNumAllocations()));
	__ASSERT_FORMAT__(frontEndAllocator.GetNumAllocations() == 0, logger, assertFile, assertLine,
		("Front end leaked %d chunks of memory.", frontEndAllocator.GetNumAllocations()));

	return result;
}

}
