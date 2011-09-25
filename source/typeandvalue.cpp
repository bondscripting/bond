#include "bond/parsenodes.h"
#include "bond/typeandvalue.h"

namespace Bond
{

bool TypeAndValue::IsTypeDefined() const { return (mTypeDescriptor != 0) && mTypeDescriptor->IsDefined(); }

}
