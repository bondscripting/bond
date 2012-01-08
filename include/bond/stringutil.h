#ifndef BOND_STRINGUTIL_H
#define BOND_STRINGUTIL_H

#include "bond/types.h"

namespace Bond
{

const bu32_t STRING_HASH_SEED = static_cast<bu32_t>(0x811c9dc5u);

bu32_t StringHash(const char *str, bu32_t seed = STRING_HASH_SEED);
bu32_t StringHash(int length, const char *str, bu32_t seed = STRING_HASH_SEED);

bool StringEqual(const char *a, const char *b, int length);

}

#endif
