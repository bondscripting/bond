#include "framework/testvmframework.h"
#include "bond/api/libinclude.h"
#include "bond/compiler/compilererror.h"
#include "bond/compiler/frontend.h"
#include "bond/compiler/lexer.h"
#include "bond/compiler/parser.h"
#include "bond/compiler/semanticanalyzer.h"
#include "bond/io/memoryoutputstream.h"
#include "bond/io/memorystreamfactory.h"
#include "bond/io/stdioinputstream.h"
#include "bond/io/stdiostreamfactory.h"
#include "bond/systems/defaultallocator.h"
#include "bond/tools/nativebindinggenerator.h"
#include "nativestructs.inl"

class Vector3
{
public:
	Vector3(): mX(0), mY(0), mZ(0) {}
	Vector3(int x, int y, int z): mX(x), mY(y), mZ(z) {}

	int GetX() const { return mX; }
	void SetX(const int x) { mX = x; }

	int GetY() const { return mY; }
	void SetY(const int y) { mY = y; }

	int GetZ() const { return mZ; }
	void SetZ(const int z) { mZ = z; }

	int Dot(const Vector3 &other) const { return mX * other.mX + mY * other.mY + mZ * other.mZ; }

private:
	int mX;
	int mY;
	int mZ;
};


void Vector3__Dot(Bond::StackFrame &frame)
{
	const Vector3 &v1 = *frame.GetArg<const Vector3 *>(0);
	const Vector3 &v2 = *frame.GetArg<const Vector3 *>(1);
	const int dot = v1.Dot(v2);
	frame.SetReturnValue(int32_t(dot));
}


void Vector3__X__get(Bond::StackFrame &frame)
{
	const Vector3 &v = *frame.GetArg<const Vector3 *>(0);
	frame.SetReturnValue(int32_t(v.GetX()));
}


void Vector3__X__set(Bond::StackFrame &frame)
{
	Vector3 &v = *frame.GetArg<Vector3 *>(0);
	const int x = int(frame.GetArg<int32_t>(1));
	v.SetX(x);
}


void Vector3__Y__get(Bond::StackFrame &frame)
{
	const Vector3 &v = *frame.GetArg<const Vector3 *>(0);
	frame.SetReturnValue(int32_t(v.GetY()));
}


void Vector3__Y__set(Bond::StackFrame &frame)
{
	Vector3 &v = *frame.GetArg<Vector3 *>(0);
	const int y = int(frame.GetArg<int32_t>(1));
	v.SetY(y);
}


void Vector3__Z__get(Bond::StackFrame &frame)
{
	const Vector3 &v = *frame.GetArg<const Vector3 *>(0);
	frame.SetReturnValue(int32_t(v.GetZ()));
}


void Vector3__Z__set(Bond::StackFrame &frame)
{
	Vector3 &v = *frame.GetArg<Vector3 *>(0);
	const int z = int(frame.GetArg<int32_t>(1));
	v.SetZ(z);
}


