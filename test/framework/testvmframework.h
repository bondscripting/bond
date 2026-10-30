#ifndef TEST_FRAMEWORK_TESTVMFRAMEWORK_H
#define TEST_FRAMEWORK_TESTVMFRAMEWORK_H

#include "framework/testframework.h"
#include "bond/systems/math.h"
#include "bond/vm/vm.h"
#include <cmath>
#include <type_traits>

#define DEFINE_VM_TEST(testName, scriptName)                                                   \
  DEFINE_VM_TEST_WITH_BINDING(testName, scriptName, nullptr)

#define DEFINE_VM_TEST_WITH_BINDING(testName, scriptName, nativeBinding)                       \
  void __Validate ## testName ## __(Bond::VM &vm);                                             \
                                                                                               \
  void __Test ## testName ## __()                                                             \
  {                                                                                            \
    TestFramework::RunVMTest(                                                                  \
      __FILE__, __LINE__, scriptName, &__Validate ## testName ## __, nativeBinding);           \
  }                                                                                            \
                                                                                               \
  void __Validate ## testName ## __(Bond::VM &vm)                                             \


// Thin pass-through macro that captures __FILE__/__LINE__ and forwards to the
// TestFramework::ValidateFunctionCall template, which calls the Bond function and asserts its
// return value, throwing on failure.
#define VALIDATE_FUNCTION_CALL(returnType, functionName, expectedResult, ...)                  \
  TestFramework::ValidateFunctionCall<returnType>(                                             \
    vm, __FILE__, __LINE__, (functionName), (expectedResult), ##__VA_ARGS__)


namespace TestFramework
{

typedef void VMValidationFunction(Bond::VM &vm);

void RunVMTest(
	const char *assertFile,
	int assertLine,
	const char *scriptName,
	VMValidationFunction *validationFunction,
	const Bond::NativeBindingCollection *nativeBinding);


/// \brief Calls functionName on vm, passing args, and asserts that the returned value matches
/// expectedResult. Integral and pointer return types are compared for exact equality; floating
/// point return types are compared within a small relative tolerance. Throws an AssertionFailure
/// on mismatch.
template <typename ReturnT, typename NameT, typename... Args>
void ValidateFunctionCall(
	Bond::VM &vm,
	const char *assertFile,
	int assertLine,
	const NameT &functionName,
	ReturnT expectedResult,
	Args... args)
{
	ReturnT returnValue{};
	vm.CallFunction(functionName, &returnValue, args...);

	if constexpr (std::is_floating_point<ReturnT>::value)
	{
		const ReturnT delta = Bond::Max(Bond::Abs(expectedResult * ReturnT(1.0e-6)), ReturnT(1.0e-6));
		Assert((returnValue >= (expectedResult - delta)) && (returnValue <= (expectedResult + delta)),
			assertFile, assertLine, "Expected %f, but was %f.", double(expectedResult), double(returnValue));
		if ((expectedResult == ReturnT(0)) && (returnValue == ReturnT(0)))
		{
			Assert(std::signbit(returnValue) == std::signbit(expectedResult), assertFile, assertLine,
				"Expected %s0, but was %s0.", std::signbit(expectedResult) ? "-" : "+", std::signbit(returnValue) ? "-" : "+");
		}
	}
	else if constexpr (std::is_pointer<ReturnT>::value)
	{
		Assert(returnValue == expectedResult, assertFile, assertLine,
			"Expected %p, but was %p.", static_cast<const void *>(expectedResult), static_cast<const void *>(returnValue));
	}
	else if constexpr (std::is_signed<ReturnT>::value)
	{
		Assert(returnValue == expectedResult, assertFile, assertLine,
			"Expected %" BOND_PRId64 ", but was %" BOND_PRId64 ".", int64_t(expectedResult), int64_t(returnValue));
	}
	else
	{
		Assert(returnValue == expectedResult, assertFile, assertLine,
			"Expected %" BOND_PRIu64 ", but was %" BOND_PRIu64 ".", uint64_t(expectedResult), uint64_t(returnValue));
	}
}

}

#endif
