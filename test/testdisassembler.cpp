#include "framework/testframework.h"
#include "bond/compiler/codegenerator.h"
#include "bond/compiler/compilererror.h"
#include "bond/compiler/lexer.h"
#include "bond/compiler/parser.h"
#include "bond/compiler/parsenodes.h"
#include "bond/compiler/semanticanalyzer.h"
#include "bond/io/memoryoutputstream.h"
#include "bond/systems/defaultallocator.h"
#include "bond/tools/disassembler.h"
#include <cstring>

#define ASSERT_DISASSEMBLY_CONTAINS(source, expected) \
  AssertDisassemblyContains(__FILE__, __LINE__, (source), (expected))


/// \brief Compiles the given Bond source and asserts that its disassembly contains the expected text.
/// The non-ASCII bytes in these tests are written as escapes, so that editors and git can't alter them.
static void AssertDisassemblyContains(const char *assertFile, int assertLine, const char *source, const char *expected)
{
	Bond::DefaultAllocator allocator;
	{
		// The code generator uses the symbol tables owned by the semantic analyzer, so it must outlive
		// code generation.
		Bond::CompilerErrorBuffer errorBuffer;
		Bond::Lexer lexer(allocator, errorBuffer);
		Bond::ParseNodeStore parseNodeStore((Bond::ParseNodeStore::allocator_type(&allocator)));
		Bond::Parser parser(allocator, errorBuffer, parseNodeStore);
		Bond::SemanticAnalyzer analyzer(errorBuffer);
		Bond::TokenCollectionHandle tokenCollectionHandle = lexer.Lex("Disassembler", source, strlen(source));
		Bond::TranslationUnit *translationUnit = nullptr;

		if (!errorBuffer.HasErrors())
		{
			Bond::TokenStream stream = tokenCollectionHandle->GetTokenStream();
			translationUnit = parser.Parse(stream);
		}

		if (!errorBuffer.HasErrors())
		{
			translationUnit->SetRequiresCodeGeneration(true);
			analyzer.Analyze(translationUnit);
		}

		char cbo[4096];
		Bond::MemoryOutputStream cboStream(cbo, Bond::Stream::pos_t(sizeof(cbo)));
		if (!errorBuffer.HasErrors())
		{
			Bond::CodeGenerator generator(allocator, errorBuffer);
			generator.Generate(translationUnit, cboStream);
		}
		TestFramework::Assert(!errorBuffer.HasErrors(), assertFile, assertLine, "Failed to compile the test source.");

		// Leave room for a terminator, since the buffer is zero-initialized and searched as a C string.
		char output[16384] = {};
		Bond::MemoryOutputStream outputStream(output, Bond::Stream::pos_t(sizeof(output) - 1));
		Bond::Disassembler disassembler(allocator);
		disassembler.Disassemble(cbo, size_t(cboStream.GetPosition()), outputStream);

		TestFramework::Assert(strstr(output, expected) != nullptr, assertFile, assertLine,
			"Expected the disassembly to contain '%s'.", expected);
	}
	TestFramework::Assert(allocator.GetNumAllocations() == 0, assertFile, assertLine, "Memory leak.");
}


DEFINE_TEST(ShortUtf8String)
{
	ASSERT_DISASSEMBLY_CONTAINS(
		"const char *F() { return \"caf\xC3\xA9 \xF0\x9F\x98\x80\"; }",
		"\"caf\xC3\xA9 \xF0\x9F\x98\x80\"");
}


// A long string is abbreviated to its first and last 12 bytes, but a UTF-8 sequence that straddles
// either boundary is left out rather than split.
DEFINE_TEST(AbbreviatedUtf8String)
{
	ASSERT_DISASSEMBLY_CONTAINS(
		"const char *F() { return \"aaaaaaaaaaa\xC3\xA9" "cccccccccc\xE2\x82\xAC" "bbbbbbbbbb\"; }",
		"\"aaaaaaaaaaa...bbbbbbbbbb\"");

	// Sequences that end or start exactly at a boundary are kept.
	ASSERT_DISASSEMBLY_CONTAINS(
		"const char *F() { return \"aaaaaaaaaa\xC3\xA9" "cccccccccc\xE2\x82\xAC" "bbbbbbbbb\"; }",
		"\"aaaaaaaaaa\xC3\xA9...\xE2\x82\xAC" "bbbbbbbbb\"");
}


// Control characters, quotes, backslashes and bytes that aren't valid UTF-8 are escaped.
DEFINE_TEST(EscapedString)
{
	ASSERT_DISASSEMBLY_CONTAINS(
		"const char *F() { return \"x\\ny\\x01z\\\"\\\\\\xFF!\"; }",
		"\"x\\ny\\x01z\\\"\\\\\\xFF!\"");
}


#define TEST_ITEMS                              \
  TEST_ITEM(ShortUtf8String)                    \
  TEST_ITEM(AbbreviatedUtf8String)              \
  TEST_ITEM(EscapedString)                      \

RUN_TESTS(Disassembler, TEST_ITEMS)
