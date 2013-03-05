#include "framework/testvmframework.h"
#include "bond/compiler/compilererror.h"
#include "bond/compiler/codegenerator.h"
#include "bond/compiler/lexer.h"
#include "bond/compiler/parser.h"
#include "bond/compiler/semanticanalyzer.h"
#include "bond/io/bufferedbinarywriter.h"
#include "bond/io/defaultfileloader.h"
#include "bond/systems/defaultallocator.h"
#include "bond/systems/exception.h"
#include "bond/vm/cboloader.h"
#include "bond/vm/codesegment.h"
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
	bool result = false;

	try
	{
		Bond::DefaultFileLoader fileLoader(fileLoaderAllocator);
		Bond::FileLoader::Handle scriptHandle = fileLoader.LoadFile(scriptName);
		Bond::CompilerErrorBuffer errorBuffer;
		Bond::Lexer lexer(lexerAllocator, errorBuffer);
		lexer.Lex(reinterpret_cast<const char *>(scriptHandle.Get().mData), scriptHandle.Get().mLength);

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
				Bond::FileData cboFile(cboBuffer, size_t(cboWriter.GetPosition()));
				Bond::CboLoader cboLoader(cboLoaderAllocator);
				Bond::CboLoader::Handle codeSegmentHandle = cboLoader.Load(&cboFile, 1);
				Bond::VM vm(vmAllocator, *codeSegmentHandle.Get(), 96 * 1024);
				result = validationFunction(logger, vm);
			}
		}

		if (errorBuffer.HasErrors())
		{
			logger.Write("line %u in %s: Failed to compile '%s'.", assertLine, assertFile, scriptName);
			result = false;
		}
	}
	catch (const Bond::Exception &e)
	{
		logger.Write("line %u in %s: %s\n", assertLine, assertFile, e.GetMessage());
	}

	__ASSERT_FORMAT__(fileLoaderAllocator.GetNumAllocations() == 0, logger, assertFile, assertLine,
		("File loader leaked %d chunks of memory.", fileLoaderAllocator.GetNumAllocations()));
	__ASSERT_FORMAT__(lexerAllocator.GetNumAllocations() == 0, logger, assertFile, assertLine,
		("Lexer leaked %d chunks of memory.", lexerAllocator.GetNumAllocations()));
	__ASSERT_FORMAT__(parserAllocator.GetNumAllocations() == 0, logger, assertFile, assertLine,
		("Parser leaked %d chunks of memory.", parserAllocator.GetNumAllocations()));
	__ASSERT_FORMAT__(codeGeneratorAllocator.GetNumAllocations() == 0, logger, assertFile, assertLine,
		("Code generator leaked %d chunks of memory.", codeGeneratorAllocator.GetNumAllocations()));
	__ASSERT_FORMAT__(cboLoaderAllocator.GetNumAllocations() == 0, logger, assertFile, assertLine,
		("CBO loader leaked %d chunks of memory.", cboLoaderAllocator.GetNumAllocations()));
	__ASSERT_FORMAT__(vmAllocator.GetNumAllocations() == 0, logger, assertFile, assertLine,
		("VM leaked %d chunks of memory.", vmAllocator.GetNumAllocations()));

	return result;
}

}
