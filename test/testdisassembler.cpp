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
#include <cstdio>
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
		"    10: iflti       -8 (5)\n");

	ASSERT_DISASSEMBLY_CONTAINS(
		"int F(int n) { int i = 0; for (int j = 0; j < n; ++j) { ++i; } return i; }",
		"     4: goto        6 (13)\n"
		"     7: inci        0, 1\n"
		"    10: inci        1, 1\n"
		"    13: push32_l1   \n"
		"    14: push32_p0   \n"
		"    15: iflti       -11 (7)\n");
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
		"     4: ifgei       6 (13)\n"
		"     7: inci        0, 1\n"
		"    10: goto        -11 (2)\n"
		"    13: push32_l0   \n");

	ASSERT_DISASSEMBLY_CONTAINS(
		"int F(int n) { int i = 0; while (true) { if (!(i < n)) { break; } ++i; } return i; }",
		"     4: ifgei       6 (13)\n");

	ASSERT_DISASSEMBLY_CONTAINS(
		"int F(int n) { int s = 0; for (int i = 0; i < n; ++i) { if (i == 2) continue; s += i; } return s; }",
		"     9: ifeqi       4 (16)\n"
		"    12: push32_l0   \n"
		"    13: push32_l1   \n"
		"    14: addi        \n"
		"    15: pop32_l0    \n"
		"    16: inci        1, 1\n");
}


