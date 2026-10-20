// Cross-module checks. They pass for static and shared builds of the runtime library; the
// shared-only checks are compiled in when BOND_SHARED is defined.

#include "framework/testframework.h"
#include "bond/api/libinclude.h"
#include "bond/api/libmath.h"
#include "bond/api/libmemory.h"
#include "bond/api/nativebinding.h"
#include "bond/compiler/codegenerator.h"
#include "bond/compiler/compilererror.h"
#include "bond/compiler/frontend.h"
#include "bond/compiler/lexer.h"
#include "bond/compiler/parser.h"
#include "bond/compiler/parsenodes.h"
#include "bond/compiler/parsenodetraverser.h"
#include "bond/compiler/semanticanalyzer.h"
#include "bond/compiler/token.h"
#include "bond/io/memoryinputstream.h"
#include "bond/io/memorystreamfactory.h"
#include "bond/io/outputstream.h"
#include "bond/io/streamfactory.h"
#include "bond/systems/defaultallocator.h"
#include "bond/systems/exception.h"
#include "bond/vm/cboloader.h"
#include "bond/vm/codesegment.h"
#include "bond/vm/vm.h"
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

// Included last: windows.h defines macros (ERROR, GetMessage) that break Bond headers.
#if defined(BOND_SHARED)
#if defined(_WIN32)
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef GetMessage
#else
#include <dlfcn.h>
#endif
#endif
namespace
{

const char TEST_SOURCE_NAME[] = "sharedlib_test.bond";

const char TEST_SOURCE[] =
	"native\n"
	"{\n"
	"	int ExeNativeAdd(int a, int b);\n"
	"	int ExeNativeThrow(int x);\n"
	"}\n"
	"\n"
	"int AddViaNative(int a, int b)\n"
	"{\n"
	"	return ExeNativeAdd(a, b);\n"
	"}\n"
	"\n"
	"int ThrowViaNative(int x)\n"
	"{\n"
	"	return ExeNativeThrow(x);\n"
	"}\n";


// An Allocator implemented in the executable and driven by the library through its vtable.
class CountingAllocator: public Bond::Allocator
{
public:
	virtual void *Allocate(size_t size) override { ++mNumCalls; return mInner.Allocate(size); }
	virtual void *AllocateAligned(size_t size, size_t align) override { ++mNumCalls; return mInner.AllocateAligned(size, align); }
	virtual void Free(void *buffer) override { mInner.Free(buffer); }
	virtual void FreeAligned(void *buffer) override { mInner.FreeAligned(buffer); }

	int GetNumAllocations() const { return mInner.GetNumAllocations(); }
	int GetNumCalls() const { return mNumCalls; }

private:
	Bond::DefaultAllocator mInner;
	int mNumCalls = 0;
};


// An OutputStream implemented in the executable that supports the seeking CodeGenerator needs.
class VectorOutputStream: public Bond::OutputStream
{
public:
	using Bond::OutputStream::Write;

	virtual pos_t GetPosition() const override { return pos_t(mPosition); }
	virtual pos_t GetEndPosition() const override { return pos_t(mBytes.size()); }
	virtual void SetPosition(off_t offset) override { mPosition = size_t(offset); }
	virtual void SetPositionFromEnd(off_t offset) override { mPosition = mBytes.size() - size_t(offset); }

	virtual void AddOffset(off_t offset) override
	{
		mPosition = size_t(off_t(mPosition) + offset);
		if (mPosition > mBytes.size())
		{
			mBytes.resize(mPosition);
		}
	}

	virtual bool IsEof() const override { return mPosition >= mBytes.size(); }
	virtual bool HasError() const override { return false; }
	virtual void ClearError() override {}

	virtual void VPrint(const char *format, va_list argList) override
	{
		char buffer[512];
		const int length = vsnprintf(buffer, sizeof(buffer), format, argList);
		if (length > 0)
		{
			Write(reinterpret_cast<const uint8_t *>(buffer), size_t(length) < sizeof(buffer) ? size_t(length) : sizeof(buffer) - 1);
		}
	}

	virtual void Write(uint8_t byte) override
	{
		if (mPosition >= mBytes.size())
		{
			mBytes.resize(mPosition + 1);
		}
		mBytes[mPosition++] = byte;
	}

	const uint8_t *GetData() const { return mBytes.data(); }
	size_t GetSize() const { return mBytes.size(); }
	std::string GetText() const { return std::string(mBytes.begin(), mBytes.end()); }

private:
	std::vector<uint8_t> mBytes;
	size_t mPosition = 0;
};


// A StreamFactory implemented in the executable that serves TEST_SOURCE from memory.
class SourceStreamFactory: public Bond::StreamFactory
{
public:
	explicit SourceStreamFactory(Bond::Allocator &allocator): mAllocator(allocator) {}

