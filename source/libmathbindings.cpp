#include "bond/api/libmath.h"

namespace Bond
{

const Bond::NativeFunctionBinding MATH_BINDING_COLLECTION_FUNCTIONS[] =
{
	{0xde43ac9c, Bond::Sin},
	{0x8088b9f2, Bond::Sinf},
	{0xf81cc603, Bond::Cos},
	{0x984bb6df, Bond::Cosf},
	{0xf63d169b, Bond::Tan},
	{0x3d2a9667, Bond::Tanf},
	{0x6b2c227f, Bond::Asin},
	{0x357a4d8b, Bond::Asinf},
	{0x8d03b554, Bond::Acos},
	{0x50d6735a, Bond::Acosf},
	{0x631f49e8, Bond::Atan},
	{0xf241585e, Bond::Atanf},
	{0xf241580a, Bond::Atan2},
	{0x66dd97d8, Bond::Atan2f},
	{0x8088b9fc, Bond::Sinh},
	{0x533cc7d2, Bond::Sinhf},
	{0x984bb6d1, Bond::Cosh},
	{0x9030cb65, Bond::Coshf},
	{0x3d2a9669, Bond::Tanh},
	{0xb30ac72d, Bond::Tanhf},
	{0xe3182843, Bond::Exp},
	{0xc207611f, Bond::Expf},
	{0xf8299204, Bond::Log},
	{0xad70dc2a, Bond::Logf},
	{0x85ab18f7, Bond::Log10},
	{0x63584cb3, Bond::Log10f},
	{0xd8477560, Bond::Pow},
	{0xd87dc646, Bond::Powf},
	{0x74c4d0ac, Bond::Sqrt},
	{0x7dd47ea2, Bond::Sqrtf},
	{0x9a6551a9, Bond::Ceil},
	{0xb67f8d6d, Bond::Ceilf},
	{0xfb801fe8, Bond::Floor},
	{0xd2b23a5e, Bond::Floorf},
	{0x87b5e790, Bond::Fabs},
	{0x335b87d6, Bond::Fabsf},
	{0x0c985adb, Bond::Ldexp},
	{0xaed706a7, Bond::Ldexpf},
	{0x746e1dd3, Bond::Frexp},
	{0x1c58f34f, Bond::Frexpf},
	{0x4af968dc, Bond::Modf},
	{0xe2a01232, Bond::Modff},
	{0x92d27b2c, Bond::Fmod},
	{0x4d57e622, Bond::Fmodf},
	{0, NULL}
};

const Bond::NativeBindingCollection MATH_BINDING_COLLECTION =
{
	MATH_BINDING_COLLECTION_FUNCTIONS,
	44
};

}
