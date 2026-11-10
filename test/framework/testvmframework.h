#ifndef TEST_FRAMEWORK_TESTVMFRAMEWORK_H
#define TEST_FRAMEWORK_TESTVMFRAMEWORK_H

#include "framework/asserts.h"
#include "framework/testframework.h"
#include "bond/io/memoryoutputstream.h"
#include "bond/systems/exception.h"
#include "bond/systems/math.h"
#include "bond/vm/vm.h"
#include <cmath>
#include <cstring>
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


// Compiles all of the scripts named by the remaining arguments into a single CBO.
#define DEFINE_VM_MULTI_TEST(testName, ...)                                                    \
  void __Validate ## testName ## __(Bond::VM &vm);                                             \
                                                                                               \
  void __Test ## testName ## __()                                                             \
  {                                                                                            \
    const char *const scriptNames[] = {__VA_ARGS__};                                           \
    TestFramework::RunVMTest(                                                                  \
      __FILE__, __LINE__, scriptNames, sizeof(scriptNames) / sizeof(*scriptNames),             \
      &__Validate ## testName ## __, nullptr);                                                 \
  }                                                                                            \
                                                                                               \
  void __Validate ## testName ## __(Bond::VM &vm)                                             \


// Thin pass-through macro that captures __FILE__/__LINE__ and forwards to the
// TestFramework::ValidateFunctionCall template, which calls the Bond function and asserts its
// return value, throwing on failure.
#define VALIDATE_FUNCTION_CALL(returnType, functionName, expectedResult, ...)                  \
  TestFramework::ValidateFunctionCall<returnType>(                                             \
    vm, __FILE__, __LINE__, (functionName), (expectedResult), ##__VA_ARGS__)


// Thin pass-through macro that captures __FILE__/__LINE__ and forwards to
// TestFramework::AssertCapturedOutput.
#define ASSERT_CAPTURED_OUTPUT(stream, expectedOutput)                                         \
  TestFramework::AssertCapturedOutput(__FILE__, __LINE__, (stream), (expectedOutput))


namespace TestFramework
{

typedef void VMValidationFunction(Bond::VM &vm);

/// \brief Size of the buffer that test scripts are compiled into.
const size_t MAX_CBO_SIZE = 8 * 1024;

/// \brief An in-memory stream that records what a script writes to the VM's stdout or stderr.
class CapturedOutputStream: public Bond::MemoryOutputStream
{
public:
	static const size_t BUFFER_SIZE = 1024;

	CapturedOutputStream(): Bond::MemoryOutputStream(mBuffer, pos_t(BUFFER_SIZE)) {}

	const char *GetBuffer() const { return mBuffer; }

private:
	char mBuffer[BUFFER_SIZE];
};


void RunVMTest(
	const char *assertFile,
	int assertLine,
	const char *scriptName,
	VMValidationFunction *validationFunction,
	const Bond::NativeBindingCollection *nativeBinding);


void RunVMTest(
	const char *assertFile,
	int assertLine,
	const char *const *scriptNames,
	size_t numScripts,
	VMValidationFunction *validationFunction,
	const Bond::NativeBindingCollection *nativeBinding);


/// \brief Compiles scriptName into cboBuffer and returns the size of the CBO. Throws an
/// AssertionFailure if the script fails to compile, the CBO does not fit or memory leaks.
size_t CompileScriptToCbo(
	const char *assertFile,
	int assertLine,
	const char *scriptName,
	uint8_t *cboBuffer,
	size_t bufferSize);


/// \brief Compiles scriptNames into a single CBO in cboBuffer and returns the size of the CBO.
/// Throws an AssertionFailure if a script fails to compile, the CBO does not fit or memory leaks.
size_t CompileScriptsToCbo(
	const char *assertFile,
	int assertLine,
	const char *const *scriptNames,
	size_t numScripts,
	uint8_t *cboBuffer,
	size_t bufferSize);


/// \brief Asserts that the text written to stream since the last call matches expectedOutput, then
/// discards it. The stream must be the VM's stdout or stderr as set up by RunVMTest. Throws an
/// AssertionFailure on mismatch.
void AssertCapturedOutput(
	const char *assertFile,
	int assertLine,
	Bond::OutputStreamAdaptor &stream,
	const char *expectedOutput);


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
		const ReturnT delta = Bond::max(Bond::Abs(expectedResult * ReturnT(1.0e-6)), ReturnT(1.0e-6));
		Assert((returnValue >= (expectedResult - delta)) && (returnValue <= (expectedResult + delta)),
			assertFile, assertLine, "Expected %f, but was %f.", double(expectedResult), double(returnValue));
		if ((expectedResult == ReturnT(0)) && (returnValue == ReturnT(0)))
		{
			Assert(Bond::signbit(returnValue) == Bond::signbit(expectedResult), assertFile, assertLine,
				"Expected %s0, but was %s0.", Bond::signbit(expectedResult) ? "-" : "+", Bond::signbit(returnValue) ? "-" : "+");
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
