#ifndef BOND_API_LIBMATH_H
#define BOND_API_LIBMATH_H

#include "bond/api/nativebinding.h"

#include "bond/conf.h"

namespace Bond
{
extern BOND_API const Bond::NativeBindingCollection MATH_BINDING_COLLECTION;
}

namespace Bond
{
BOND_API void SinD(Bond::StackFrame &frame);
BOND_API void SinF(Bond::StackFrame &frame);
BOND_API void CosD(Bond::StackFrame &frame);
BOND_API void CosF(Bond::StackFrame &frame);
BOND_API void TanD(Bond::StackFrame &frame);
BOND_API void TanF(Bond::StackFrame &frame);
BOND_API void AsinD(Bond::StackFrame &frame);
BOND_API void AsinF(Bond::StackFrame &frame);
BOND_API void AcosD(Bond::StackFrame &frame);
BOND_API void AcosF(Bond::StackFrame &frame);
BOND_API void AtanD(Bond::StackFrame &frame);
BOND_API void AtanF(Bond::StackFrame &frame);
BOND_API void Atan2D(Bond::StackFrame &frame);
BOND_API void Atan2F(Bond::StackFrame &frame);
BOND_API void SinhD(Bond::StackFrame &frame);
BOND_API void SinhF(Bond::StackFrame &frame);
BOND_API void CoshD(Bond::StackFrame &frame);
BOND_API void CoshF(Bond::StackFrame &frame);
BOND_API void TanhD(Bond::StackFrame &frame);
BOND_API void TanhF(Bond::StackFrame &frame);
BOND_API void AsinhD(Bond::StackFrame &frame);
BOND_API void AsinhF(Bond::StackFrame &frame);
BOND_API void AcoshD(Bond::StackFrame &frame);
BOND_API void AcoshF(Bond::StackFrame &frame);
BOND_API void AtanhD(Bond::StackFrame &frame);
BOND_API void AtanhF(Bond::StackFrame &frame);
BOND_API void ExpD(Bond::StackFrame &frame);
BOND_API void ExpF(Bond::StackFrame &frame);
BOND_API void Exp2D(Bond::StackFrame &frame);
BOND_API void Exp2F(Bond::StackFrame &frame);
BOND_API void LogD(Bond::StackFrame &frame);
BOND_API void LogF(Bond::StackFrame &frame);
BOND_API void Log2D(Bond::StackFrame &frame);
BOND_API void Log2F(Bond::StackFrame &frame);
BOND_API void Log10D(Bond::StackFrame &frame);
BOND_API void Log10F(Bond::StackFrame &frame);
BOND_API void PowD(Bond::StackFrame &frame);
BOND_API void PowF(Bond::StackFrame &frame);
BOND_API void SqrtD(Bond::StackFrame &frame);
BOND_API void SqrtF(Bond::StackFrame &frame);
BOND_API void CbrtD(Bond::StackFrame &frame);
BOND_API void CbrtF(Bond::StackFrame &frame);
BOND_API void HypotD(Bond::StackFrame &frame);
BOND_API void HypotF(Bond::StackFrame &frame);
BOND_API void CeilD(Bond::StackFrame &frame);
BOND_API void CeilF(Bond::StackFrame &frame);
BOND_API void FloorD(Bond::StackFrame &frame);
BOND_API void FloorF(Bond::StackFrame &frame);
BOND_API void TruncD(Bond::StackFrame &frame);
BOND_API void TruncF(Bond::StackFrame &frame);
BOND_API void AbsC(Bond::StackFrame &frame);
BOND_API void AbsS(Bond::StackFrame &frame);
BOND_API void AbsI(Bond::StackFrame &frame);
BOND_API void AbsL(Bond::StackFrame &frame);
BOND_API void AbsD(Bond::StackFrame &frame);
BOND_API void AbsF(Bond::StackFrame &frame);
BOND_API void LdexpD(Bond::StackFrame &frame);
BOND_API void LdexpF(Bond::StackFrame &frame);
BOND_API void FrexpD(Bond::StackFrame &frame);
BOND_API void FrexpF(Bond::StackFrame &frame);
BOND_API void ModfD(Bond::StackFrame &frame);
BOND_API void ModfF(Bond::StackFrame &frame);
BOND_API void FmodD(Bond::StackFrame &frame);
BOND_API void FmodF(Bond::StackFrame &frame);
BOND_API void CopySignD(Bond::StackFrame &frame);
BOND_API void CopySignF(Bond::StackFrame &frame);
BOND_API void NaND(Bond::StackFrame &frame);
BOND_API void NaNF(Bond::StackFrame &frame);
BOND_API void IsNaND(Bond::StackFrame &frame);
BOND_API void IsNaNF(Bond::StackFrame &frame);
BOND_API void IsInfD(Bond::StackFrame &frame);
BOND_API void IsInfF(Bond::StackFrame &frame);
BOND_API void IsFiniteD(Bond::StackFrame &frame);
BOND_API void IsFiniteF(Bond::StackFrame &frame);
BOND_API void SignBitD(Bond::StackFrame &frame);
BOND_API void SignBitF(Bond::StackFrame &frame);
BOND_API void RoundD(Bond::StackFrame &frame);
BOND_API void RoundF(Bond::StackFrame &frame);
BOND_API void RintD(Bond::StackFrame &frame);
BOND_API void RintF(Bond::StackFrame &frame);
BOND_API void NearbyintD(Bond::StackFrame &frame);
BOND_API void NearbyintF(Bond::StackFrame &frame);
BOND_API void RemainderD(Bond::StackFrame &frame);
BOND_API void RemainderF(Bond::StackFrame &frame);
BOND_API void LroundD(Bond::StackFrame &frame);
BOND_API void LroundF(Bond::StackFrame &frame);
BOND_API void Log1pD(Bond::StackFrame &frame);
BOND_API void Log1pF(Bond::StackFrame &frame);
BOND_API void Expm1D(Bond::StackFrame &frame);
BOND_API void Expm1F(Bond::StackFrame &frame);
BOND_API void ErfD(Bond::StackFrame &frame);
BOND_API void ErfF(Bond::StackFrame &frame);
BOND_API void ErfcD(Bond::StackFrame &frame);
BOND_API void ErfcF(Bond::StackFrame &frame);
BOND_API void TgammaD(Bond::StackFrame &frame);
BOND_API void TgammaF(Bond::StackFrame &frame);
BOND_API void LgammaD(Bond::StackFrame &frame);
BOND_API void LgammaF(Bond::StackFrame &frame);
BOND_API void FmaD(Bond::StackFrame &frame);
BOND_API void FmaF(Bond::StackFrame &frame);
BOND_API void NextafterD(Bond::StackFrame &frame);
BOND_API void NextafterF(Bond::StackFrame &frame);
BOND_API void ScalbnD(Bond::StackFrame &frame);
BOND_API void ScalbnF(Bond::StackFrame &frame);
BOND_API void IlogbD(Bond::StackFrame &frame);
BOND_API void IlogbF(Bond::StackFrame &frame);
BOND_API void LogbD(Bond::StackFrame &frame);
BOND_API void LogbF(Bond::StackFrame &frame);
BOND_API void RemquoD(Bond::StackFrame &frame);
BOND_API void RemquoF(Bond::StackFrame &frame);
BOND_API void InfD(Bond::StackFrame &frame);
BOND_API void InfF(Bond::StackFrame &frame);
BOND_API void MinC(Bond::StackFrame &frame);
BOND_API void MinUC(Bond::StackFrame &frame);
BOND_API void MinS(Bond::StackFrame &frame);
BOND_API void MinUS(Bond::StackFrame &frame);
BOND_API void MinI(Bond::StackFrame &frame);
BOND_API void MinUI(Bond::StackFrame &frame);
BOND_API void MinL(Bond::StackFrame &frame);
BOND_API void MinUL(Bond::StackFrame &frame);
BOND_API void MinD(Bond::StackFrame &frame);
BOND_API void MinF(Bond::StackFrame &frame);
BOND_API void MaxC(Bond::StackFrame &frame);
BOND_API void MaxUC(Bond::StackFrame &frame);
BOND_API void MaxS(Bond::StackFrame &frame);
BOND_API void MaxUS(Bond::StackFrame &frame);
BOND_API void MaxI(Bond::StackFrame &frame);
BOND_API void MaxUI(Bond::StackFrame &frame);
BOND_API void MaxL(Bond::StackFrame &frame);
BOND_API void MaxUL(Bond::StackFrame &frame);
BOND_API void MaxD(Bond::StackFrame &frame);
BOND_API void MaxF(Bond::StackFrame &frame);
BOND_API void ClampC(Bond::StackFrame &frame);
BOND_API void ClampUC(Bond::StackFrame &frame);
BOND_API void ClampS(Bond::StackFrame &frame);
BOND_API void ClampUS(Bond::StackFrame &frame);
BOND_API void ClampI(Bond::StackFrame &frame);
BOND_API void ClampUI(Bond::StackFrame &frame);
BOND_API void ClampL(Bond::StackFrame &frame);
BOND_API void ClampUL(Bond::StackFrame &frame);
BOND_API void ClampD(Bond::StackFrame &frame);
BOND_API void ClampF(Bond::StackFrame &frame);
}

#endif
