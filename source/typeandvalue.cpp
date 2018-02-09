#include "bond/compiler/parsenodes.h"
#include "bond/compiler/parsenodeutil.h"
#include "bond/compiler/typeandvalue.h"

namespace Bond
{

bool TypeAndValue::IsTypeDefined() const { return (mTypeDescriptor != nullptr) && mTypeDescriptor->IsDefined(); }


int64_t TypeAndValue::AsLongValue() const
{
	const TypeDescriptor longTypeDescriptor = TypeDescriptor::GetLongType();
	return CastValue(*this, longTypeDescriptor).mLong;
}


uint64_t TypeAndValue::AsULongValue() const
{
	const TypeDescriptor ulongTypeDescriptor = TypeDescriptor::GetULongType();
	return CastValue(*this, ulongTypeDescriptor).mULong;
}

}
