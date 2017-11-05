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
	{"Bond.Asinh", Bond::Asinh},
	{"Bond.Asinhf", Bond::Asinhf},
	{"Bond.Acosh", Bond::Acosh},
	{"Bond.Acoshf", Bond::Acoshf},
	{"Bond.Atanh", Bond::Atanh},
	{"Bond.Atanhf", Bond::Atanhf},
	{"Bond.Exp", Bond::Exp},
	{"Bond.Expf", Bond::Expf},
	{"Bond.Exp2", Bond::Exp2},
	{"Bond.Exp2f", Bond::Exp2f},
	{"Bond.Log", Bond::Log},
	{"Bond.Logf", Bond::Logf},
	{"Bond.Log2", Bond::Log2},
	{"Bond.Log2f", Bond::Log2f},
	{"Bond.Log10", Bond::Log10},
	{"Bond.Log10f", Bond::Log10f},
	{"Bond.Pow", Bond::Pow},
	{"Bond.Powf", Bond::Powf},
	{"Bond.Sqrt", Bond::Sqrt},
	{"Bond.Sqrtf", Bond::Sqrtf},
	{"Bond.Cbrt", Bond::Cbrt},
	{"Bond.Cbrtf", Bond::Cbrtf},
	{"Bond.Hypot", Bond::Hypot},
	{"Bond.Hypotf", Bond::Hypotf},
	{"Bond.Ceil", Bond::Ceil},
	{"Bond.Ceilf", Bond::Ceilf},
	{"Bond.Floor", Bond::Floor},
	{"Bond.Floorf", Bond::Floorf},
	{"Bond.Trunc", Bond::Trunc},
	{"Bond.Truncf", Bond::Truncf},
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
	{"Bond.CopySign", Bond::CopySign},
	{"Bond.CopySignf", Bond::CopySignf},
	{"Bond.NaN", Bond::NaN},
	{"Bond.NaNf", Bond::NaNf},
	{nullptr, nullptr}
};

const Bond::NativeBindingCollection MATH_BINDING_COLLECTION =
{
	MATH_BINDING_COLLECTION_FUNCTIONS,
	64
};

}
