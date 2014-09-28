#include "framework/testsemanticanalyzerframework.h"
#include "bond/api/libinclude.h"
#include "bond/compiler/compilererror.h"
#include "bond/compiler/frontend.h"
#include "bond/compiler/lexer.h"
#include "bond/io/diskfileloader.h"
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
	Bond::DefaultAllocator fileLoaderAllocator;
	Bond::DefaultAllocator frontEndAllocator;
	bool result = false;

	try
	{
		Bond::CompilerErrorBuffer errorBuffer;
		Bond::Lexer lexer(lexerAllocator, errorBuffer);
		Bond::Parser parser(parserAllocator, errorBuffer);
		Bond::SemanticAnalyzer analyzer(errorBuffer);
		Bond::DiskFileLoader fileLoader(fileLoaderAllocator);
		Bond::MemoryFileLoader stdLibLoader(Bond::INCLUDE_FILE_INDEX, &fileLoader);
		Bond::FrontEnd frontEnd(frontEndAllocator, lexer, parser, analyzer, stdLibLoader);

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
	__ASSERT_FORMAT__(fileLoaderAllocator.GetNumAllocations() == 0, logger, assertFile, assertLine,
		("File loader leaked %d chunks of memory.", fileLoaderAllocator.GetNumAllocations()));
	__ASSERT_FORMAT__(frontEndAllocator.GetNumAllocations() == 0, logger, assertFile, assertLine,
		("Front end leaked %d chunks of memory.", frontEndAllocator.GetNumAllocations()));

	return result;
}

}
