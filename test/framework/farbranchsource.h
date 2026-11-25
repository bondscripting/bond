#ifndef TEST_FRAMEWORK_FARBRANCHSOURCE_H
#define TEST_FRAMEWORK_FARBRANCHSOURCE_H

#include "bond/stl/vector.h"
#include "bond/types/types.h"

namespace TestFramework
{

typedef Bond::Vector<char> SourceBuffer;

/// \brief Number of terms in the padding expression of the functions written by
/// GenerateFarBranchSource. Each term compiles to two bytes of code, so the padding is about 34 KB,
/// more than a 16-bit branch offset can span.
const int32_t FAR_BRANCH_PAD_TERMS = 17000;

/// \brief Appends Bond source to the given buffer and null terminates it. Each function contains a
/// branch, of the kind named in the comment above it, over padding code that sums
/// FAR_BRANCH_PAD_TERMS copies of the parameter c, so the code generator must emit the branch as an
/// ext instruction. The functions and their results are:
///
/// - FarIfzi(a, c), FarIfnzi(a, c), FarIfzl(long a, c), FarIfnzl(long a, c), FarIfeqi(a, b, c),
///   FarIfnei(a, b, c), FarIflti(a, b, c), FarIfgei(a, b, c), FarIfgti(a, b, c), FarIflei(a, b, c):
///   the padding sum if a != 0, a == 0, a != 0, a == 0, a != b, a == b, a >= b, a < b, a <= b or
///   a > b respectively, otherwise 1.
/// - FarBrz(bool a, c), FarBrnz(bool a, c), FarNbrz(bool a, c), FarNbrnz(bool a, c): the bool
///   a && (sum != 0), a || (sum != 0), !a && (sum != 0) and !a || (sum != 0).
/// - FarGoto(a, c): 2 if a != 0, otherwise the padding sum.
/// - FarWhile(a, b, c): the padding sum times the number of iterations of while (a < b).
/// - FarBreak(n, c): the padding sum times n, from a while (true) loop that breaks at the top.
/// - FarDo(n, c): the padding sum times n for n >= 1, from a do-while loop.
void GenerateFarBranchSource(SourceBuffer &source);

}

#endif