	virtual Bond::InputStreamHandle CreateInputStream(const char *fileName) override
	{
		Bond::InputStreamHandle handle;
		if (strcmp(fileName, TEST_SOURCE_NAME) == 0)
		{
			handle = mAllocator.AllocOwnedObject<Bond::MemoryInputStream>(TEST_SOURCE, Bond::Stream::pos_t(sizeof(TEST_SOURCE) - 1));
		}
		return handle;
	}

	virtual Bond::OutputStreamHandle CreateOutputStream(const char *, bool) override
	{
		return Bond::OutputStreamHandle();
	}

private:
	Bond::Allocator &mAllocator;
};


// A parse tree visitor implemented in the executable and driven by the library's Accept methods.
class FunctionCounter: public Bond::ParseNodeTraverser
{
public:
	using Bond::ParseNodeTraverser::Visit;

	virtual void Visit(Bond::FunctionDefinition *functionDefinition) override
	{
		Count(functionDefinition);
		Bond::ParseNodeTraverser::Visit(functionDefinition);
	}

	virtual void Visit(const Bond::FunctionDefinition *functionDefinition) override
	{
		Count(functionDefinition);
		Bond::ParseNodeTraverser::Visit(functionDefinition);
	}

	int GetNumDefinitions() const { return mNumDefinitions; }
	bool SawAddViaNative() const { return mSawAddViaNative; }

private:
	void Count(const Bond::FunctionDefinition *functionDefinition)
	{
		if (functionDefinition->GetBody() != nullptr)
		{
			++mNumDefinitions;
			mSawAddViaNative = mSawAddViaNative || (functionDefinition->GetName()->GetText() == "AddViaNative");
		}
	}

	int mNumDefinitions = 0;
	bool mSawAddViaNative = false;
};


VectorOutputStream *gCallStackDump = nullptr;


void ExeNativeAdd(Bond::StackFrame &frame)
{
	if (gCallStackDump != nullptr)
	{
		frame.GetVM().DumpCallStack(*gCallStackDump);
	}
	const int32_t a = frame.GetArg<int32_t>(0);
	const int32_t b = frame.GetArg<int32_t>(1);
	frame.SetReturnValue(int32_t(a + b));
}


void ExeNativeThrow(Bond::StackFrame &frame)
{
	throw Bond::Exception("exe-thrown %d", int(frame.GetArg<int32_t>(0)));
}


const Bond::NativeFunctionBinding EXE_BINDING_FUNCTIONS[] =
{
	{"ExeNativeAdd", ExeNativeAdd},
	{"ExeNativeThrow", ExeNativeThrow},
	{nullptr, nullptr}
};

const Bond::NativeBindingCollection EXE_BINDING_COLLECTION = {EXE_BINDING_FUNCTIONS, 2};


struct PipelineResult
{
	int mNumFunctionDefinitions = 0;
	bool mSawAddViaNative = false;
	size_t mCboSize = 0;
};


// Compiles and loads TEST_SOURCE using only the given allocator, then calls validate(vm). All
// pipeline objects are destroyed before returning (or unwinding), so the caller can check for leaks.
template <typename Func>
PipelineResult CompileAndRun(Bond::Allocator &allocator, Func validate)
{
	PipelineResult result;

	Bond::CompilerErrorBuffer errorBuffer;
	Bond::TokenCollectionStore tokenStore((Bond::TokenCollectionStore::allocator_type(&allocator)));
	Bond::ParseNodeStore parseNodeStore((Bond::ParseNodeStore::allocator_type(&allocator)));
	Bond::Lexer lexer(allocator, errorBuffer);
	Bond::Parser parser(allocator, errorBuffer, parseNodeStore);
	Bond::SemanticAnalyzer analyzer(errorBuffer);
	SourceStreamFactory streamFactory(allocator);
	Bond::FrontEnd frontEnd(allocator, tokenStore, lexer, parser, analyzer, streamFactory);

	frontEnd.AddInputFile(TEST_SOURCE_NAME);
	Bond::TranslationUnit *translationUnitList = frontEnd.Analyze();
	ASSERT_MESSAGE(!errorBuffer.HasErrors(), "Failed to analyze the test source.");

	FunctionCounter counter;
	counter.TraverseList(translationUnitList);
	result.mNumFunctionDefinitions = counter.GetNumDefinitions();
	result.mSawAddViaNative = counter.SawAddViaNative();

	VectorOutputStream cboStream;
	Bond::CodeGenerator generator(allocator, errorBuffer);
	generator.Generate(translationUnitList, cboStream);
	ASSERT_MESSAGE(!errorBuffer.HasErrors(), "Failed to generate code for the test source.");
	result.mCboSize = cboStream.GetSize();

	Bond::CboLoader cboLoader(allocator);
	cboLoader.AddNativeBinding(EXE_BINDING_COLLECTION);
	cboLoader.AddCboFile(cboStream.GetData(), cboStream.GetSize());
	Bond::CodeSegmentHandle codeSegmentHandle = cboLoader.Load();

	Bond::VM vm(allocator, *codeSegmentHandle.get(), 64 * 1024);
	codeSegmentHandle.get()->CallStaticInitializers(vm);
	validate(vm);

	return result;
}


#if defined(BOND_SHARED)

// Returns an identifier of the module (executable, DLL or shared object) containing address.
const void *GetContainingModule(const void *address)
{
#if defined(_WIN32)
	HMODULE module = nullptr;
	GetModuleHandleExA(
		GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
		static_cast<LPCSTR>(address),
		&module);
	return module;
#else
	Dl_info info;
	return (dladdr(address, &info) != 0) ? info.dli_fbase : nullptr;
#endif
}


void ExecutableAnchor() {}

#endif

}