DEFINE_VM_TEST_WITH_BINDING(NativeStructs, "scripts/vm_NativeStructs.bond", &NATIVE_STRUCTS_BINDING_COLLECTION)
{
	using namespace Bond;

	Vector3 v3a;
	Vector3 v3b;

	vm.CallVoidFunction("SetVector3", &v3a, int32_t(21), int32_t(-22), int32_t(23));
	ASSERT_FORMAT(v3a.GetX() == 21, "Expected 21, but was %" BOND_PRId32 ".", v3a.GetX());
	ASSERT_FORMAT(v3a.GetY() == -22, "Expected -22, but was %" BOND_PRId32 ".", v3a.GetY());
	ASSERT_FORMAT(v3a.GetZ() == 23, "Expected 23, but was %" BOND_PRId32 ".", v3a.GetZ());

	v3a = Vector3(44, 444, 4444);
	VALIDATE_FUNCTION_CALL(int32_t, "GetVector3X", int32_t(v3a.GetX()), &v3a);
	VALIDATE_FUNCTION_CALL(int32_t, "Vector3.X." BOND_NATIVE_GETTER_SUFFIX, int32_t(v3a.GetX()), &v3a);
	VALIDATE_FUNCTION_CALL(int32_t, "GetVector3Y", int32_t(v3a.GetY()), &v3a);
	VALIDATE_FUNCTION_CALL(int32_t, "Vector3.Y." BOND_NATIVE_GETTER_SUFFIX, int32_t(v3a.GetY()), &v3a);
	VALIDATE_FUNCTION_CALL(int32_t, "GetVector3Z", int32_t(v3a.GetZ()), &v3a);
	VALIDATE_FUNCTION_CALL(int32_t, "Vector3.Z." BOND_NATIVE_GETTER_SUFFIX, int32_t(v3a.GetZ()), &v3a);

	v3a = Vector3(1, 2, 3);
	vm.CallVoidFunction("Vector3.X." BOND_NATIVE_SETTER_SUFFIX, &v3a, int32_t(-31));
	ASSERT_MESSAGE((v3a.GetX() == -31) && (v3a.GetY() == 2) && (v3a.GetZ() == 3), "Vector3.X setter changed the wrong component.");
	vm.CallVoidFunction("Vector3.Y." BOND_NATIVE_SETTER_SUFFIX, &v3a, int32_t(-32));
	ASSERT_MESSAGE((v3a.GetX() == -31) && (v3a.GetY() == -32) && (v3a.GetZ() == 3), "Vector3.Y setter changed the wrong component.");
	vm.CallVoidFunction("Vector3.Z." BOND_NATIVE_SETTER_SUFFIX, &v3a, int32_t(-33));
	ASSERT_MESSAGE((v3a.GetX() == -31) && (v3a.GetY() == -32) && (v3a.GetZ() == -33), "Vector3.Z setter changed the wrong component.");

	v3a = Vector3(2, 3, 4);
	v3b = Vector3(5, -6, 7);
	VALIDATE_FUNCTION_CALL(int32_t, "DotVector3", int32_t(20), &v3a, &v3b);
	VALIDATE_FUNCTION_CALL(int32_t, "Vector3.Dot", int32_t(20), &v3a, &v3b);

	v3a = Vector3(17, 17, 17);
	int32_t oi = 91; int32_t ri = v3a.GetY() + oi;
	VALIDATE_FUNCTION_CALL(int32_t, "Vector3AddY", ri, &v3a, oi);
	ASSERT_FORMAT(v3a.GetX() == 17, "Expected 17, but was %" BOND_PRId32 ".", v3a.GetX());
	ASSERT_FORMAT(v3a.GetY() == ri, "Expected %" BOND_PRId32 ", but was %" BOND_PRId32 ".", ri, v3a.GetY());
	ASSERT_FORMAT(v3a.GetZ() == 17, "Expected 17, but was %" BOND_PRId32 ".", v3a.GetZ());

	v3a = Vector3(23, 23, 23);
	ri = v3a.GetY() + 1;
	VALIDATE_FUNCTION_CALL(int32_t, "Vector3PreincrementY", ri, &v3a);
	ASSERT_FORMAT(v3a.GetY() == ri, "Expected %" BOND_PRId32 ", but was %" BOND_PRId32 ".", ri, v3a.GetY());

	v3a = Vector3(56, 56, 56);
	ri = v3a.GetY();
	VALIDATE_FUNCTION_CALL(int32_t, "Vector3PostincrementY", ri, &v3a);
	ASSERT_FORMAT(v3a.GetY() == (ri + 1), "Expected %" BOND_PRId32 ", but was %" BOND_PRId32 ".", ri + 1, v3a.GetY());
}


namespace
{
// Each returns a small integer whose C++ type differs in signedness from the declared Bond type.
void ReturnBoolFromChar(Bond::StackFrame &frame) { frame.SetReturnValue(int8_t(5)); }
void ReturnCharFromUChar(Bond::StackFrame &frame) { frame.SetReturnValue(uint8_t(200)); }
void ReturnUCharFromChar(Bond::StackFrame &frame) { frame.SetReturnValue(int8_t(-1)); }
void ReturnShortFromUShort(Bond::StackFrame &frame) { frame.SetReturnValue(uint16_t(0xffff)); }
void ReturnUShortFromShort(Bond::StackFrame &frame) { frame.SetReturnValue(int16_t(-1)); }


const Bond::NativeFunctionBinding NATIVE_TYPES_BINDING_FUNCTIONS[] =
{
	{"ReturnBoolFromChar", ReturnBoolFromChar},
	{"ReturnCharFromUChar", ReturnCharFromUChar},
	{"ReturnUCharFromChar", ReturnUCharFromChar},
	{"ReturnShortFromUShort", ReturnShortFromUShort},
	{"ReturnUShortFromShort", ReturnUShortFromShort},
	{nullptr, nullptr}
};

const Bond::NativeBindingCollection NATIVE_TYPES_BINDING_COLLECTION = {NATIVE_TYPES_BINDING_FUNCTIONS, 5};


enum NativeTypesEnum
{
	NATIVE_TYPES_ENUM_VALUE = 17
};


struct Pair
{
	int32_t a;
	int32_t b;
};
}


