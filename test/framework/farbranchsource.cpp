#include "framework/farbranchsource.h"

namespace TestFramework
{

// Appends a sum of numTerms copies of c as a balanced tree, e.g. ((c+c)+(c+c)), so that the
// parser, the code generator and the operand stack only nest log2(numTerms) deep.
static void AppendBalancedSum(SourceBuffer &source, int32_t numTerms)
{
	if (numTerms == 1)
	{
		source.push_back('c');
	}
	else
	{
		const int32_t numLeftTerms = numTerms / 2;
		source.push_back('(');
		AppendBalancedSum(source, numLeftTerms);
		source.push_back('+');
		AppendBalancedSum(source, numTerms - numLeftTerms);
		source.push_back(')');
	}
}


void GenerateFarBranchSource(SourceBuffer &source)
{
	// Each '$' is replaced with the padding sum. The comments name the far branch that each
	// function is expected to contain.
	static const char *const FUNCTIONS[] =
	{
		// ifzi
		"int FarIfzi(int a, int c) { int x = 1; if (a != 0) { x = $; } return x; }\n",
		// ifnzi
		"int FarIfnzi(int a, int c) { int x = 1; if (a == 0) { x = $; } return x; }\n",
		// ifzl
		"int FarIfzl(long a, int c) { int x = 1; if (a != 0l) { x = $; } return x; }\n",
		// ifnzl
		"int FarIfnzl(long a, int c) { int x = 1; if (a == 0l) { x = $; } return x; }\n",
		// The fused branches jump over the padding when the condition is false.
		"int FarIfeqi(int a, int b, int c) { int x = 1; if (a != b) { x = $; } return x; }\n",
		"int FarIfnei(int a, int b, int c) { int x = 1; if (a == b) { x = $; } return x; }\n",
		"int FarIflti(int a, int b, int c) { int x = 1; if (a >= b) { x = $; } return x; }\n",
		"int FarIfgei(int a, int b, int c) { int x = 1; if (a < b) { x = $; } return x; }\n",
		"int FarIfgti(int a, int b, int c) { int x = 1; if (a <= b) { x = $; } return x; }\n",
		"int FarIflei(int a, int b, int c) { int x = 1; if (a > b) { x = $; } return x; }\n",
		// brz, brnz, nbrz and nbrnz, from && and || in value context.
		"bool FarBrz(bool a, int c) { bool r = a && ($ != 0); return r; }\n",
		"bool FarBrnz(bool a, int c) { bool r = a || ($ != 0); return r; }\n",
		"bool FarNbrz(bool a, int c) { bool r = !a && ($ != 0); return r; }\n",
		"bool FarNbrnz(bool a, int c) { bool r = !a || ($ != 0); return r; }\n",
		// goto, from the end of the then-branch over the else-branch.
		"int FarGoto(int a, int c) { int x = 1; if (a != 0) { x = 2; } else { x = $; } return x; }\n",
		// A forward goto to the condition and a backward iflti to the body.
		"int FarWhile(int a, int b, int c) { int x = 0; while (a < b) { x = x + $; a = a + 1; } return x; }\n",
		// A forward ifgei out of the loop and a backward goto to its start.
		"int FarBreak(int n, int c) { int x = 0; int i = 0; while (true) { if (i >= n) break; x = x + $; i = i + 1; } return x; }\n",
		// A backward ifnzi to the body.
		"int FarDo(int n, int c) { int x = 0; do { x = x + $; n = n - 1; } while (n != 0); return x; }\n",
	};

	for (const char *function: FUNCTIONS)
	{
		for (const char *ch = function; *ch != '\0'; ++ch)
		{
			if (*ch == '$')
			{
				AppendBalancedSum(source, FAR_BRANCH_PAD_TERMS);
			}
			else
			{
				source.push_back(*ch);
			}
		}
	}
	source.push_back('\0');
}

}