DEFINE_TEST(ModuleIdentity)
{
	Bond::ValidateConfiguration();

#if defined(BOND_SHARED)
	const void *libraryModule = GetContainingModule(reinterpret_cast<const void *>(&Bond::ValidateConfiguration));
	const void *exeModule = GetContainingModule(reinterpret_cast<const void *>(&ExecutableAnchor));
	ASSERT_MESSAGE(libraryModule != nullptr, "Could not determine the module of the library.");
	ASSERT_MESSAGE(exeModule != nullptr, "Could not determine the module of the executable.");
	ASSERT_MESSAGE(libraryModule != exeModule, "The runtime library was linked statically into the executable.");
#endif
}


DEFINE_TEST(ExportedData)
{
	ASSERT_MESSAGE(Bond::VOID_TOKEN.GetTokenType() == Bond::Token::KEY_VOID, "VOID_TOKEN has the wrong type.");
	ASSERT_MESSAGE(Bond::INT_TOKEN.GetTokenType() == Bond::Token::KEY_INT, "INT_TOKEN has the wrong type.");
	ASSERT_MESSAGE(Bond::PRIMITIVE_TYPE_SPECIFIERS_TYPESET.Contains(Bond::Token::KEY_INT), "KEY_INT is not a primitive type specifier.");
	ASSERT_MESSAGE(!Bond::PRIMITIVE_TYPE_SPECIFIERS_TYPESET.Contains(Bond::Token::KEY_STRUCT), "KEY_STRUCT is a primitive type specifier.");
	ASSERT_MESSAGE(Bond::Token::GetTokenName(Bond::Token::KEY_VOID) != nullptr, "KEY_VOID has no name.");

	ASSERT_MESSAGE(Bond::INCLUDE_FILE_INDEX.GetNumDataViews() > 0, "INCLUDE_FILE_INDEX is empty.");
	ASSERT_MESSAGE(Bond::INCLUDE_FILE_INDEX.GetDataViewNames()[0] == "io.bond", "INCLUDE_FILE_INDEX has the wrong first name.");
	ASSERT_MESSAGE(Bond::INCLUDE_FILE_INDEX.GetDataViews()[0].GetLength() > 0, "INCLUDE_FILE_INDEX has an empty first file.");

	ASSERT_MESSAGE(Bond::MATH_BINDING_COLLECTION.mFunctionBindingCount > 0, "MATH_BINDING_COLLECTION is empty.");
	ASSERT_MESSAGE(Bond::MATH_BINDING_COLLECTION.mFunctionBindings[0].mFunction != nullptr, "MATH_BINDING_COLLECTION has a null function.");
	ASSERT_MESSAGE(Bond::MEMORY_BINDING_COLLECTION.mFunctionBindingCount > 0, "MEMORY_BINDING_COLLECTION is empty.");
}


DEFINE_TEST(ExecutableImplementedInterfaces)
{
	CountingAllocator allocator;
	VectorOutputStream callStackDump;
	gCallStackDump = &callStackDump;

	int32_t sum = 0;
	PipelineResult result;
	try
	{
		result = CompileAndRun(allocator, [&sum](Bond::VM &vm)
		{
			vm.CallFunction("AddViaNative", &sum, int32_t(20), int32_t(22));
		});
	}
	catch (...)
	{
		gCallStackDump = nullptr;
		throw;
	}
	gCallStackDump = nullptr;

	ASSERT_FORMAT(sum == 42, "Expected 42, but was %d.", int(sum));
	ASSERT_FORMAT(result.mNumFunctionDefinitions == 2, "Expected 2 function definitions, but saw %d.", result.mNumFunctionDefinitions);
	ASSERT_MESSAGE(result.mSawAddViaNative, "The executable's visitor did not see AddViaNative.");
	ASSERT_MESSAGE(result.mCboSize > 0, "The executable's output stream received no bytecode.");
	ASSERT_MESSAGE(callStackDump.GetText().find("AddViaNative") != std::string::npos, "The call stack dump does not mention AddViaNative.");
	ASSERT_MESSAGE(allocator.GetNumCalls() > 0, "The library never used the executable's allocator.");
	ASSERT_FORMAT(allocator.GetNumAllocations() == 0, "Leaked %d allocations.", allocator.GetNumAllocations());
}


