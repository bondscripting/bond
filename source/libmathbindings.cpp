#include "bond/api/libmath.h"

namespace Bond
{

const Bond::NativeFunctionBinding MATH_BINDING_COLLECTION_FUNCTIONS[] =
{
	{"Bond.Sin", Bond::Sin},
	{"Bond.Sinf", Bond::Sinf},
	{"Bond.Cos", Bond::Cos},
	{"Bond.Cosf", Bond::Cosf},
	{"Bond.Tan", Bond::Tan},
	{"Bond.Tanf", Bond::Tanf},
	{"Bond.Asin", Bond::Asin},
	{"Bond.Asinf", Bond::Asinf},
	{"Bond.Acos", Bond::Acos},
	{"Bond.Acosf", Bond::Acosf},
	{"Bond.Atan", Bond::Atan},
	{"Bond.Atanf", Bond::Atanf},
	{"Bond.Atan2", Bond::Atan2},
	{"Bond.Atan2f", Bond::Atan2f},
	{"Bond.Sinh", Bond::Sinh},
	{"Bond.Sinhf", Bond::Sinhf},
	{"Bond.Cosh", Bond::Cosh},
	{"Bond.Coshf", Bond::Coshf},
	{"Bond.Tanh", Bond::Tanh},
	{"Bond.Tanhf", Bond::Tanhf},
	{"Bond.Exp", Bond::Exp},
	{"Bond.Expf", Bond::Expf},
	{"Bond.Log", Bond::Log},
	{"Bond.Logf", Bond::Logf},
	{"Bond.Log10", Bond::Log10},
	{"Bond.Log10f", Bond::Log10f},
	{"Bond.Pow", Bond::Pow},
	{"Bond.Powf", Bond::Powf},
	{"Bond.Sqrt", Bond::Sqrt},
	{"Bond.Sqrtf", Bond::Sqrtf},
	{"Bond.Ceil", Bond::Ceil},
	{"Bond.Ceilf", Bond::Ceilf},
	{"Bond.Floor", Bond::Floor},
	{"Bond.Floorf", Bond::Floorf},
	{"Bond.Fabs", Bond::Fabs},
	{"Bond.Fabsf", Bond::Fabsf},
	{"Bond.Ldexp", Bond::Ldexp},
	{"Bond.Ldexpf", Bond::Ldexpf},
	{"Bond.Frexp", Bond::Frexp},
	{"Bond.Frexpf", Bond::Frexpf},
	{"Bond.Modf", Bond::Modf},
	{"Bond.Modff", Bond::Modff},
	{"Bond.Fmod", Bond::Fmod},
	{"Bond.Fmodf", Bond::Fmodf},
	{nullptr, nullptr}
};

const Bond::NativeBindingCollection MATH_BINDING_COLLECTION =
{
	MATH_BINDING_COLLECTION_FUNCTIONS,
	44
};

}
