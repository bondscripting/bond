#ifndef BOND_PRIVATE_TYPETRAITS_H
#define BOND_PRIVATE_TYPETRAITS_H

#include <type_traits>

namespace Bond
{

using std::enable_if;
using std::has_virtual_destructor;
using std::is_arithmetic;
using std::is_class_v;
using std::is_enum_v;
using std::is_integral;
using std::is_integral_v;
using std::is_null_pointer_v;
using std::is_pointer_v;
using std::is_same;
using std::is_same_v;
using std::is_trivially_copyable_v;
using std::is_union_v;
using std::underlying_type_t;

}

#endif
