#include "framework/testvmframework.h"
#include "bond/bufferedbinarywriter.h"
#include "bond/cboloader.h"
#include "bond/codegenerator.h"
#include "bond/codesegment.h"
#include "bond/compilererror.h"
#include "bond/defaultallocator.h"
#include "bond/defaultfileloader.h"
#include "bond/lexer.h"
#include "bond/parser.h"
#include "bond/semanticanalyzer.h"
#include <stdio.h>

namespace TestFramework
{

bool RunVMTest(
	Bond::TextWriter &logger,
	const char *assertFile,
	int assertLine,
	const char *scriptName,
	VMValidationFunction *validationFunction)
{
	__ASSERT_FORMAT__(scriptName != 0, logger, assertFile, assertLine, ("Script name is NULL."));
	__ASSERT_FORMAT__(validationFunction != 0, logger, assertFile, assertLine, ("Validation function is NULL."));

	Bond::DefaultAllocator fileLoaderAllocator;
	Bond::DefaultAllocator lexerAllocator;
	Bond::DefaultAllocator parserAllocator;
	Bond::DefaultAllocator codeGeneratorAllocator;
	Bond::DefaultAllocator cboLoaderAllocator;
	Bond::DefaultAllocator vmAllocator;
	Bond::DefaultFileLoader fileLoader(fileLoaderAllocator);
	Bond::FileData script = fileLoader.LoadFile(scriptName);
	bool result = false;

	if (script.mValid)
	{
		Bond::CompilerErrorBuffer errorBuffer;
		Bond::Lexer lexer(lexerAllocator, errorBuffer);
		lexer.Lex(reinterpret_cast<const char *>(script.mData), script.mLength);
		fileLoader.DisposeFile(script);

		Bond::Parser parser(parserAllocator, errorBuffer);
		if (!errorBuffer.HasErrors())
		{
			Bond::TokenStream stream = lexer.GetTokenCollectionList()->GetTokenStream();
			parser.Parse(stream);
		}

		Bond::SemanticAnalyzer analyzer(errorBuffer);
		if (!errorBuffer.HasErrors())
		{
			analyzer.Analyze(parser.GetTranslationUnitList());
		}

		if (!errorBuffer.HasErrors())
		{
			const int MAX_CBO_SIZE = 8 * 1024;
			Bond::bu8_t cboBuffer[MAX_CBO_SIZE];
			Bond::BufferedBinaryWriter cboWriter(cboBuffer, MAX_CBO_SIZE);
			Bond::CodeGenerator generator(codeGeneratorAllocator, errorBuffer);
			generator.Generate(parser.GetTranslationUnitList(), cboWriter);

			if (!errorBuffer.HasErrors())
			{
				Bond::FileData cboFile(cboBuffer, size_t(cboWriter.GetPosition()), true);
				Bond::CboLoader cboLoader(cboLoaderAllocator);
				const Bond::CodeSegment *codeSegment = cboLoader.Load(&cboFile, 1);
				Bond::VM vm(vmAllocator, *codeSegment, 96 * 1024);
				result = validationFunction(logger, vm);
				cboLoader.Dispose(codeSegment);
			}
		}

		if (errorBuffer.HasErrors())
		{
			__ERROR_FORMAT__(logger, assertFile, assertLine, ("Failed to compile '%s'.", scriptName));
		}

		fileLoader.DisposeFile(script);
	}
	else
	{
		__ERROR_FORMAT__(logger, assertFile, assertLine, ("Failed to load file '%s'.", scriptName));
	}

	__ASSERT_FORMAT__(fileLoaderAllocator.GetNumAllocations() == 0, logger, assertFile, assertLine,
		("File loader leaked %d chunks of memory.\n", fileLoaderAllocator.GetNumAllocations()));
	__ASSERT_FORMAT__(lexerAllocator.GetNumAllocations() == 0, logger, assertFile, assertLine,
		("Lexer leaked %d chunks of memory.\n", lexerAllocator.GetNumAllocations()));
	__ASSERT_FORMAT__(parserAllocator.GetNumAllocations() == 0, logger, assertFile, assertLine,
		("Parser leaked %d chunks of memory.\n", parserAllocator.GetNumAllocations()));
	__ASSERT_FORMAT__(codeGeneratorAllocator.GetNumAllocations() == 0, logger, assertFile, assertLine,
		("Code generator leaked %d chunks of memory.\n", codeGeneratorAllocator.GetNumAllocations()));
	__ASSERT_FORMAT__(cboLoaderAllocator.GetNumAllocations() == 0, logger, assertFile, assertLine,
		("CBO loader leaked %d chunks of memory.\n", cboLoaderAllocator.GetNumAllocations()));
	__ASSERT_FORMAT__(vmAllocator.GetNumAllocations() == 0, logger, assertFile, assertLine,
		("VM leaked %d chunks of memory.\n", vmAllocator.GetNumAllocations()));

	return result;
}

}