DEFINE_VM_TEST_WITH_BINDING(NativeTypes, "scripts/vm_NativeTypes.bond", &NATIVE_TYPES_BINDING_COLLECTION)
{
	using namespace Bond;

	// Small integer return values are widened according to the declared Bond type.
	VALIDATE_FUNCTION_CALL(uint32_t, "BoolReturnIsNormalized", uint32_t(1));
	VALIDATE_FUNCTION_CALL(uint32_t, "CharReturnIsSignExtended", uint32_t(1));
	VALIDATE_FUNCTION_CALL(uint32_t, "UCharReturnIsZeroExtended", uint32_t(1));
	VALIDATE_FUNCTION_CALL(uint32_t, "ShortReturnIsSignExtended", uint32_t(1));
	VALIDATE_FUNCTION_CALL(uint32_t, "UShortReturnIsZeroExtended", uint32_t(1));

	// Bool arguments are stored as 0 or 1 whatever type is used to pass them.
	VALIDATE_FUNCTION_CALL(uint32_t, "IsTrue", uint32_t(1), true);
	VALIDATE_FUNCTION_CALL(uint32_t, "IsTrue", uint32_t(1), uint8_t(23));
	VALIDATE_FUNCTION_CALL(uint32_t, "IsTrue", uint32_t(1), int8_t(-1));
	VALIDATE_FUNCTION_CALL(uint32_t, "IsTrue", uint32_t(0), int8_t(0));

	// Integers match by size, whichever fundamental type the fixed width typedefs alias.
	VALIDATE_FUNCTION_CALL(int32_t, "PassChar", int32_t(65), char(65));
	VALIDATE_FUNCTION_CALL(int32_t, "PassChar", int32_t(-3), static_cast<signed char>(-3));
	VALIDATE_FUNCTION_CALL(int64_t, "PassLong", int64_t(-5), static_cast<long long>(-5));
	VALIDATE_FUNCTION_CALL(int64_t, "PassLong", int64_t(6), static_cast<unsigned long long>(6));
	VALIDATE_FUNCTION_CALL(int32_t, "PassInt", int32_t(17), NATIVE_TYPES_ENUM_VALUE);
	VALIDATE_FUNCTION_CALL(uint32_t, "IsNull", uint32_t(1), nullptr);

	Pair pair = {3, 4};
	VALIDATE_FUNCTION_CALL(int32_t, "SumPair", int32_t(7), pair);
	VALIDATE_FUNCTION_CALL(int32_t, "SumPairPointer", int32_t(7), &pair);

#if BOND_RUNTIME_CHECKS_ENABLED
	int32_t intResult = 0;
	int64_t longResult = 0;
	ASSERT_BOND_EXCEPTION("Attempt to access argument 0 using wrong type.", vm.CallFunction("SumPair", &intResult, &pair));
	ASSERT_BOND_EXCEPTION("Attempt to access argument 0 using wrong type.", vm.CallFunction("SumPairPointer", &intResult, pair));
	ASSERT_BOND_EXCEPTION("Attempt to access argument 0 using wrong type.", vm.CallFunction("PassLong", &longResult, int32_t(1)));
	ASSERT_BOND_EXCEPTION("Attempt to access argument 0 using wrong type.", vm.CallFunction("PassChar", &intResult, int16_t(1)));
	ASSERT_BOND_EXCEPTION("Attempt to access argument 0 using wrong type.", vm.CallFunction("PassInt", &intResult, 1.0f));
	ASSERT_BOND_EXCEPTION("Attempt to call function with too many arguments.", vm.CallFunction("PassInt", &intResult, int32_t(1), int32_t(2)));
#endif
}


namespace
{
const size_t MAX_BINDING_FILE_SIZE = 4 * 1024;

// Reads a file and strips carriage returns, so that a CRLF checkout compares equal to LF output.
size_t ReadFileWithoutCarriageReturns(const char *assertFile, int assertLine, const char *fileName, char *buffer, size_t bufferSize)
{
	Bond::StdioInputStream stream(fileName);
	uint8_t rawBuffer[MAX_BINDING_FILE_SIZE];
	const size_t rawSize = stream.Read(rawBuffer, sizeof(rawBuffer));
	TestFramework::Assert(rawSize > 0, assertFile, assertLine, "Failed to read '%s'.", fileName);
	TestFramework::Assert(rawSize < sizeof(rawBuffer), assertFile, assertLine, "'%s' exceeds the %d byte test buffer.", fileName, int(sizeof(rawBuffer)));

	size_t size = 0;
	for (size_t i = 0; (i < rawSize) && (size < bufferSize); ++i)
	{
		if (rawBuffer[i] != '\r')
		{
			buffer[size++] = char(rawBuffer[i]);
		}
	}
	return size;
}


void AssertMatchesFile(const char *assertFile, int assertLine, const char *fileName, const char *actual, size_t actualSize)
{
	char expected[MAX_BINDING_FILE_SIZE];
	const size_t expectedSize = ReadFileWithoutCarriageReturns(assertFile, assertLine, fileName, expected, sizeof(expected));

	size_t i = 0;
	while ((i < expectedSize) && (i < actualSize) && (expected[i] == actual[i]))
	{
		++i;
	}

	const size_t EXCERPT_SIZE = 32;
	const size_t expectedExcerpt = ((expectedSize - i) < EXCERPT_SIZE) ? (expectedSize - i) : EXCERPT_SIZE;
	const size_t actualExcerpt = ((actualSize - i) < EXCERPT_SIZE) ? (actualSize - i) : EXCERPT_SIZE;
	TestFramework::Assert((i == expectedSize) && (i == actualSize), assertFile, assertLine,
		"Generated output differs from '%s' at offset %d. Expected \"%.*s\", but was \"%.*s\".",
		fileName, int(i), int(expectedExcerpt), expected + i, int(actualExcerpt), actual + i);
}
}


