#include "framework/testvmframework.h"
#include "framework/asserts.h"
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

void RunVMTest(
	const char *assertFile,
	int assertLine,
	const char *scriptName,
	VMValidationFunction *validationFunction,
	const Bond::NativeBindingCollection *nativeBinding)
{
	Assert(validationFunction != nullptr, assertFile, assertLine, "Validation function is NULL.");

	Bond::ValidateConfiguration();

	uint8_t cboBuffer[MAX_CBO_SIZE];
	const size_t cboSize = CompileScriptToCbo(assertFile, assertLine, scriptName, cboBuffer, MAX_CBO_SIZE);

	Bond::DefaultAllocator cboLoaderAllocator;
	Bond::DefaultAllocator vmAllocator;
	AllocationLeakGuard<Bond::DefaultAllocator> cboLoaderLeakGuard(
		cboLoaderAllocator, "CBO loader", assertFile, assertLine);
	AllocationLeakGuard<Bond::DefaultAllocator> vmLeakGuard(vmAllocator, "VM", assertFile, assertLine);

	try
	{
		Bond::CboLoader cboLoader(cboLoaderAllocator);
		Bond::LoadAllLibs(cboLoader);
		if (nativeBinding != nullptr)
		{
			cboLoader.AddNativeBinding(*nativeBinding);
		}
		cboLoader.AddCboFile(cboBuffer, cboSize);
		Bond::CodeSegmentHandle codeSegmentHandle = cboLoader.Load();
		Bond::StdioStreamFactory vmStreamFactory(vmAllocator);
		Bond::VM vm(vmAllocator, *codeSegmentHandle.get(), 96 * 1024, nullptr, nullptr, nullptr, &vmStreamFactory);
		codeSegmentHandle.get()->CallStaticInitializers(vm);
		validationFunction(vm);
	}
	catch (const Bond::Exception &e)
	{
		throw AssertionFailure(assertFile, assertLine, "%s", e.GetMessage());
	}
}


size_t CompileScriptToCbo(
	const char *assertFile,
	int assertLine,
	const char *scriptName,
	uint8_t *cboBuffer,
	size_t bufferSize)
{
	Assert(scriptName != nullptr, assertFile, assertLine, "Script name is NULL.");

	Bond::DefaultAllocator lexerAllocator;
	Bond::DefaultAllocator parserAllocator;
	Bond::DefaultAllocator streamFactoryAllocator;
	Bond::DefaultAllocator frontEndAllocator;
	Bond::DefaultAllocator codeGeneratorAllocator;
	AllocationLeakGuard<Bond::DefaultAllocator> lexerLeakGuard(lexerAllocator, "Lexer", assertFile, assertLine);
	AllocationLeakGuard<Bond::DefaultAllocator> parserLeakGuard(parserAllocator, "Parser", assertFile, assertLine);
	AllocationLeakGuard<Bond::DefaultAllocator> streamFactoryLeakGuard(
		streamFactoryAllocator, "Stream factory", assertFile, assertLine);
	AllocationLeakGuard<Bond::DefaultAllocator> frontEndLeakGuard(
		frontEndAllocator, "Front end", assertFile, assertLine);
	AllocationLeakGuard<Bond::DefaultAllocator> codeGeneratorLeakGuard(
		codeGeneratorAllocator, "Code generator", assertFile, assertLine);

	size_t cboSize = 0;

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
			Bond::MemoryOutputStream cboStream(cboBuffer, Bond::Stream::pos_t(bufferSize));
			Bond::CodeGenerator generator(codeGeneratorAllocator, errorBuffer);
			generator.Generate(translationUnitList, cboStream);
			cboSize = size_t(cboStream.GetPosition());
			Assert(cboSize < bufferSize, assertFile, assertLine,
				"CBO for '%s' exceeds the %d byte test buffer.", scriptName, int(bufferSize));
		}

		if (errorBuffer.HasErrors())
		{
			throw AssertionFailure(assertFile, assertLine, "Failed to compile '%s'.", scriptName);
		}
	}
	catch (const Bond::Exception &e)
	{
		throw AssertionFailure(assertFile, assertLine, "%s", e.GetMessage());
	}

	return cboSize;
}

}
