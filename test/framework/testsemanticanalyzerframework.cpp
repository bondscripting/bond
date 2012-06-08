#include "framework/testsemanticanalyzerframework.h"
#include "bond/compilererror.h"
#include "bond/defaultallocator.h"
#include "bond/defaultfileloader.h"
#include "bond/lexer.h"
#include "bond/parser.h"

namespace TestFramework
{

static bool RunSemanticAnalyzerTest(
	Bond::TextWriter &logger,
	const char *assertFile,
	int assertLine,
	const Bond::FileData &script,
	SemanticAnalyzerValidationFunction *validationFunction);


bool RunSemanticAnalyzerTest(
	Bond::TextWriter &logger,
	const char *assertFile,
	int assertLine,
	const char *scriptName,
	SemanticAnalyzerValidationFunction *validationFunction)
{
	__ASSERT_FORMAT__(scriptName != 0, logger, assertFile, assertLine, ("Script name is NULL."));
	__ASSERT_FORMAT__(validationFunction != 0, logger, assertFile, assertLine, ("Validation function is NULL."));

	Bond::DefaultAllocator allocator;
	Bond::DefaultFileLoader fileLoader(allocator);
	Bond::FileData script = fileLoader.LoadFile(scriptName);
	__ASSERT_FORMAT__(script.mLength >= 0, logger, assertFile, assertLine,
		("Failed to load file '%s'.", scriptName));

	// Delegate to another function so we can still clean up even if something bails during the test.
	const bool result = RunSemanticAnalyzerTest(logger, assertFile, assertLine, script, validationFunction);

	fileLoader.DisposeFile(script);

	return result;
}


static bool RunSemanticAnalyzerTest(
	Bond::TextWriter &logger,
	const char *assertFile,
	int assertLine,
	const Bond::FileData &script,
	SemanticAnalyzerValidationFunction *validationFunction)
{
	bool result = true;

	Bond::DefaultAllocator lexerAllocator;
	Bond::DefaultAllocator parserAllocator;
	{
		Bond::CompilerErrorBuffer errorBuffer;
		Bond::Lexer lexer(lexerAllocator, errorBuffer);
		lexer.Lex(script.mData, script.mLength);
		Bond::TokenStream stream = lexer.GetTokenCollectionList()->GetTokenStream();
		Bond::Parser parser(parserAllocator, errorBuffer);
		if (!errorBuffer.HasErrors())
		{
			parser.Parse(stream);
		}
		Bond::SemanticAnalyzer analyzer(errorBuffer);
		if (!errorBuffer.HasErrors())
		{
			analyzer.Analyze(parser.GetTranslationUnitList());
		}
		result = validationFunction(logger, errorBuffer, analyzer);
	}

	__ASSERT_FORMAT__(lexerAllocator.GetNumAllocations() == 0, logger, assertFile, assertLine,
		("Lexer leaked %d chunks of memory.\n", parserAllocator.GetNumAllocations()));
	__ASSERT_FORMAT__(parserAllocator.GetNumAllocations() == 0, logger, assertFile, assertLine,
		("Parser leaked %d chunks of memory.\n", parserAllocator.GetNumAllocations()));

	return result;
}

}
