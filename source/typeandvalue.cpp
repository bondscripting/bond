#include "bond/parsenodes.h"
#include "bond/parsenodeutil.h"
#include "bond/typeandvalue.h"

namespace Bond
{

bool TypeAndValue::IsTypeDefined() const { return (mTypeDescriptor != NULL) && mTypeDescriptor->IsDefined(); }


bi64_t TypeAndValue::AsLongValue() const
{
	const TypeDescriptor longTypeDescriptor = TypeDescriptor::GetLongType();
	return CastValue(*this, &longTypeDescriptor).mLong;
}


bu64_t TypeAndValue::AsULongValue() const
{
	const TypeDescriptor ulongTypeDescriptor = TypeDescriptor::GetULongType();
	return CastValue(*this, &ulongTypeDescriptor).mULong;
}

}
