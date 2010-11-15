#include "framework/testsemanticanalyzerframework.h"
#include "framework/util.h"
#include "bond/defaultallocator.h"
#include "bond/lexer.h"
#include "bond/parser.h"
#include "bond/textwriter.h"

namespace TestFramework
{
static bool RunSemanticAnalyzerTest(
	Bond::TextWriter &logger,
	const char *assertFile,
	int assertLine,
	const FileData &script,
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

	FileData script = ReadFile(scriptName);
	__ASSERT_FORMAT__(script.length >= 0, logger, assertFile, assertLine,
		("Failed to load file '%s'.", scriptName));

	// Delegate to another function so we can still clean up even if something bails during the test.
	const bool result = RunSemanticAnalyzerTest(logger, assertFile, assertLine, script, validationFunction);

	DisposeFile(script);

	return result;
}


static bool RunSemanticAnalyzerTest(
	Bond::TextWriter &logger,
	const char *assertFile,
	int assertLine,
	const FileData &script,
	SemanticAnalyzerValidationFunction *validationFunction)
{
	bool result = true;

	Bond::DefaultAllocator lexerAllocator;
	Bond::DefaultAllocator parserAllocator;
	Bond::DefaultAllocator analyzerAllocator;
	{
		Bond::Lexer lexer(lexerAllocator);
		lexer.Lex(script.data, script.length);
		Bond::TokenStream stream = lexer.GetTokenStream();
		Bond::Parser parser(parserAllocator);
		parser.Parse(stream);
		Bond::SemanticAnalyzer analyzer(analyzerAllocator);
		analyzer.Analyze(parser.GetTranslationUnitList());
		result = validationFunction(logger, analyzer);
	}

	__ASSERT_FORMAT__(lexerAllocator.GetNumAllocations() == 0, logger, assertFile, assertLine,
		("Lexer leaked %d chunks of memory.", parserAllocator.GetNumAllocations()));
	__ASSERT_FORMAT__(parserAllocator.GetNumAllocations() == 0, logger, assertFile, assertLine,
		("Parser leaked %d chunks of memory.", parserAllocator.GetNumAllocations()));
	__ASSERT_FORMAT__(analyzerAllocator.GetNumAllocations() == 0, logger, assertFile, assertLine,
		("Semantic Analyzer leaked %d chunks of memory.", analyzerAllocator.GetNumAllocations()));

	return result;
}

}
