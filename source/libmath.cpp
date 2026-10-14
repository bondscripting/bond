#include "bond/api/libmath.h"
#include "bond/vm/vm.h"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <limits>

namespace Bond
{

void SinD(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<double>(0);
	const auto y = std::sin(x);
	frame.SetReturnValue(y);
}


void SinF(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<float>(0);
	const auto y = std::sin(x);
	frame.SetReturnValue(y);
}


void CosD(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<double>(0);
	const auto y = std::cos(x);
	frame.SetReturnValue(y);
}


void CosF(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<float>(0);
	const auto y = std::cos(x);
	frame.SetReturnValue(y);
}


void TanD(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<double>(0);
	const auto y = std::tan(x);
	frame.SetReturnValue(y);
}


void TanF(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<float>(0);
	const auto y = std::tan(x);
	frame.SetReturnValue(y);
}


void AsinD(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<double>(0);
	const auto y = std::asin(x);
	frame.SetReturnValue(y);
}


void AsinF(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<float>(0);
	const auto y = std::asin(x);
	frame.SetReturnValue(y);
}


void AcosD(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<double>(0);
	const auto y = std::acos(x);
	frame.SetReturnValue(y);
}


void AcosF(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<float>(0);
	const auto y = std::acos(x);
	frame.SetReturnValue(y);
}


void AtanD(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<double>(0);
	const auto y = std::atan(x);
	frame.SetReturnValue(y);
}


void AtanF(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<float>(0);
	const auto y = std::atan(x);
	frame.SetReturnValue(y);
}


void Atan2D(Bond::StackFrame &frame)
{
	const auto y = frame.GetArg<double>(0);
	const auto x = frame.GetArg<double>(1);
	const auto result = std::atan2(y, x);
	frame.SetReturnValue(result);
}


void Atan2F(Bond::StackFrame &frame)
{
	const auto y = frame.GetArg<float>(0);
	const auto x = frame.GetArg<float>(1);
	const auto result = std::atan2(y, x);
	frame.SetReturnValue(result);
}


void SinhD(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<double>(0);
	const auto y = std::sinh(x);
	frame.SetReturnValue(y);
}


void SinhF(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<float>(0);
	const auto y = std::sinh(x);
	frame.SetReturnValue(y);
}


void CoshD(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<double>(0);
	const auto y = std::cosh(x);
	frame.SetReturnValue(y);
}


void CoshF(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<float>(0);
	const auto y = std::cosh(x);
	frame.SetReturnValue(y);
}


void TanhD(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<double>(0);
	const auto y = std::tanh(x);
	frame.SetReturnValue(y);
}


void TanhF(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<float>(0);
	const auto y = std::tanh(x);
	frame.SetReturnValue(y);
}


void AsinhD(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<double>(0);
	const auto y = std::asinh(x);
	frame.SetReturnValue(y);
}


void AsinhF(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<float>(0);
	const auto y = std::asinh(x);
	frame.SetReturnValue(y);
}


void AcoshD(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<double>(0);
	const auto y = std::acosh(x);
	frame.SetReturnValue(y);
}


void AcoshF(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<float>(0);
	const auto y = std::acosh(x);
	frame.SetReturnValue(y);
}


void AtanhD(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<double>(0);
	const auto y = std::atanh(x);
	frame.SetReturnValue(y);
}


void AtanhF(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<float>(0);
	const auto y = std::atanh(x);
	frame.SetReturnValue(y);
}


void ExpD(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<double>(0);
	const auto y = std::exp(x);
	frame.SetReturnValue(y);
}


void ExpF(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<float>(0);
	const auto y = std::exp(x);
	frame.SetReturnValue(y);
}


void Exp2D(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<double>(0);
	const auto y = std::exp2(x);
	frame.SetReturnValue(y);
}


void Exp2F(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<float>(0);
	const auto y = std::exp2(x);
	frame.SetReturnValue(y);
}


void LogD(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<double>(0);
	const auto y = std::log(x);
	frame.SetReturnValue(y);
}


void LogF(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<float>(0);
	const auto y = std::log(x);
	frame.SetReturnValue(y);
}


void Log2D(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<double>(0);
	const auto y = std::log2(x);
	frame.SetReturnValue(y);
}


void Log2F(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<float>(0);
	const auto y = std::log2(x);
	frame.SetReturnValue(y);
}


void Log10D(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<double>(0);
	const auto y = std::log10(x);
	frame.SetReturnValue(y);
}


void Log10F(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<float>(0);
	const auto y = std::log10(x);
	frame.SetReturnValue(y);
}


void PowD(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<double>(0);
	const auto y = frame.GetArg<double>(1);
	const auto result = std::pow(x, y);
	frame.SetReturnValue(result);
}


void PowF(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<float>(0);
	const auto y = frame.GetArg<float>(1);
	const auto result = std::pow(x, y);
	frame.SetReturnValue(result);
}


void SqrtD(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<double>(0);
	const auto y = std::sqrt(x);
	frame.SetReturnValue(y);
}


void SqrtF(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<float>(0);
	const auto y = std::sqrt(x);
	frame.SetReturnValue(y);
}


void CbrtD(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<double>(0);
	const auto y = std::cbrt(x);
	frame.SetReturnValue(y);
}


void CbrtF(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<float>(0);
	const auto y = std::cbrt(x);
	frame.SetReturnValue(y);
}


void HypotD(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<double>(0);
	const auto y = frame.GetArg<double>(1);
	const auto h = std::hypot(x, y);
	frame.SetReturnValue(h);
}


void HypotF(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<float>(0);
	const auto y = frame.GetArg<float>(1);
	const auto h = std::hypot(x, y);
	frame.SetReturnValue(h);
}


void CeilD(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<double>(0);
	const auto y = std::ceil(x);
	frame.SetReturnValue(y);
}


void CeilF(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<float>(0);
	const auto y = std::ceil(x);
	frame.SetReturnValue(y);
}


void FloorD(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<double>(0);
	const auto y = std::floor(x);
	frame.SetReturnValue(y);
}


void FloorF(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<float>(0);
	const auto y = std::floor(x);
	frame.SetReturnValue(y);
}


void TruncD(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<double>(0);
	const auto y = std::trunc(x);
	frame.SetReturnValue(y);
}


void TruncF(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<float>(0);
	const auto y = std::trunc(x);
	frame.SetReturnValue(y);
}


void AbsC(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<int8_t>(0);
	const auto y = std::abs(x);
	frame.SetReturnValue(y);
}


void AbsS(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<int16_t>(0);
	const auto y = static_cast<int16_t>(std::abs(x));
	frame.SetReturnValue(y);
}


void AbsI(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<int32_t>(0);
	const auto y = std::abs(x);
	frame.SetReturnValue(y);
}


void AbsL(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<int64_t>(0);
	const auto y = std::abs(x);
	frame.SetReturnValue(y);
}


void AbsD(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<double>(0);
	const auto y = std::abs(x);
	frame.SetReturnValue(y);
}


void AbsF(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<float>(0);
	const auto y = std::abs(x);
	frame.SetReturnValue(y);
}


void LdexpD(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<double>(0);
	const int n = int(frame.GetArg<int32_t>(1));
	const auto result = std::ldexp(x, n);
	frame.SetReturnValue(result);
}


void LdexpF(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<float>(0);
	const int n = int(frame.GetArg<int32_t>(1));
	const auto result = std::ldexp(x, n);
	frame.SetReturnValue(result);
}


void FrexpD(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<double>(0);
	int exp;
	const auto result = std::frexp(x, &exp);
	frame.SetReturnValue(result);
	*frame.GetArg<int32_t *>(1) = int32_t(exp);
}


void FrexpF(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<float>(0);
	int exp;
	const auto result = std::frexp(x, &exp);
	frame.SetReturnValue(result);
	*frame.GetArg<int32_t *>(1) = int32_t(exp);
}


void ModfD(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<double>(0);
	double ip;
	const auto fp = std::modf(x, &ip);
	frame.SetReturnValue(fp);
	*frame.GetArg<double *>(1) = ip;
}


void ModfF(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<float>(0);
	float ip;
	const auto fp = std::modf(x, &ip);
	frame.SetReturnValue(fp);
	*frame.GetArg<float *>(1) = ip;
}


void FmodD(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<double>(0);
	const auto y = frame.GetArg<double>(1);
	const auto result = std::fmod(x, y);
	frame.SetReturnValue(result);
}


void FmodF(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<float>(0);
	const auto y = frame.GetArg<float>(1);
	const auto result = std::fmod(x, y);
	frame.SetReturnValue(result);
}

void CopySignD(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<double>(0);
	const auto y = frame.GetArg<double>(1);
	const auto result = std::copysign(x, y);
	frame.SetReturnValue(result);
}


void CopySignF(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<float>(0);
	const auto y = frame.GetArg<float>(1);
	const auto result = std::copysign(x, y);
	frame.SetReturnValue(result);
}

void NaND(Bond::StackFrame &frame)
{
	const auto result = std::nan("");
	frame.SetReturnValue(result);
}


void NaNF(Bond::StackFrame &frame)
{
	const auto result = std::nanf("");
	frame.SetReturnValue(result);
}


void InfD(Bond::StackFrame &frame)
{
	const auto result = std::numeric_limits<double>::infinity();
	frame.SetReturnValue(result);
}


void InfF(Bond::StackFrame &frame)
{
	const auto result = std::numeric_limits<float>::infinity();
	frame.SetReturnValue(result);
}


void IsNaND(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<double>(0);
	const bool result = std::isnan(x);
	frame.SetReturnValue(result);
}


void IsNaNF(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<float>(0);
	const bool result = std::isnan(x);
	frame.SetReturnValue(result);
}


void IsInfD(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<double>(0);
	const bool result = std::isinf(x);
	frame.SetReturnValue(result);
}


void IsInfF(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<float>(0);
	const bool result = std::isinf(x);
	frame.SetReturnValue(result);
}


void IsFiniteD(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<double>(0);
	const bool result = std::isfinite(x);
	frame.SetReturnValue(result);
}


void IsFiniteF(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<float>(0);
	const bool result = std::isfinite(x);
	frame.SetReturnValue(result);
}


void SignBitD(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<double>(0);
	const bool result = std::signbit(x);
	frame.SetReturnValue(result);
}


void SignBitF(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<float>(0);
	const bool result = std::signbit(x);
	frame.SetReturnValue(result);
}


void RoundD(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<double>(0);
	const auto y = std::round(x);
	frame.SetReturnValue(y);
}


void RoundF(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<float>(0);
	const auto y = std::round(x);
	frame.SetReturnValue(y);
}


void RintD(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<double>(0);
	const auto y = std::rint(x);
	frame.SetReturnValue(y);
}


void RintF(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<float>(0);
	const auto y = std::rint(x);
	frame.SetReturnValue(y);
}


void NearbyintD(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<double>(0);
	const auto y = std::nearbyint(x);
	frame.SetReturnValue(y);
}


void NearbyintF(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<float>(0);
	const auto y = std::nearbyint(x);
	frame.SetReturnValue(y);
}


void RemainderD(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<double>(0);
	const auto y = frame.GetArg<double>(1);
	const auto result = std::remainder(x, y);
	frame.SetReturnValue(result);
}


void RemainderF(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<float>(0);
	const auto y = frame.GetArg<float>(1);
	const auto result = std::remainder(x, y);
	frame.SetReturnValue(result);
}


void LroundD(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<double>(0);
	const auto result = int64_t(std::llround(x));
	frame.SetReturnValue(result);
}


void LroundF(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<float>(0);
	const auto result = int64_t(std::llround(x));
	frame.SetReturnValue(result);
}


void Log1pD(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<double>(0);
	const auto y = std::log1p(x);
	frame.SetReturnValue(y);
}


void Log1pF(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<float>(0);
	const auto y = std::log1p(x);
	frame.SetReturnValue(y);
}


void Expm1D(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<double>(0);
	const auto y = std::expm1(x);
	frame.SetReturnValue(y);
}


void Expm1F(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<float>(0);
	const auto y = std::expm1(x);
	frame.SetReturnValue(y);
}


void ErfD(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<double>(0);
	const auto y = std::erf(x);
	frame.SetReturnValue(y);
}


void ErfF(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<float>(0);
	const auto y = std::erf(x);
	frame.SetReturnValue(y);
}


void ErfcD(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<double>(0);
	const auto y = std::erfc(x);
	frame.SetReturnValue(y);
}


void ErfcF(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<float>(0);
	const auto y = std::erfc(x);
	frame.SetReturnValue(y);
}


void TgammaD(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<double>(0);
	const auto y = std::tgamma(x);
	frame.SetReturnValue(y);
}


void TgammaF(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<float>(0);
	const auto y = std::tgamma(x);
	frame.SetReturnValue(y);
}


void LgammaD(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<double>(0);
	const auto y = std::lgamma(x);
	frame.SetReturnValue(y);
}


void LgammaF(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<float>(0);
	const auto y = std::lgamma(x);
	frame.SetReturnValue(y);
}


void FmaD(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<double>(0);
	const auto y = frame.GetArg<double>(1);
	const auto z = frame.GetArg<double>(2);
	const auto result = std::fma(x, y, z);
	frame.SetReturnValue(result);
}


void FmaF(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<float>(0);
	const auto y = frame.GetArg<float>(1);
	const auto z = frame.GetArg<float>(2);
	const auto result = std::fma(x, y, z);
	frame.SetReturnValue(result);
}


void NextafterD(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<double>(0);
	const auto y = frame.GetArg<double>(1);
	const auto result = std::nextafter(x, y);
	frame.SetReturnValue(result);
}


void NextafterF(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<float>(0);
	const auto y = frame.GetArg<float>(1);
	const auto result = std::nextafter(x, y);
	frame.SetReturnValue(result);
}


void ScalbnD(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<double>(0);
	const int n = int(frame.GetArg<int32_t>(1));
	const auto result = std::scalbn(x, n);
	frame.SetReturnValue(result);
}


void ScalbnF(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<float>(0);
	const int n = int(frame.GetArg<int32_t>(1));
	const auto result = std::scalbn(x, n);
	frame.SetReturnValue(result);
}


void IlogbD(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<double>(0);
	const int32_t result = std::ilogb(x);
	frame.SetReturnValue(result);
}


void IlogbF(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<float>(0);
	const int32_t result = std::ilogb(x);
	frame.SetReturnValue(result);
}


void LogbD(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<double>(0);
	const auto y = std::logb(x);
	frame.SetReturnValue(y);
}


void LogbF(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<float>(0);
	const auto y = std::logb(x);
	frame.SetReturnValue(y);
}


void RemquoD(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<double>(0);
	const auto y = frame.GetArg<double>(1);
	int quo;
	const auto result = std::remquo(x, y, &quo);
	frame.SetReturnValue(result);
	*frame.GetArg<int32_t *>(2) = int32_t(quo);
}


void RemquoF(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<float>(0);
	const auto y = frame.GetArg<float>(1);
	int quo;
	const auto result = std::remquo(x, y, &quo);
	frame.SetReturnValue(result);
	*frame.GetArg<int32_t *>(2) = int32_t(quo);
}


void MinC(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<int8_t>(0);
	const auto y = frame.GetArg<int8_t>(1);
	const auto result = std::min(x, y);
	frame.SetReturnValue(result);
}


void MinUC(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<uint8_t>(0);
	const auto y = frame.GetArg<uint8_t>(1);
	const auto result = std::min(x, y);
	frame.SetReturnValue(result);
}


void MinS(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<int16_t>(0);
	const auto y = frame.GetArg<int16_t>(1);
	const auto result = std::min(x, y);
	frame.SetReturnValue(result);
}


void MinUS(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<uint16_t>(0);
	const auto y = frame.GetArg<uint16_t>(1);
	const auto result = std::min(x, y);
	frame.SetReturnValue(result);
}


void MinI(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<int32_t>(0);
	const auto y = frame.GetArg<int32_t>(1);
	const auto result = std::min(x, y);
	frame.SetReturnValue(result);
}


void MinUI(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<uint32_t>(0);
	const auto y = frame.GetArg<uint32_t>(1);
	const auto result = std::min(x, y);
	frame.SetReturnValue(result);
}


void MinL(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<int64_t>(0);
	const auto y = frame.GetArg<int64_t>(1);
	const auto result = std::min(x, y);
	frame.SetReturnValue(result);
}


void MinUL(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<uint64_t>(0);
	const auto y = frame.GetArg<uint64_t>(1);
	const auto result = std::min(x, y);
	frame.SetReturnValue(result);
}


void MinD(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<double>(0);
	const auto y = frame.GetArg<double>(1);
	const auto result = std::fmin(x, y);
	frame.SetReturnValue(result);
}


void MinF(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<float>(0);
	const auto y = frame.GetArg<float>(1);
	const auto result = std::fmin(x, y);
	frame.SetReturnValue(result);
}


void MaxC(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<int8_t>(0);
	const auto y = frame.GetArg<int8_t>(1);
	const auto result = std::max(x, y);
	frame.SetReturnValue(result);
}


void MaxUC(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<uint8_t>(0);
	const auto y = frame.GetArg<uint8_t>(1);
	const auto result = std::max(x, y);
	frame.SetReturnValue(result);
}


void MaxS(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<int16_t>(0);
	const auto y = frame.GetArg<int16_t>(1);
	const auto result = std::max(x, y);
	frame.SetReturnValue(result);
}


void MaxUS(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<uint16_t>(0);
	const auto y = frame.GetArg<uint16_t>(1);
	const auto result = std::max(x, y);
	frame.SetReturnValue(result);
}


void MaxI(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<int32_t>(0);
	const auto y = frame.GetArg<int32_t>(1);
	const auto result = std::max(x, y);
	frame.SetReturnValue(result);
}


void MaxUI(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<uint32_t>(0);
	const auto y = frame.GetArg<uint32_t>(1);
	const auto result = std::max(x, y);
	frame.SetReturnValue(result);
}


void MaxL(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<int64_t>(0);
	const auto y = frame.GetArg<int64_t>(1);
	const auto result = std::max(x, y);
	frame.SetReturnValue(result);
}


void MaxUL(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<uint64_t>(0);
	const auto y = frame.GetArg<uint64_t>(1);
	const auto result = std::max(x, y);
	frame.SetReturnValue(result);
}


void MaxD(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<double>(0);
	const auto y = frame.GetArg<double>(1);
	const auto result = std::fmax(x, y);
	frame.SetReturnValue(result);
}


void MaxF(Bond::StackFrame &frame)
{
	const auto x = frame.GetArg<float>(0);
	const auto y = frame.GetArg<float>(1);
	const auto result = std::fmax(x, y);
	frame.SetReturnValue(result);
}


void ClampC(Bond::StackFrame &frame)
{
	const auto value = frame.GetArg<int8_t>(0);
	const auto low = frame.GetArg<int8_t>(1);
	const auto high = frame.GetArg<int8_t>(2);
	const auto result = std::clamp(value, low, high);
	frame.SetReturnValue(result);
}


void ClampUC(Bond::StackFrame &frame)
{
	const auto value = frame.GetArg<uint8_t>(0);
	const auto low = frame.GetArg<uint8_t>(1);
	const auto high = frame.GetArg<uint8_t>(2);
	const auto result = std::clamp(value, low, high);
	frame.SetReturnValue(result);
}


void ClampS(Bond::StackFrame &frame)
{
	const auto value = frame.GetArg<int16_t>(0);
	const auto low = frame.GetArg<int16_t>(1);
	const auto high = frame.GetArg<int16_t>(2);
	const auto result = std::clamp(value, low, high);
	frame.SetReturnValue(result);
}


void ClampUS(Bond::StackFrame &frame)
{
	const auto value = frame.GetArg<uint16_t>(0);
	const auto low = frame.GetArg<uint16_t>(1);
	const auto high = frame.GetArg<uint16_t>(2);
	const auto result = std::clamp(value, low, high);
	frame.SetReturnValue(result);
}


void ClampI(Bond::StackFrame &frame)
{
	const auto value = frame.GetArg<int32_t>(0);
	const auto low = frame.GetArg<int32_t>(1);
	const auto high = frame.GetArg<int32_t>(2);
	const auto result = std::clamp(value, low, high);
	frame.SetReturnValue(result);
}


void ClampUI(Bond::StackFrame &frame)
{
	const auto value = frame.GetArg<uint32_t>(0);
	const auto low = frame.GetArg<uint32_t>(1);
	const auto high = frame.GetArg<uint32_t>(2);
	const auto result = std::clamp(value, low, high);
	frame.SetReturnValue(result);
}


void ClampL(Bond::StackFrame &frame)
{
	const auto value = frame.GetArg<int64_t>(0);
	const auto low = frame.GetArg<int64_t>(1);
	const auto high = frame.GetArg<int64_t>(2);
	const auto result = std::clamp(value, low, high);
	frame.SetReturnValue(result);
}


void ClampUL(Bond::StackFrame &frame)
{
	const auto value = frame.GetArg<uint64_t>(0);
	const auto low = frame.GetArg<uint64_t>(1);
	const auto high = frame.GetArg<uint64_t>(2);
	const auto result = std::clamp(value, low, high);
	frame.SetReturnValue(result);
}


void ClampD(Bond::StackFrame &frame)
{
	const auto value = frame.GetArg<double>(0);
	const auto low = frame.GetArg<double>(1);
	const auto high = frame.GetArg<double>(2);
	const auto result = std::clamp(value, low, high);
	frame.SetReturnValue(result);
}


void ClampF(Bond::StackFrame &frame)
{
	const auto value = frame.GetArg<float>(0);
	const auto low = frame.GetArg<float>(1);
	const auto high = frame.GetArg<float>(2);
	const auto result = std::clamp(value, low, high);
	frame.SetReturnValue(result);
}

}
