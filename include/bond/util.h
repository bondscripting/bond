#ifndef BOND_UTIL_H
#define BOND_UTIL_H

#include "bond/conf.h"

namespace Bond
{
namespace Util
{

bu32_t StringHash(const char *str, bu32_t seed = 0x811c9dc5);

}
}

#endif
