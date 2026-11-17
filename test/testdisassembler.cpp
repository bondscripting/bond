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
  AssertDisassembly(__FILE__, __LINE__, (source), (expected), true, Bond::BOND_NATIVE_POINTER_SIZE)

#define ASSERT_DISASSEMBLY_NOT_CONTAINS(source, unexpected) \
  AssertDisassembly(__FILE__, __LINE__, (source), (unexpected), false, Bond::BOND_NATIVE_POINTER_SIZE)

#define ASSERT_DISASSEMBLY_CONTAINS_P(source, expected, pointerSize) \
  AssertDisassembly(__FILE__, __LINE__, (source), (expected), true, (pointerSize))

#define ASSERT_DISASSEMBLY_NOT_CONTAINS_P(source, unexpected, pointerSize) \
  AssertDisassembly(__FILE__, __LINE__, (source), (unexpected), false, (pointerSize))


/// \brief Compiles the given Bond source for the given pointer size and asserts that its disassembly
/// contains, or doesn't contain, the given text. The non-ASCII bytes in these tests are written as
/// escapes, so that editors and git can't alter them.
static void AssertDisassembly(
	const char *assertFile,
	int assertLine,
	const char *source,
	const char *text,
	bool shouldContain,
	Bond::PointerSize pointerSize)
{
	Bond::DefaultAllocator allocator;
	{
		// The code generator uses the symbol tables owned by the semantic analyzer, so it must outlive
		// code generation.
		Bond::CompilerErrorBuffer errorBuffer;
		Bond::Lexer lexer(allocator, errorBuffer);
		Bond::ParseNodeStore parseNodeStore((Bond::ParseNodeStore::allocator_type(&allocator)));
		Bond::Parser parser(allocator, errorBuffer, parseNodeStore);
		Bond::SemanticAnalyzer analyzer(errorBuffer, pointerSize);
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
			Bond::CodeGenerator generator(allocator, errorBuffer, pointerSize);
			generator.Generate(translationUnit, cboStream);
		}
		TestFramework::Assert(!errorBuffer.HasErrors(), assertFile, assertLine, "Failed to compile the test source.");

		// Leave room for a terminator, since the buffer is zero-initialized and searched as a C string.
		char output[16384] = {};
		Bond::MemoryOutputStream outputStream(output, Bond::Stream::pos_t(sizeof(output) - 1));
		Bond::Disassembler disassembler(allocator);
		disassembler.Disassemble(cbo, size_t(cboStream.GetPosition()), outputStream);

		if (shouldContain)
		{
			TestFramework::Assert(strstr(output, text) != nullptr, assertFile, assertLine,
				"Expected the disassembly to contain '%s'.", text);
		}
		else
		{
			TestFramework::Assert(strstr(output, text) == nullptr, assertFile, assertLine,
				"Expected the disassembly not to contain '%s'.", text);
		}
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


// A struct passed by value through a pointer is copied with its exact size, so that the VM doesn't
// read past the end of the pointed-to object.
DEFINE_TEST(LoadMemExactSize)
{
	ASSERT_DISASSEMBLY_CONTAINS(
		"struct Int3 { int x; int y; int z; };"
		"int G(Int3 v) { return v.x; }"
		"int F(Int3 *p) { return G(*p); }",
		"loadmemw    12\n");
}


// The operand stack size of a function includes every slot occupied by a struct pushed by value.
DEFINE_TEST(LoadMemStackSize)
{
	ASSERT_DISASSEMBLY_CONTAINS(
		"struct Big { int[6] x; };"
		"int G(Big b) { return b.x[0]; }"
		"int F(Big *p) { return G(*p); }",
		"int F(*)\n\targ size: 8\n\tpacked arg size: 8\n\tlocal size: 0\n\tstack size: 24\n");
}


// A while or for loop is entered through its condition, which follows the body, so that each
// iteration executes a single branch.
DEFINE_TEST(LoopInversion)
{
	ASSERT_DISASSEMBLY_CONTAINS(
		"int F(int n) { int i = 0; while (i < n) { ++i; } return i; }",
		"     2: goto        3 (8)\n"
		"     5: inci        0, 1\n"
		"     8: push32_l0   \n"
		"     9: push32_p0   \n"
		"    10: cmplti      \n"
		"    11: ifnz        -9 (5)\n");

	ASSERT_DISASSEMBLY_CONTAINS(
		"int F(int n) { int i = 0; for (int j = 0; j < n; ++j) { ++i; } return i; }",
		"     4: goto        6 (13)\n"
		"     7: inci        0, 1\n"
		"    10: inci        1, 1\n"
		"    13: push32_l1   \n"
		"    14: push32_p0   \n"
		"    15: cmplti      \n"
		"    16: ifnz        -12 (7)\n");
}


// Constant loop conditions are not tested at runtime.
DEFINE_TEST(ConstantLoopConditions)
{
	ASSERT_DISASSEMBLY_NOT_CONTAINS(
		"int F(int n) { int i = 0; while (true) { if (i >= n) break; ++i; } return i; }",
		"consti_1");

	ASSERT_DISASSEMBLY_NOT_CONTAINS(
		"int F(int n) { int i = 0; for (;true;) { if (i >= n) break; ++i; } return i; }",
		"consti_1");

	ASSERT_DISASSEMBLY_NOT_CONTAINS(
		"int F(int n) { int i = n; while (false) { ++i; } return i; }",
		"inci");

	ASSERT_DISASSEMBLY_CONTAINS(
		"int F(int n) { int i = n; for (i += 5; false; ++i) { i += 2; } return i; }",
		"     2: inci        0, 5\n"
		"     5: push32_l0   \n"
		"     6: return32    \n");

	// A constant false do loop runs its body once, without a branch.
	ASSERT_DISASSEMBLY_CONTAINS(
		"int F(int n) { int i = 0; do { ++i; } while (false); return i; }",
		"     2: inci        0, 1\n"
		"     5: push32_l0   \n"
		"     6: return32    \n");
}


// 'if (c) break;' and 'if (c) continue;' compile to a single conditional branch.
DEFINE_TEST(ConditionalBreakAndContinue)
{
	ASSERT_DISASSEMBLY_CONTAINS(
		"int F(int n) { int i = 0; while (true) { if (i >= n) break; ++i; } return i; }",
		"     4: cmpgei      \n"
		"     5: ifnz        6 (14)\n"
		"     8: inci        0, 1\n"
		"    11: goto        -12 (2)\n"
		"    14: push32_l0   \n");

	ASSERT_DISASSEMBLY_CONTAINS(
		"int F(int n) { int i = 0; while (true) { if (!(i < n)) { break; } ++i; } return i; }",
		"     4: cmplti      \n"
		"     5: ifz         6 (14)\n");

	ASSERT_DISASSEMBLY_CONTAINS(
		"int F(int n) { int s = 0; for (int i = 0; i < n; ++i) { if (i == 2) continue; s += i; } return s; }",
		"     9: cmpeqi      \n"
		"    10: ifnz        4 (17)\n"
		"    13: push32_l0   \n"
		"    14: push32_l1   \n"
		"    15: addi        \n"
		"    16: pop32_l0    \n"
		"    17: inci        1, 1\n");
}


// 'x = x + c', 'x = c + x' and 'x = x - c' on a 32 or 64-bit local use an increment instruction when
// the constant fits in 8 bits.
DEFINE_TEST(AssignmentIncrement)
{
	ASSERT_DISASSEMBLY_CONTAINS(
		"int F(int n) { int i = n; i = i + 1; i = 2 + i; i = i - 3; return i; }",
		"     2: inci        0, 1\n"
		"     5: inci        0, 2\n"
		"     8: inci        0, -3\n");

	ASSERT_DISASSEMBLY_CONTAINS(
		"long F(long n) { n = n - 128; return n; }",
		"     0: incl        -1, -128\n");

	ASSERT_DISASSEMBLY_NOT_CONTAINS(
		"int F(int n) { n = n + 128; n = n - (-128); return n; }",
		"inci");

	ASSERT_DISASSEMBLY_NOT_CONTAINS(
		"int F(int n, int m) { n = m + 1; n = 1 - n; return n; }",
		"inci");

	ASSERT_DISASSEMBLY_NOT_CONTAINS(
		"int F(char c) { c = cast<char>(c + 1); return c; }",
		"inci");
}


// A goto whose target is a return instruction is replaced with a copy of that instruction, padded
// with nops.
DEFINE_TEST(GotoToReturn)
{
	ASSERT_DISASSEMBLY_CONTAINS_P(
		"void F(int *p, bool c) { if (c) { *p = 1; } else { *p = 2; } }",
		"     9: return      \n"
		"    10: nop         \n"
		"    11: nop         \n"
		"    12: push64_p0   \n",
		Bond::POINTER_64BIT);

	ASSERT_DISASSEMBLY_CONTAINS(
		"int F(bool c, int a, int b) { return c ? a : b; }",
		"     6: push32_p1   \n"
		"     7: return32    \n"
		"     8: nop         \n"
		"     9: nop         \n"
		"    10: push32_p2   \n"
		"    11: return32    \n");

	ASSERT_DISASSEMBLY_CONTAINS(
		"long F(bool c, long a, long b) { return c ? a : b; }",
		"     7: return64    \n"
		"     8: nop         \n");

	ASSERT_DISASSEMBLY_CONTAINS(
		"struct Pair { int a; int b; };"
		"Pair F(bool c, Pair a, Pair b) { return c ? a : b; }",
		"     6: loadfp      -16\n"
		"     9: returnmemw  8\n"
		"    12: loadfp      -24\n"
		"    15: returnmemw  8\n");
}


// Where only the truth value matters, a comparison of a 32-bit value against zero tests the value
// directly.
DEFINE_TEST(TruthTestAgainstZero)
{
	ASSERT_DISASSEMBLY_CONTAINS(
		"int F(int x) { if (x != 0) { return 1; } return 2; }",
		"     0: push32_p0   \n"
		"     1: ifz         ");

	ASSERT_DISASSEMBLY_CONTAINS(
		"int F(int x) { if (x == 0) { return 1; } return 2; }",
		"     0: push32_p0   \n"
		"     1: ifnz        ");

	ASSERT_DISASSEMBLY_CONTAINS(
		"int F(int x, bool y) { return ((x != 0) && y) ? 1 : 2; }",
		"     0: push32_p0   \n"
		"     1: ifz         ");

	ASSERT_DISASSEMBLY_NOT_CONTAINS_P(
		"int F(int *p) { if (p != null) { return 1; } return 2; }",
		"cmpneqi",
		Bond::POINTER_32BIT);

	// The comparison remains where the result is used as a value, for long, 64-bit pointer and
	// floating point operands, and for comparisons against true.
	ASSERT_DISASSEMBLY_CONTAINS(
		"bool F(int x, bool y) { bool b = (x != 0) || y; return b; }",
		"cmpneqi");

	ASSERT_DISASSEMBLY_CONTAINS(
		"bool F(int x) { return x != 0; }",
		"cmpneqi");

	ASSERT_DISASSEMBLY_CONTAINS_P(
		"int F(int *p) { if (p != null) { return 1; } return 2; }",
		"cmpneql",
		Bond::POINTER_64BIT);

	ASSERT_DISASSEMBLY_CONTAINS(
		"int F(long l) { if (l != 0l) { return 1; } return 2; }",
		"cmpneql");

	ASSERT_DISASSEMBLY_CONTAINS(
		"int F(float f) { if (f != 0.0f) { return 1; } return 2; }",
		"cmpneqf");

	ASSERT_DISASSEMBLY_CONTAINS(
		"int F(bool b) { if (b == true) { return 1; } return 2; }",
		"cmpeqi");
}


// A local struct initialized by a call is returned into directly, without a temporary or a copy.
DEFINE_TEST(ReturnValueElision)
{
	ASSERT_DISASSEMBLY_CONTAINS(
		"struct R { int a; int b; };"
		"R M(int x) { R r = { x, x }; return r; }"
		"int F() { R v = M(1); return v.a; }",
		"int F()\n"
		"\targ size: 0\n"
		"\tpacked arg size: 0\n"
		"\tlocal size: 8\n"
		"\tstack size: 16\n"
		"\tframe pointer alignment: 8\n"
		"\tcode size: 9\n"
		"     0: consti_1    \n"
		"     1: loadfp      0\n"
		"     4: invoke      M\n"
		"     7: push32_l0   \n"
		"     8: return32    \n");

	ASSERT_DISASSEMBLY_NOT_CONTAINS(
		"struct R { int a; int b; };"
		"struct P { R x; R y; };"
		"R M(int x) { R r = { x, x }; return r; }"
		"int F() { P p = { M(1), M(2) }; return p.y.a; }",
		"memcopyw");

	// Assignment still copies through a temporary.
	ASSERT_DISASSEMBLY_CONTAINS(
		"struct R { int a; int b; };"
		"R M(int x) { R r = { x, x }; return r; }"
		"int F() { R v = M(1); v = M(v.a); return v.a; }",
		"memcopyw");
}


#define TEST_ITEMS                              \
  TEST_ITEM(ShortUtf8String)                    \
  TEST_ITEM(AbbreviatedUtf8String)              \
  TEST_ITEM(EscapedString)                      \
  TEST_ITEM(LoadMemExactSize)                   \
  TEST_ITEM(LoadMemStackSize)                   \
  TEST_ITEM(LoopInversion)                      \
  TEST_ITEM(ConstantLoopConditions)             \
  TEST_ITEM(ConditionalBreakAndContinue)        \
  TEST_ITEM(AssignmentIncrement)                \
  TEST_ITEM(GotoToReturn)                       \
  TEST_ITEM(TruthTestAgainstZero)               \
  TEST_ITEM(ReturnValueElision)                 \

RUN_TESTS(Disassembler, TEST_ITEMS)
