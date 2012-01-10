#include "framework/testsemanticanalyzerframework.h"
#include "bond/defaultallocator.h"
#include "bond/defaultfileloader.h"
#include "bond/lexer.h"
#include "bond/parseerror.h"
#include "bond/parser.h"
#include "bond/textwriter.h"

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
		Bond::Lexer lexer(lexerAllocator);
		lexer.Lex(script.mData, script.mLength);
		Bond::TokenStream stream = lexer.GetTokenStream();
		Bond::ParseErrorBuffer errorBuffer;
		Bond::Parser parser(parserAllocator, errorBuffer);
		parser.Parse(stream);
		//__ASSERT_FORMAT__(!parser.HasErrors(), logger, assertFile, assertLine,
		//	("Cannot run semantic analysis, since parser returned errors.\n"));
		Bond::SemanticAnalyzer analyzer(errorBuffer);
		if (!errorBuffer.HasErrors())
		{
			analyzer.Analyze(parser.GetTranslationUnitList());
		}
		result = validationFunction(logger, errorBuffer, parser, analyzer);
	}

	__ASSERT_FORMAT__(lexerAllocator.GetNumAllocations() == 0, logger, assertFile, assertLine,
		("Lexer leaked %d chunks of memory.\n", parserAllocator.GetNumAllocations()));
	__ASSERT_FORMAT__(parserAllocator.GetNumAllocations() == 0, logger, assertFile, assertLine,
		("Parser leaked %d chunks of memory.\n", parserAllocator.GetNumAllocations()));

	return result;
}

}