// A constant true loop whose body ends in 'if (c) break;' branches back to its start when c is false,
// instead of branching to its end when c is true and then executing a goto back to the start.
DEFINE_TEST(TailBreakLoops)
{
	const char *const expected =
		"     2: inci        0, 1\n"
		"     5: push32_l0   \n"
		"     6: push32_p0   \n"
		"     7: iflti       -8 (2)\n"
		"    10: push32_l0   \n"
		"    11: return32    \n";

	ASSERT_DISASSEMBLY_CONTAINS(
		"int F(int n) { int i = 0; while (true) { ++i; if (i >= n) break; } return i; }",
		expected);

	ASSERT_DISASSEMBLY_CONTAINS(
		"int F(int n) { int i = 0; do { ++i; if (i >= n) { break; } } while (true); return i; }",
		expected);

	ASSERT_DISASSEMBLY_CONTAINS(
		"int F(int n) { int i = 0; for (;;) { ++i; if (!(i < n)) break; } return i; }",
		expected);

	ASSERT_DISASSEMBLY_CONTAINS(
		"int F(int n) { int i = 0; for (;true;) { ++i; { if (i >= n) break; } } return i; }",
		expected);

	// Continue jumps to the start, and each operand of '&&' branches to the start when false.
	ASSERT_DISASSEMBLY_CONTAINS(
		"int F(int n, int m) { int i = 0; while (true) { ++i; if (i % 2 == 0) continue; if ((i >= n) && (i >= m)) break; } return i; }",
		"     8: ifzi        -9 (2)\n"
		"    11: push32_l0   \n"
		"    12: push32_p0   \n"
		"    13: iflti       -14 (2)\n"
		"    16: push32_l0   \n"
		"    17: push32_p1   \n"
		"    18: iflti       -19 (2)\n"
		"    21: push32_l0   \n");

	// A 64-bit tail condition tests all 64 bits.
	ASSERT_DISASSEMBLY_CONTAINS_P(
		"int F(long v) { int i = 0; while (true) { ++i; v = v - 0x100000000l; if (v == 0l) break; } return i; }",
		"ifnzl       -",
		Bond::POINTER_64BIT);

	// Loops that keep their goto: a counting expression, a tail continue, an if with an else, and a
	// break that is not last.
	ASSERT_DISASSEMBLY_CONTAINS(
		"int F(int n) { int i = 0; for (;; ++i) { if (i >= n) break; } return i; }",
		"goto");

	ASSERT_DISASSEMBLY_CONTAINS(
		"int F(int n) { int i = 0; while (true) { ++i; if (i >= n) break; if (i == 3) continue; } return i; }",
		"goto");

	ASSERT_DISASSEMBLY_CONTAINS(
		"int F(int n) { int i = 0; int j = 0; while (true) { ++i; if (i >= n) { break; } else { ++j; } } return i + j; }",
		"goto");

	ASSERT_DISASSEMBLY_CONTAINS(
		"int F(int n) { int i = 0; while (true) { if (i >= n) break; ++i; } return i; }",
		"goto");

	// A loop with a non-constant condition is already inverted, and its tail break stays a branch to
	// the end.
	ASSERT_DISASSEMBLY_CONTAINS(
		"int F(int n) { int i = 0; while (i < 100) { ++i; if (i >= n) break; } return i; }",
		"ifgei");
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


// When control can't reach the end of a then-branch, no goto jumps over the else-branch. When it
// can't reach the end of a void function body, no trailing return is emitted.
DEFINE_TEST(DeadJumpElimination)
{
	ASSERT_DISASSEMBLY_CONTAINS(
		"int F(bool c) { if (c) return 1; else return 2; }",
		"\tcode size: 10\n"
		"     0: pushuc      -8\n"
		"     3: ifzi        2 (8)\n"
		"     6: consti_1    \n"
		"     7: return32    \n"
		"     8: consti_2    \n"
		"     9: return32    \n");

	ASSERT_DISASSEMBLY_CONTAINS(
		"void F(bool c) { if (c) { return; } else { return; } }",
		"\tcode size: 8\n"
		"     0: pushuc      -8\n"
		"     3: ifzi        1 (7)\n"
		"     6: return      \n"
		"     7: return      \n");

	ASSERT_DISASSEMBLY_CONTAINS_P(
		"void F(int *p, int x) { if (x == 1) { *p = 1; return; } else if (x == 2) { *p = 2; return; } else { return; } }",
		"\tcode size: 19\n"
		"     0: push32_p1   \n"
		"     1: consti_1    \n"
		"     2: ifnei       4 (9)\n"
		"     5: push64_p0   \n"
		"     6: consti_1    \n"
		"     7: store32     \n"
		"     8: return      \n"
		"     9: push32_p1   \n",
		Bond::POINTER_64BIT);

	// A switch whose sections all return, with a default, ends the function.
	ASSERT_DISASSEMBLY_CONTAINS_P(
		"void F(int *p, int x) { switch (x) { case 1: *p = 1; return; default: return; } }",
		"\tcode size: 25\n",
		Bond::POINTER_64BIT);

	// A then-branch that doesn't end in a jump still jumps over the else-branch.
	ASSERT_DISASSEMBLY_CONTAINS(
		"int F(bool c) { int x = 0; if (c) { x = 1; } else { return 2; } return x; }",
		"goto");

	// A break in a switch lets control reach the end of the function.
	ASSERT_DISASSEMBLY_CONTAINS(
		"void F(int x) { switch (x) { case 1: if (x == 1) break; return; default: return; } }",
		"    22: ifeqi       2 (27)\n"
		"    25: return      \n"
		"    26: return      \n"
		"    27: return      \n");

	// So do continue in a do loop and the end of a while loop.
	ASSERT_DISASSEMBLY_CONTAINS(
		"void F(bool c, bool d) { do { if (c) continue; return; } while (d); }",
		"    10: ifnzi       -13 (0)\n"
		"    13: return      \n");

	ASSERT_DISASSEMBLY_CONTAINS(
		"void F(bool c) { while (c) { } }",
		"     6: ifnzi       -6 (3)\n"
		"     9: return      \n");
}


// Where only the truth value matters, a comparison of a 32-bit value against zero tests the value
// directly.
DEFINE_TEST(TruthTestAgainstZero)
{
	ASSERT_DISASSEMBLY_CONTAINS(
		"int F(int x) { if (x != 0) { return 1; } return 2; }",
		"     0: push32_p0   \n"
		"     1: ifzi        ");

	ASSERT_DISASSEMBLY_CONTAINS(
		"int F(int x) { if (x == 0) { return 1; } return 2; }",
		"     0: push32_p0   \n"
		"     1: ifnzi       ");

	ASSERT_DISASSEMBLY_CONTAINS(
		"int F(int x, bool y) { return ((x != 0) && y) ? 1 : 2; }",
		"     0: push32_p0   \n"
		"     1: ifzi        ");

	ASSERT_DISASSEMBLY_NOT_CONTAINS_P(
		"int F(int *p) { if (p != null) { return 1; } return 2; }",
		"cmpneqi",
		Bond::POINTER_32BIT);

	// The comparison remains where the result is used as a value, for floating point operands, and
	// for comparisons against true. Longs and 64-bit pointers are covered by WideTruthTest.
	ASSERT_DISASSEMBLY_CONTAINS(
		"bool F(int x, bool y) { bool b = (x != 0) || y; return b; }",
		"cmpneqi");

	ASSERT_DISASSEMBLY_CONTAINS(
		"bool F(int x) { return x != 0; }",
		"cmpneqi");

	ASSERT_DISASSEMBLY_CONTAINS(
		"int F(float f) { if (f != 0.0f) { return 1; } return 2; }",
		"cmpneqf");

	// A comparison against true is kept, fused into its branch.
	ASSERT_DISASSEMBLY_CONTAINS(
		"int F(bool b) { if (b == true) { return 1; } return 2; }",
		"     3: consti_1    \n"
		"     4: ifnei       ");
}


// A comparison of a long or 64-bit pointer against zero that is itself a condition branches on the
// value with ifzl or ifnzl. Elsewhere, 'x != 0' becomes tstl, and so does 'x == 0' where its
// negation can be folded into the consumer.
DEFINE_TEST(WideTruthTest)
{
	// Conditions.
	ASSERT_DISASSEMBLY_CONTAINS_P(
		"int F(int *p) { if (p != null) { return 1; } return 2; }",
		"     0: push64_p0   \n"
		"     1: ifzl        ",
		Bond::POINTER_64BIT);

	ASSERT_DISASSEMBLY_CONTAINS_P(
		"int F(int *p) { if (p != null) { return 1; } return 2; }",
		"     0: push32_p0   \n"
		"     1: ifzi        ",
		Bond::POINTER_32BIT);

	ASSERT_DISASSEMBLY_CONTAINS(
		"int F(long l) { if (l != 0l) { return 1; } return 2; }",
		"     0: push64_p0   \n"
		"     1: ifzl        ");

	ASSERT_DISASSEMBLY_CONTAINS(
		"int F(long l) { if (l == 0l) { return 1; } return 2; }",
		"     0: push64_p0   \n"
		"     1: ifnzl       ");

	ASSERT_DISASSEMBLY_CONTAINS(
		"int F(long l) { if (0l != l) { return 1; } return 2; }",
		"     0: push64_p0   \n"
		"     1: ifzl        ");

	ASSERT_DISASSEMBLY_CONTAINS(
		"int F(ulong u) { if (u != 0ul) { return 1; } return 2; }",
		"     0: push64_p0   \n"
		"     1: ifzl        ");

	ASSERT_DISASSEMBLY_CONTAINS(
		"int F(long l) { if (!(l == 0l)) { return 1; } return 2; }",
		"     0: push64_p0   \n"
		"     1: ifzl        ");

	ASSERT_DISASSEMBLY_CONTAINS_P(
		"struct N { N *next; }; int F(N *p) { int n = 0; while (p != null) { p = p->next; ++n; } return n; }",
		"ifnzl       ",
		Bond::POINTER_64BIT);

	ASSERT_DISASSEMBLY_CONTAINS_P(
		"int F(int *p, int *q) { if ((p != null) && (q == null)) { return 1; } return 2; }",
		"     0: push64_p0   \n"
		"     1: ifzl        6 (10)\n"
		"     4: push64_p1   \n"
		"     5: ifnzl       2 (10)\n",
		Bond::POINTER_64BIT);

	ASSERT_DISASSEMBLY_CONTAINS(
		"int F(long a, long b) { if ((a == 0l) || (b != 0l)) { return 1; } return 2; }",
		"     0: push64_p0   \n"
		"     1: ifzl        4 (8)\n"
		"     4: push64_p1   \n"
		"     5: ifzl        ");

	ASSERT_DISASSEMBLY_CONTAINS(
		"int F(long l) { return (l != 0l) ? 1 : 2; }",
		"     0: push64_p0   \n"
		"     1: ifzl        ");

	// Value contexts use tstl, which yields an exact bool.
	ASSERT_DISASSEMBLY_CONTAINS_P(
		"bool F(int *p) { bool b = p != null; return b; }",
		"     0: push64_p0   \n"
		"     1: tstl        \n",
		Bond::POINTER_64BIT);

	ASSERT_DISASSEMBLY_CONTAINS(
		"bool F(long l) { return l != 0l; }",
		"     0: push64_p0   \n"
		"     1: tstl        \n"
		"     2: return32    \n");

	ASSERT_DISASSEMBLY_CONTAINS(
		"bool F(long l) { return !(l == 0l); }",
		"     0: push64_p0   \n"
		"     1: tstl        \n"
		"     2: return32    \n");

	ASSERT_DISASSEMBLY_CONTAINS(
		"bool F(long l, bool y) { return (l == 0l) || y; }",
		"     0: push64_p0   \n"
		"     1: tstl        \n"
		"     2: nbrnz       ");

	ASSERT_DISASSEMBLY_CONTAINS(
		"bool F(long l, bool y) { return (l != 0l) && y; }",
		"     0: push64_p0   \n"
		"     1: tstl        \n"
		"     2: brz         ");

	// 'x == 0' with nothing to fold the negation into keeps the comparison.
	ASSERT_DISASSEMBLY_CONTAINS(
		"bool F(long l) { return l == 0l; }",
		"cmpeql");

	ASSERT_DISASSEMBLY_NOT_CONTAINS(
		"bool F(long l) { return l == 0l; }",
		"tstl");

	// No other comparison is affected.
	ASSERT_DISASSEMBLY_CONTAINS(
		"int F(long l) { if (l != 1l) { return 1; } return 2; }",
		"cmpneql");

	ASSERT_DISASSEMBLY_CONTAINS(
		"int F(double d) { if (d != 0.0) { return 1; } return 2; }",
		"cmpneqd");

	ASSERT_DISASSEMBLY_CONTAINS(
		"int F(float f) { return (f != 0.0f) ? 1 : 2; }",
		"cmpneqf");

	ASSERT_DISASSEMBLY_NOT_CONTAINS(
		"int F(int x) { if (x != 0) { return 1; } return 2; }",
		"tstl");
}


// A condition that is a signed int comparison, alone or as an operand of !, && or ||, compiles to a
// single compare-and-branch instruction. 'if' branches when the relation is false, loops when it is
// true.
DEFINE_TEST(FusedCompareBranch)
{
	const char *const relations[][4] =
	{
		{"==", "ifnei", "ifeqi"},
		{"!=", "ifeqi", "ifnei"},
		{"<", "ifgei", "iflti"},
		{">=", "iflti", "ifgei"},
		{">", "iflei", "ifgti"},
		{"<=", "ifgti", "iflei"},
	};
	for (const auto &relation: relations)
	{
		char source[256];
		char expected[64];
		snprintf(source, sizeof(source), "int F(int a, int b) { if (a %s b) { return 1; } return 2; }", relation[0]);
		snprintf(expected, sizeof(expected), "     2: %-12s", relation[1]);
		ASSERT_DISASSEMBLY_CONTAINS(source, expected);

		snprintf(source, sizeof(source), "int F(int a, int b) { int n = 0; while (a %s b) { ++n; --a; } return n; }", relation[0]);
		snprintf(expected, sizeof(expected), "%-12s", relation[2]);
		ASSERT_DISASSEMBLY_CONTAINS(source, expected);

		snprintf(source, sizeof(source), "int F(int a, int b) { int n = 0; do { ++n; --a; } while (a %s b); return n; }", relation[0]);
		ASSERT_DISASSEMBLY_CONTAINS(source, expected);

		snprintf(source, sizeof(source), "int F(int a, int b) { return (a %s b) ? 1 : 2; }", relation[0]);
		snprintf(expected, sizeof(expected), "     2: %-12s", relation[1]);
		ASSERT_DISASSEMBLY_CONTAINS(source, expected);
	}

	// !, && and || are compiled into branches, so that each comparison fuses.
	ASSERT_DISASSEMBLY_CONTAINS(
		"int F(int a, int b, int c) { if (!(a < b)) { return 1; } return 2; }",
		"     2: iflti       ");

	ASSERT_DISASSEMBLY_CONTAINS(
		"int F(int a, int b, int c) { if ((a < b) && (b < c)) { return 1; } return 2; }",
		"     0: push32_p0   \n"
		"     1: push32_p1   \n"
		"     2: ifgei       7 (12)\n"
		"     5: push32_p1   \n"
		"     6: push32_p2   \n"
		"     7: ifgei       2 (12)\n");

	ASSERT_DISASSEMBLY_CONTAINS(
		"int F(int a, int b, int c) { if ((a < b) || (b < c)) { return 1; } return 2; }",
		"     0: push32_p0   \n"
		"     1: push32_p1   \n"
		"     2: iflti       5 (10)\n"
		"     5: push32_p1   \n"
		"     6: push32_p2   \n"
		"     7: ifgei       2 (12)\n");

	ASSERT_DISASSEMBLY_CONTAINS(
		"int F(int a, int b, int c) { if (!((a < b) && !(b < c))) { return 1; } return 2; }",
		"     0: push32_p0   \n"
		"     1: push32_p1   \n"
		"     2: ifgei       5 (10)\n"
		"     5: push32_p1   \n"
		"     6: push32_p2   \n"
		"     7: ifgei       2 (12)\n");

	// Operands promoted to int fuse, and so does unsigned equality.
	ASSERT_DISASSEMBLY_CONTAINS(
		"int F(char a, short b) { if (a < b) { return 1; } return 2; }",
		"ifgei");

	ASSERT_DISASSEMBLY_CONTAINS(
		"int F(uint a, uint b) { if (a == b) { return 1; } return 2; }",
		"ifnei");

	ASSERT_DISASSEMBLY_CONTAINS_P(
		"int F(int *p, int *q) { if (p == q) { return 1; } return 2; }",
		"ifnei",
		Bond::POINTER_32BIT);

	// A brz or a conditional expression may leave its value for the fused branch.
	ASSERT_DISASSEMBLY_CONTAINS(
		"int F(bool z, bool x, bool y) { if (z == (x && y)) { return 1; } return 2; }",
		"     6: brz         3 (12)\n"
		"     9: pushuc      -24\n"
		"    12: ifnei       ");

	ASSERT_DISASSEMBLY_CONTAINS(
		"int F(int z, bool p, int x, int y) { return (z < (p ? x : y)) ? 1 : 2; }",
		"     8: goto        1 (12)\n"
		"    11: push32_p3   \n"
		"    12: ifgei       ");

	// Unsigned relations, long, floating point and 64-bit pointer comparisons, tests against zero,
	// constant conditions and comparisons used as values don't fuse.
	const char *const unfused[] =
	{
		"int F(uint a, uint b) { if (a < b) { return 1; } return 2; }",
		"int F(long a, long b) { if (a < b) { return 1; } return 2; }",
		"int F(float a, float b) { if (a < b) { return 1; } return 2; }",
		"int F(double a, double b) { if (a == b) { return 1; } return 2; }",
		"int F(int a) { if (a != 0) { return 1; } return 2; }",
		"int F(int a) { const int k = 3; if (k < 4) { a += 1; } return a; }",
		"bool F(int a, int b) { bool v = a < b; return v; }",
	};
	for (const char *source: unfused)
	{
		ASSERT_DISASSEMBLY_NOT_CONTAINS(source, "ifeqi");
		ASSERT_DISASSEMBLY_NOT_CONTAINS(source, "ifnei");
		ASSERT_DISASSEMBLY_NOT_CONTAINS(source, "iflti");
		ASSERT_DISASSEMBLY_NOT_CONTAINS(source, "ifgei");
		ASSERT_DISASSEMBLY_NOT_CONTAINS(source, "ifgti");
		ASSERT_DISASSEMBLY_NOT_CONTAINS(source, "iflei");
	}

	ASSERT_DISASSEMBLY_NOT_CONTAINS_P(
		"int F(int *p, int *q) { if (p == q) { return 1; } return 2; }",
		"ifnei",
		Bond::POINTER_64BIT);
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


// A constant offset that fits in 16 bits is added to a pointer with a single ptraddc. Larger offsets
// push a constant and add it.
DEFINE_TEST(PointerAddConstant)
{
	const Bond::PointerSize POINTER_SIZES[] = { Bond::POINTER_32BIT, Bond::POINTER_64BIT };
	for (const Bond::PointerSize pointerSize: POINTER_SIZES)
	{
		// Fields through a pointer, through 'this' and of a global. 32-bit and 64-bit loads use
		// load32o and load64o instead (see OffsetLoads), so these use small types.
		ASSERT_DISASSEMBLY_CONTAINS_P(
			"struct S { int a; short b; };"
			"int F(S *p) { return p->b; }",
			"     1: ptraddc     4\n"
			"     4: loads       \n",
			pointerSize);

		ASSERT_DISASSEMBLY_CONTAINS_P(
			"struct S { int a; short b; int Get() const { return b; } };",
			"     1: ptraddc     4\n"
			"     4: loads       \n",
			pointerSize);

		ASSERT_DISASSEMBLY_CONTAINS_P(
			"struct S { int a; short b; };"
			"const S g = { 1, 2 };"
			"int F() { return g.b; }",
			"     0: loadea      g\n"
			"     3: ptraddc     4\n"
			"     6: loads       \n",
			pointerSize);

		// Constant subscripts, including negative ones.
		ASSERT_DISASSEMBLY_CONTAINS_P(
			"int F(short *p) { return p[3] + p[-2]; }",
			"     1: ptraddc     6\n",
			pointerSize);

		ASSERT_DISASSEMBLY_CONTAINS_P(
			"int F(short *p) { return p[3] + p[-2]; }",
			"     6: ptraddc     -4\n",
			pointerSize);

		// The 16-bit boundaries.
		ASSERT_DISASSEMBLY_CONTAINS_P(
			"int F(char *p) { return p[32767]; }",
			"ptraddc     32767\n",
			pointerSize);

		ASSERT_DISASSEMBLY_CONTAINS_P(
			"int F(char *p) { return p[-32768]; }",
			"ptraddc     -32768\n",
			pointerSize);

		ASSERT_DISASSEMBLY_NOT_CONTAINS_P(
			"int F(char *p) { return p[32768] + p[-32769]; }",
			"ptraddc",
			pointerSize);

		ASSERT_DISASSEMBLY_CONTAINS_P(
			"struct Big { char[32767] pad; char x; char y; };"
			"char F(Big *b) { return b->x; }",
			"ptraddc     32767\n",
			pointerSize);

		ASSERT_DISASSEMBLY_NOT_CONTAINS_P(
			"struct Big { char[32767] pad; char x; char y; };"
			"char F(Big *b) { return b->y; }",
			"ptraddc",
			pointerSize);

		// Pointer increments and compound assignments on a field.
		ASSERT_DISASSEMBLY_NOT_CONTAINS_P(
			"struct S { int a; int *ptr; };"
			"void F(S *s) { ++s->ptr; s->ptr -= 3; }",
			"addi",
			pointerSize);

		ASSERT_DISASSEMBLY_NOT_CONTAINS_P(
			"struct S { int a; int *ptr; };"
			"void F(S *s) { ++s->ptr; s->ptr -= 3; }",
			"addl",
			pointerSize);

		// A zero offset emits nothing.
		ASSERT_DISASSEMBLY_NOT_CONTAINS_P(
			"struct S { int a; int b; };"
			"int F(S *p, int *q) { return p->a + q[0]; }",
			"ptraddc",
			pointerSize);
	}

	// The fallback adds a constant of the pointer's width.
	ASSERT_DISASSEMBLY_CONTAINS_P(
		"int F(char *p) { return p[32768]; }",
		"     1: constus     32768\n"
		"     4: addi        \n",
		Bond::POINTER_32BIT);

	ASSERT_DISASSEMBLY_CONTAINS_P(
		"int F(char *p) { return p[32768]; }",
		"     1: const64     0x8000\n"
		"     4: addl        \n",
		Bond::POINTER_64BIT);

	ASSERT_DISASSEMBLY_CONTAINS_P(
		"struct S { int a; int *ptr; };"
		"void F(S *s) { ++s->ptr; s->ptr -= 3; }",
		"     0: push64_p0   \n"
		"     1: dup         \n"
		"     2: load64o     8\n"
		"     5: ptraddc     4\n"
		"     8: store64o    8\n",
		Bond::POINTER_64BIT);

	ASSERT_DISASSEMBLY_CONTAINS_P(
		"struct S { int a; int *ptr; };"
		"void F(S *s) { ++s->ptr; s->ptr -= 3; }",
		"ptraddc     -12\n",
		Bond::POINTER_64BIT);
}


// A 32-bit or 64-bit value at a nonzero constant offset that fits in 16 bits is stored with a single
// store32o or store64o. The offset stays out of the address until the store, so compound assignment
// and increments duplicate the base address and use load32o/load64o and store32o/store64o.
DEFINE_TEST(OffsetStores)
{
	const Bond::PointerSize POINTER_SIZES[] = { Bond::POINTER_32BIT, Bond::POINTER_64BIT };
	for (const Bond::PointerSize pointerSize: POINTER_SIZES)
	{
		// Every eligible type, through a pointer and through 'this'.
		ASSERT_DISASSEMBLY_CONTAINS_P(
			"struct S { int a; int b; void Set(int v) { b = v; } };",
			"     2: store32o    4\n",
			pointerSize);

		const char *const ALL_TYPES =
			"struct M { int a; int i; uint u; float f; long l; ulong ul; double d; };"
			"void F(M *p) { p->i = 1; p->u = 2u; p->f = 3.0f; p->l = 4l; p->ul = 5ul; p->d = 6.0; }";
		ASSERT_DISASSEMBLY_CONTAINS_P(ALL_TYPES, "store32o    4\n", pointerSize);
		ASSERT_DISASSEMBLY_CONTAINS_P(ALL_TYPES, "store32o    8\n", pointerSize);
		ASSERT_DISASSEMBLY_CONTAINS_P(ALL_TYPES, "store32o    12\n", pointerSize);
		ASSERT_DISASSEMBLY_CONTAINS_P(ALL_TYPES, "store64o    16\n", pointerSize);
		ASSERT_DISASSEMBLY_CONTAINS_P(ALL_TYPES, "store64o    24\n", pointerSize);
		ASSERT_DISASSEMBLY_CONTAINS_P(ALL_TYPES, "store64o    32\n", pointerSize);
		ASSERT_DISASSEMBLY_NOT_CONTAINS_P(ALL_TYPES, "ptraddc", pointerSize);

		// Constant subscripts, including negative ones, and the 16-bit boundaries.
		ASSERT_DISASSEMBLY_CONTAINS_P(
			"void F(int *p, int v) { p[3] = v; p[-2] = v; }",
			"     0: push32_p0   \n"
			"     1: push32_p1   \n"
			"     2: store32o    12\n",
			Bond::POINTER_32BIT);

		ASSERT_DISASSEMBLY_CONTAINS_P(
			"void F(int *p, int v) { p[3] = v; p[-2] = v; }",
			"store32o    -8\n",
			pointerSize);

		ASSERT_DISASSEMBLY_CONTAINS_P(
			"void F(int *p, int v) { p[8191] = v; p[-8192] = v; }",
			"store32o    32764\n",
			pointerSize);

		ASSERT_DISASSEMBLY_CONTAINS_P(
			"void F(int *p, int v) { p[8191] = v; p[-8192] = v; }",
			"store32o    -32768\n",
			pointerSize);

		ASSERT_DISASSEMBLY_CONTAINS_P(
			"void F(long *p, long v) { p[4095] = v; p[-4096] = v; }",
			"store64o    32760\n",
			pointerSize);

		ASSERT_DISASSEMBLY_CONTAINS_P(
			"void F(long *p, long v) { p[4095] = v; p[-4096] = v; }",
			"store64o    -32768\n",
			pointerSize);

		// Cases that keep a plain store: offsets out of range, a zero offset and small types.
		ASSERT_DISASSEMBLY_NOT_CONTAINS_P(
			"void F(int *p, int v) { p[8192] = v; p[-8193] = v; }",
			"store32o",
			pointerSize);

		ASSERT_DISASSEMBLY_NOT_CONTAINS_P(
			"struct Big { char[32768] pad; int x; };"
			"void F(Big *b) { b->x = 1; ++b->x; }",
			"store32o",
			pointerSize);

		ASSERT_DISASSEMBLY_NOT_CONTAINS_P(
			"struct S { int a; int b; };"
			"void F(S *p, int *q, int v) { p->a = v; *q = v; q[0] += v; }",
			"store32o",
			pointerSize);

		ASSERT_DISASSEMBLY_NOT_CONTAINS_P(
			"struct C { int a; char c; uchar uc; short s; ushort us; bool b; };"
			"void F(C *p) { p->c = cast<char>(1); p->uc = cast<uchar>(2); p->s = cast<short>(3); p->us = cast<ushort>(4); p->b = true; ++p->s; }",
			"store32o",
			pointerSize);
	}

	// Pointer-valued fields use the store of the pointer's width.
	ASSERT_DISASSEMBLY_CONTAINS_P(
		"struct R { int a; int *ptr; };"
		"void F(R *r, int *q) { r->ptr = q; }",
		"     2: store32o    4\n",
		Bond::POINTER_32BIT);

	ASSERT_DISASSEMBLY_CONTAINS_P(
		"struct R { int a; int *ptr; };"
		"void F(R *r, int *q) { r->ptr = q; }",
		"     2: store64o    8\n",
		Bond::POINTER_64BIT);

	// Value context: dupins copies the value below the address, then the store pops both.
	ASSERT_DISASSEMBLY_CONTAINS_P(
		"struct T { int a; int b; long c; };"
		"int F(T *p, int v) { return p->b = v; }",
		"     0: push64_p0   \n"
		"     1: push32_p1   \n"
		"     2: dupins      \n"
		"     3: store32o    4\n"
		"     6: return32    \n",
		Bond::POINTER_64BIT);

	// Compound assignment, prefix and postfix increments.
	ASSERT_DISASSEMBLY_CONTAINS_P(
		"struct T { int a; int b; long c; };"
		"void F(T *p) { p->b += 3; ++p->c; p->b = 7; }",
		"     0: push64_p0   \n"
		"     1: dup         \n"
		"     2: load32o     4\n"
		"     5: consti_3    \n"
		"     6: addi        \n"
		"     7: store32o    4\n"
		"    10: push64_p0   \n"
		"    11: dup         \n"
		"    12: load64o     8\n"
		"    15: constl_1    \n"
		"    16: addl        \n"
		"    17: store64o    8\n"
		"    20: push64_p0   \n"
		"    21: constc      7\n"
		"    23: store32o    4\n",
		Bond::POINTER_64BIT);

	ASSERT_DISASSEMBLY_CONTAINS_P(
		"struct T { int a; int b; long c; };"
		"long F(T *p) { return p->c++; }",
		"     0: push64_p0   \n"
		"     1: dup         \n"
		"     2: load64o     8\n"
		"     5: dupins      \n"
		"     6: constl_1    \n"
		"     7: addl        \n"
		"     8: store64o    8\n"
		"    11: return64    \n",
		Bond::POINTER_64BIT);
}


// A 32-bit or 64-bit value at a nonzero constant offset that fits in 16 bits is loaded with a single
// load32o or load64o. Other sizes, a zero offset and larger offsets keep ptraddc and a plain load.
DEFINE_TEST(OffsetLoads)
{
	const Bond::PointerSize POINTER_SIZES[] = { Bond::POINTER_32BIT, Bond::POINTER_64BIT };
	for (const Bond::PointerSize pointerSize: POINTER_SIZES)
	{
		// Fields through a pointer, through 'this', of a global and of a nested struct.
		ASSERT_DISASSEMBLY_CONTAINS_P(
			"struct S { int a; int b; };"
			"int F(S *p) { return p->b; }",
			"     1: load32o     4\n"
			"     4: return32    \n",
			pointerSize);

		ASSERT_DISASSEMBLY_CONTAINS_P(
			"struct S { int a; float b; float Get() const { return b; } };",
			"     1: load32o     4\n",
			pointerSize);

		ASSERT_DISASSEMBLY_CONTAINS_P(
			"struct S { int a; long b; };"
			"const S g = { 1, 2l };"
			"long F() { return g.b; }",
			"     0: loadea      g\n"
			"     3: load64o     8\n",
			pointerSize);

		ASSERT_DISASSEMBLY_CONTAINS_P(
			"struct In { int x; double d; };"
			"struct Out { int a; In in; };"
			"double F(Out *p) { return p->in.d; }",
			"     1: load64o     16\n",
			pointerSize);

		ASSERT_DISASSEMBLY_CONTAINS_P(
			"struct S { uint a; ulong b; uint c; };"
			"ulong F(S *p) { return p->b + p->c; }",
			"load32o     16\n",
			pointerSize);

		// Constant subscripts, including negative ones.
		ASSERT_DISASSEMBLY_CONTAINS_P(
			"int F(int *p) { return p[3] + p[-2]; }",
			"     1: load32o     12\n"
			"     4: push",
			pointerSize);

		ASSERT_DISASSEMBLY_CONTAINS_P(
			"int F(int *p) { return p[3] + p[-2]; }",
			"load32o     -8\n",
			pointerSize);

		// The 16-bit boundaries.
		ASSERT_DISASSEMBLY_CONTAINS_P(
			"int F(int *p) { return p[8191] + p[-8192]; }",
			"load32o     32764\n",
			pointerSize);

		ASSERT_DISASSEMBLY_CONTAINS_P(
			"int F(int *p) { return p[8191] + p[-8192]; }",
			"load32o     -32768\n",
			pointerSize);

		ASSERT_DISASSEMBLY_NOT_CONTAINS_P(
			"int F(int *p) { return p[8192] + p[-8193]; }",
			"load32o",
			pointerSize);

		ASSERT_DISASSEMBLY_NOT_CONTAINS_P(
			"struct Big { char[32768] pad; int x; };"
			"int F(Big *b) { return b->x; }",
			"load32o",
			pointerSize);

		// A zero offset uses a plain load.
		ASSERT_DISASSEMBLY_NOT_CONTAINS_P(
			"struct S { int a; long b; };"
			"long F(S *p, int *q) { return p->a + q[0]; }",
			"load32o",
			pointerSize);

		// Small types keep ptraddc.
		ASSERT_DISASSEMBLY_NOT_CONTAINS_P(
			"struct S { int a; char b; uchar c; short d; ushort e; bool f; };"
			"int F(S *p) { return p->b + p->c + p->d + p->e + (p->f ? 1 : 0); }",
			"load32o",
			pointerSize);

		// Compound assignment and increment duplicate the base address, then load and store at the
		// offset (see OffsetStores).
		ASSERT_DISASSEMBLY_NOT_CONTAINS_P(
			"struct S { int a; int b; long c; };"
			"void F(S *p) { p->b += 3; ++p->c; p->b = 7; }",
			"ptraddc",
			pointerSize);
	}

	// Pointer-valued fields use the load of the pointer's width.
	ASSERT_DISASSEMBLY_CONTAINS_P(
		"struct S { int a; int *p; };"
		"int *F(S *s) { return s->p; }",
		"     1: load32o     4\n",
		Bond::POINTER_32BIT);

	ASSERT_DISASSEMBLY_CONTAINS_P(
		"struct S { int a; int *p; };"
		"int *F(S *s) { return s->p; }",
		"     1: load64o     8\n",
		Bond::POINTER_64BIT);

	// Following a linked list.
	ASSERT_DISASSEMBLY_CONTAINS_P(
		"struct Node { int v; Node *next; };"
		"int F(Node *n) { int s = 0; while (n != null) { s += n->v; n = n->next; } return s; }",
		"load64o     8\n",
		Bond::POINTER_64BIT);
}


DEFINE_TEST(FarFrameOffsets)
{
	// A local beyond the int16 frame offset range is read with loadfpw and load. Its address is
	// pushed before the value is computed, so a write is loadfpw before the value and a store after
	// it, with no swap.
	ASSERT_DISASSEMBLY_CONTAINS(
		"int F(int a) { char[40000] pad = {}; int x = a; return x; }",
		"     6: loadfpw     40000\n"
		"     9: push32_p0   \n"
		"    10: store32     \n"
		"    11: loadfpw     40000\n"
		"    14: load32      \n");

	ASSERT_DISASSEMBLY_CONTAINS(
		"int F(int a) { char[40000] pad = {}; int x = 0; x = a; return x; }",
		"    11: loadfpw     40000\n"
		"    14: push32_p0   \n"
		"    15: store32     \n");

	// Compound assignment loads through a copy of the address.
	ASSERT_DISASSEMBLY_CONTAINS(
		"int F(int a) { char[40000] pad = {}; int x = 0; x += a; return x; }",
		"    11: loadfpw     40000\n"
		"    14: dup         \n"
		"    15: load32      \n"
		"    16: push32_p0   \n"
		"    17: addi        \n"
		"    18: store32     \n");

	// In value context, the value is kept below the address with dupins.
	ASSERT_DISASSEMBLY_CONTAINS(
		"int F(int a) { char[40000] pad = {}; int x = 0; return x = a; }",
		"    11: loadfpw     40000\n"
		"    14: push32_p0   \n"
		"    15: dupins      \n"
		"    16: store32     \n");

	// Struct members, increments and pointers.
	ASSERT_DISASSEMBLY_NOT_CONTAINS(
		"struct S { int a; long b; };"
		"long F(int a) { char[40000] pad = {}; S s = { a, 2l }; s.a = s.a + 5; s.b += 7l; s.a++; --s.b; return s.b; }",
		"swap");

	ASSERT_DISASSEMBLY_NOT_CONTAINS(
		"int F(int *q) { char[40000] pad = {}; int *p = q; p++; --p; p += 3; p = p - 1; return *p; }",
		"swap");

	ASSERT_DISASSEMBLY_NOT_CONTAINS(
		"char F(char c, short s) { char[40000] pad = {}; char x = c; short y = s; x += c; y = cast<short>(y * s); ++x; y--; return cast<char>(x + y); }",
		"swap");

	// A parameter beyond the int16 range, after a large struct passed by value.
	ASSERT_DISASSEMBLY_NOT_CONTAINS(
		"struct Huge { char[40000] pad; };"
		"int F(Huge h, int x) { x = x + 1000; x += 2; ++x; return x; }",
		"swap");

	ASSERT_DISASSEMBLY_CONTAINS(
		"struct Huge { char[40000] pad; };"
		"int F(Huge h, int x) { x = x + 1000; x += 2; ++x; return x; }",
		"loadfpw");
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
  TEST_ITEM(TailBreakLoops)                     \
  TEST_ITEM(AssignmentIncrement)                \
  TEST_ITEM(GotoToReturn)                       \
  TEST_ITEM(DeadJumpElimination)                \
  TEST_ITEM(TruthTestAgainstZero)               \
  TEST_ITEM(WideTruthTest)                      \
  TEST_ITEM(FusedCompareBranch)                 \
  TEST_ITEM(ReturnValueElision)                 \
  TEST_ITEM(PointerAddConstant)                 \
  TEST_ITEM(OffsetLoads)                        \
  TEST_ITEM(OffsetStores)                       \
  TEST_ITEM(FarFrameOffsets)                    \

RUN_TESTS(Disassembler, TEST_ITEMS)
