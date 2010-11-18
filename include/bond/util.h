#ifndef BOND_UTIL_H
#define BOND_UTIL_H

#include "bond/conf.h"

namespace Bond
{
namespace Util
{

const bu32_t STRING_HASH_SEED = static_cast<bu32_t>(0x811c9dc5u);

bu32_t StringHash(const char *str, bu32_t seed = STRING_HASH_SEED);

}
}

#endif
