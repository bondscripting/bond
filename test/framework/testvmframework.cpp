#include "framework/testvmframework.h"
#include "bond/api/libinclude.h"
#include "bond/api/libruntime.h"
#include "bond/compiler/compilererror.h"
#include "bond/compiler/codegenerator.h"
#include "bond/compiler/frontend.h"
#include "bond/compiler/lexer.h"
#include "bond/compiler/parser.h"
#include "bond/compiler/semanticanalyzer.h"
#include "bond/io/bufferedbinarywriter.h"
#include "bond/io/diskfileloader.h"
#include "bond/systems/defaultallocator.h"
#include "bond/systems/exception.h"
#include "bond/vm/cboloader.h"
#include "bond/vm/codesegment.h"

namespace TestFramework
{

bool RunVMTest(
	Bond::TextWriter &logger,
	const char *assertFile,
	int assertLine,
	const char *scriptName,
	VMValidationFunction *validationFunction,
	const Bond::NativeBindingCollection *nativeBinding)
{
	__ASSERT_FORMAT__(scriptName != nullptr, logger, assertFile, assertLine, ("Script name is NULL."));
	__ASSERT_FORMAT__(validationFunction != nullptr, logger, assertFile, assertLine, ("Validation function is NULL."));

	Bond::ValidateConfiguration();

	Bond::DefaultAllocator lexerAllocator;
	Bond::DefaultAllocator parserAllocator;
	Bond::DefaultAllocator fileLoaderAllocator;
	Bond::DefaultAllocator frontEndAllocator;
	Bond::DefaultAllocator codeGeneratorAllocator;
	Bond::DefaultAllocator cboLoaderAllocator;
	Bond::DefaultAllocator vmAllocator;
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
				Bond::LoadAllLibs(cboLoader);
				if (nativeBinding != nullptr)
				{
					cboLoader.AddNativeBinding(*nativeBinding);
				}
				cboLoader.AddCboFile(cboFile);
				Bond::CboLoader::Handle codeSegmentHandle = cboLoader.Load();
				Bond::VM vm(vmAllocator, *codeSegmentHandle.get(), 96 * 1024);
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

	__ASSERT_FORMAT__(lexerAllocator.GetNumAllocations() == 0, logger, assertFile, assertLine,
		("Lexer leaked %d chunks of memory.", lexerAllocator.GetNumAllocations()));
	__ASSERT_FORMAT__(parserAllocator.GetNumAllocations() == 0, logger, assertFile, assertLine,
		("Parser leaked %d chunks of memory.", parserAllocator.GetNumAllocations()));
	__ASSERT_FORMAT__(fileLoaderAllocator.GetNumAllocations() == 0, logger, assertFile, assertLine,
		("File loader leaked %d chunks of memory.", fileLoaderAllocator.GetNumAllocations()));
	__ASSERT_FORMAT__(frontEndAllocator.GetNumAllocations() == 0, logger, assertFile, assertLine,
		("Front end leaked %d chunks of memory.", frontEndAllocator.GetNumAllocations()));
	__ASSERT_FORMAT__(codeGeneratorAllocator.GetNumAllocations() == 0, logger, assertFile, assertLine,
		("Code generator leaked %d chunks of memory.", codeGeneratorAllocator.GetNumAllocations()));
	__ASSERT_FORMAT__(cboLoaderAllocator.GetNumAllocations() == 0, logger, assertFile, assertLine,
		("CBO loader leaked %d chunks of memory.", cboLoaderAllocator.GetNumAllocations()));
	__ASSERT_FORMAT__(vmAllocator.GetNumAllocations() == 0, logger, assertFile, assertLine,
		("VM leaked %d chunks of memory.", vmAllocator.GetNumAllocations()));

	return result;
}

}
