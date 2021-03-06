#include "framework/testvmframework.h"
#include "bond/api/libinclude.h"
#include "bond/api/libruntime.h"
#include "bond/compiler/compilererror.h"
#include "bond/compiler/codegenerator.h"
#include "bond/compiler/frontend.h"
#include "bond/compiler/lexer.h"
#include "bond/compiler/parser.h"
#include "bond/compiler/semanticanalyzer.h"
#include "bond/io/memoryoutputstream.h"
#include "bond/io/memorystreamfactory.h"
#include "bond/io/stdiostreamfactory.h"
#include "bond/systems/defaultallocator.h"
#include "bond/systems/exception.h"
#include "bond/vm/cboloader.h"
#include "bond/vm/codesegment.h"

namespace TestFramework
{

bool RunVMTest(
	Bond::OutputStream &logger,
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
	Bond::DefaultAllocator streamFactoryAllocator;
	Bond::DefaultAllocator frontEndAllocator;
	Bond::DefaultAllocator codeGeneratorAllocator;
	Bond::DefaultAllocator cboLoaderAllocator;
	Bond::DefaultAllocator vmAllocator;
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
		Bond::TranslationUnit *translationUnitList = frontEnd.Analyze();

		if (!errorBuffer.HasErrors())
		{
			const Bond::Stream::pos_t MAX_CBO_SIZE = 8 * 1024;
			uint8_t cboBuffer[MAX_CBO_SIZE];
			Bond::MemoryOutputStream cboStream(cboBuffer, MAX_CBO_SIZE);
			Bond::CodeGenerator generator(codeGeneratorAllocator, errorBuffer);
			generator.Generate(translationUnitList, cboStream);

			if (!errorBuffer.HasErrors())
			{
				Bond::CboLoader cboLoader(cboLoaderAllocator);
				Bond::LoadAllLibs(cboLoader);
				if (nativeBinding != nullptr)
				{
					cboLoader.AddNativeBinding(*nativeBinding);
				}
				cboLoader.AddCboFile(cboBuffer, size_t(cboStream.GetPosition()));
				Bond::CodeSegmentHandle codeSegmentHandle = cboLoader.Load();
				Bond::StdioStreamFactory vmStreamFactory(vmAllocator);
				Bond::VM vm(vmAllocator, *codeSegmentHandle.get(), 96 * 1024, nullptr, nullptr, nullptr, &vmStreamFactory);
				codeSegmentHandle.get()->CallStaticInitializers(vm);
				result = validationFunction(logger, vm);
			}
		}

		if (errorBuffer.HasErrors())
		{
			logger.Print("line %u in %s: Failed to compile '%s'.", assertLine, assertFile, scriptName);
			result = false;
		}
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
	__ASSERT_FORMAT__(codeGeneratorAllocator.GetNumAllocations() == 0, logger, assertFile, assertLine,
		("Code generator leaked %d chunks of memory.", codeGeneratorAllocator.GetNumAllocations()));
	__ASSERT_FORMAT__(cboLoaderAllocator.GetNumAllocations() == 0, logger, assertFile, assertLine,
		("CBO loader leaked %d chunks of memory.", cboLoaderAllocator.GetNumAllocations()));
	__ASSERT_FORMAT__(vmAllocator.GetNumAllocations() == 0, logger, assertFile, assertLine,
		("VM leaked %d chunks of memory.", vmAllocator.GetNumAllocations()));

	return result;
}

}