// The checked-in test/nativestructs.* files must match what the generator emits for the script.
DEFINE_TEST(NativeBindingGenerator)
{
	using namespace Bond;
	using namespace TestFramework;

	DefaultAllocator lexerAllocator;
	DefaultAllocator parserAllocator;
	DefaultAllocator streamFactoryAllocator;
	DefaultAllocator frontEndAllocator;
	AllocationLeakGuard<DefaultAllocator> lexerLeakGuard(lexerAllocator, "Lexer", __FILE__, __LINE__);
	AllocationLeakGuard<DefaultAllocator> parserLeakGuard(parserAllocator, "Parser", __FILE__, __LINE__);
	AllocationLeakGuard<DefaultAllocator> streamFactoryLeakGuard(streamFactoryAllocator, "Stream factory", __FILE__, __LINE__);
	AllocationLeakGuard<DefaultAllocator> frontEndLeakGuard(frontEndAllocator, "Front end", __FILE__, __LINE__);

	char cppBuffer[MAX_BINDING_FILE_SIZE];
	char hBuffer[MAX_BINDING_FILE_SIZE];
	size_t cppSize = 0;
	size_t hSize = 0;

	try
	{
		CompilerErrorBuffer errorBuffer;
		TokenCollectionStore tokenStore((TokenCollectionStore::allocator_type(&lexerAllocator)));
		ParseNodeStore parseNodeStore((ParseNodeStore::allocator_type(&parserAllocator)));
		Lexer lexer(lexerAllocator, errorBuffer);
		Parser parser(parserAllocator, errorBuffer, parseNodeStore);
		SemanticAnalyzer analyzer(errorBuffer);
		StdioStreamFactory streamFactory(streamFactoryAllocator, nullptr, nullptr, false);
		MemoryStreamFactory stdLibFactory(streamFactoryAllocator, INCLUDE_FILE_INDEX, &streamFactory, false);
		FrontEnd frontEnd(frontEndAllocator, tokenStore, lexer, parser, analyzer, stdLibFactory);
		frontEnd.AddInputFile("scripts/vm_NativeStructs.bond");
		const TranslationUnit *translationUnitList = frontEnd.Analyze();
		Assert(!errorBuffer.HasErrors(), __FILE__, __LINE__, "Failed to compile 'scripts/vm_NativeStructs.bond'.");

		MemoryOutputStream cppStream(cppBuffer, Stream::pos_t(sizeof(cppBuffer)));
		MemoryOutputStream hStream(hBuffer, Stream::pos_t(sizeof(hBuffer)));
		NativeBindingGenerator generator;
		generator.Generate(translationUnitList, cppStream, hStream, "NATIVE_STRUCTS_BINDING_COLLECTION", "nativestructs.h");
		cppSize = size_t(cppStream.GetPosition());
		hSize = size_t(hStream.GetPosition());
		Assert((cppSize < sizeof(cppBuffer)) && (hSize < sizeof(hBuffer)), __FILE__, __LINE__,
			"Generated bindings exceed the %d byte test buffer.", int(sizeof(cppBuffer)));
	}
	catch (const Exception &e)
	{
		throw AssertionFailure(__FILE__, __LINE__, "%s", e.GetMessage());
	}

	AssertMatchesFile(__FILE__, __LINE__, "test/nativestructs.inl", cppBuffer, cppSize);
	AssertMatchesFile(__FILE__, __LINE__, "test/nativestructs.h", hBuffer, hSize);
}


#define TEST_ITEMS                              \
  TEST_ITEM(NativeStructs)                      \
  TEST_ITEM(NativeTypes)                        \
  TEST_ITEM(NativeBindingGenerator)             \

RUN_TESTS(Native, TEST_ITEMS)