DEFINE_TEST(ExceptionThrownByExecutableCrossesLibrary)
{
	CountingAllocator allocator;
	bool caught = false;

	try
	{
		CompileAndRun(allocator, [](Bond::VM &vm)
		{
			int32_t result = 0;
			vm.CallFunction("ThrowViaNative", &result, int32_t(7));
		});
	}
	catch (const Bond::Exception &e)
	{
		caught = true;
		ASSERT_FORMAT(strcmp(e.GetMessage(), "exe-thrown 7") == 0, "Unexpected message '%s'.", e.GetMessage());
	}

	ASSERT_MESSAGE(caught, "The exception thrown through the library was not caught as Bond::Exception.");
	ASSERT_FORMAT(allocator.GetNumAllocations() == 0, "Leaked %d allocations while unwinding.", allocator.GetNumAllocations());
}


DEFINE_TEST(ExceptionThrownByLibraryCaughtByExecutable)
{
	CountingAllocator allocator;
	const Bond::DataViewIndex emptyIndex(nullptr, nullptr, 0);
	Bond::MemoryStreamFactory factory(allocator, emptyIndex);
	bool caught = false;

	try
	{
		Bond::InputStreamHandle handle = factory.CreateInputStream("missing.bond");
	}
	catch (const Bond::Exception &e)
	{
		caught = true;
		ASSERT_MESSAGE(strstr(e.GetMessage(), "missing.bond") != nullptr, "The message does not mention the missing file.");
	}

	ASSERT_MESSAGE(caught, "The library's exception was not caught as Bond::Exception.");
	ASSERT_FORMAT(allocator.GetNumAllocations() == 0, "Leaked %d allocations.", allocator.GetNumAllocations());
}


DEFINE_TEST(AllocationsCrossModules)
{
	CountingAllocator allocator;

	{
		// Constructed and destroyed in the executable, but its virtual methods live in the library.
		const uint8_t data[] = {1, 2, 3};
		Bond::InputStreamHandle stream = allocator.AllocOwnedObject<Bond::MemoryInputStream>(data, Bond::Stream::pos_t(sizeof(data)));
		ASSERT_FORMAT(allocator.GetNumAllocations() == 1, "Expected 1 allocation, but was %d.", allocator.GetNumAllocations());
		ASSERT_MESSAGE(stream->Read() == 1, "Read the wrong first byte.");
		ASSERT_MESSAGE(stream->Read() == 2, "Read the wrong second byte.");
	}
	ASSERT_FORMAT(allocator.GetNumAllocations() == 0, "Leaked %d allocations.", allocator.GetNumAllocations());

	// Out-of-line library code allocating and freeing through an executable-defined allocator.
	void *aligned = allocator.AllocateAligned(100, 64);
	ASSERT_MESSAGE(aligned != nullptr, "AllocateAligned returned null.");
	ASSERT_MESSAGE((reinterpret_cast<uintptr_t>(aligned) % 64) == 0, "AllocateAligned returned a misaligned pointer.");
	allocator.FreeAligned(aligned);

	Bond::DefaultAllocator libraryAllocator;
	void *libraryAligned = libraryAllocator.AllocateAligned(100, 32);
	ASSERT_MESSAGE((reinterpret_cast<uintptr_t>(libraryAligned) % 32) == 0, "DefaultAllocator returned a misaligned pointer.");
	libraryAllocator.FreeAligned(libraryAligned);
	ASSERT_FORMAT(libraryAllocator.GetNumAllocations() == 0, "Leaked %d allocations.", libraryAllocator.GetNumAllocations());
}


#define TEST_ITEMS                                   \
  TEST_ITEM(ModuleIdentity)                          \
  TEST_ITEM(ExportedData)                            \
  TEST_ITEM(ExecutableImplementedInterfaces)         \
  TEST_ITEM(ExceptionThrownByExecutableCrossesLibrary) \
  TEST_ITEM(ExceptionThrownByLibraryCaughtByExecutable) \
  TEST_ITEM(AllocationsCrossModules)                 \

RUN_TESTS(SharedLib, TEST_ITEMS)
