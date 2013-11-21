#ifndef TEST_FRAMEWORK_TESTVMFRAMEWORK_H
#define TEST_FRAMEWORK_TESTVMFRAMEWORK_H

#include "framework/testframework.h"
#include "bond/systems/math.h"
#include "bond/vm/vm.h"

#define DEFINE_VM_TEST(testName, scriptName)                                                   \
  bool __Validate ## testName ## __(                                                           \
    Bond::TextWriter &logger,                                                                  \
    Bond::VM &vm);                                                                             \
                                                                                               \
  bool __Test ## testName ## __(Bond::TextWriter &logger)                                      \
  {                                                                                            \
    return TestFramework::RunVMTest(                                                           \
      logger, __FILE__, __LINE__, scriptName, &__Validate ## testName ## __);                  \
  }                                                                                            \
                                                                                               \
  bool __Validate ## testName ## __(                                                           \
    Bond::TextWriter &logger,                                                                  \
    Bond::VM &vm)                                                                              \


#define VALIDATE_FUNCTION_CALL_0(returnType, functionName, expectedResult)                     \
  {                                                                                            \
    DECLARE_RETURN_VALUE_ ## returnType()                                                      \
    vm.CallFunction((functionName), &returnValue);                                             \
    VALIDATE_RETURN_VALUE_ ## returnType(expectedResult)                                       \
  }                                                                                            \


#define VALIDATE_FUNCTION_CALL_1(returnType, functionName, expectedResult, a0)                 \
  {                                                                                            \
    DECLARE_RETURN_VALUE_ ## returnType()                                                      \
    vm.CallFunction((functionName), &returnValue, a0);                                         \
    VALIDATE_RETURN_VALUE_ ## returnType(expectedResult)                                       \
  }                                                                                            \


#define VALIDATE_FUNCTION_CALL_2(returnType, functionName, expectedResult, a0, a1)             \
  {                                                                                            \
    DECLARE_RETURN_VALUE_ ## returnType()                                                      \
    vm.CallFunction((functionName), &returnValue, a0, a1);                                     \
    VALIDATE_RETURN_VALUE_ ## returnType(expectedResult)                                       \
  }                                                                                            \


#define VALIDATE_FUNCTION_CALL_3(returnType, functionName, expectedResult, a0, a1, a2)         \
  {                                                                                            \
    DECLARE_RETURN_VALUE_ ## returnType()                                                      \
    vm.CallFunction((functionName), &returnValue, a0, a1, a2);                                 \
    VALIDATE_RETURN_VALUE_ ## returnType(expectedResult)                                       \
  }                                                                                            \


#define VALIDATE_FUNCTION_CALL_4(returnType, functionName, expectedResult, a0, a1, a2, a3)     \
  {                                                                                            \
    DECLARE_RETURN_VALUE_ ## returnType()                                                      \
    vm.CallFunction((functionName), &returnValue, a0, a1, a2, a3);                             \
    VALIDATE_RETURN_VALUE_ ## returnType(expectedResult)                                       \
  }                                                                                            \


#define VALIDATE_FUNCTION_CALL_5(returnType, functionName, expectedResult, a0, a1, a2, a3, a4) \
  {                                                                                            \
    DECLARE_RETURN_VALUE_ ## returnType()                                                      \
    vm.CallFunction((functionName), &returnValue, a0, a1, a2, a3, a4);                         \
    VALIDATE_RETURN_VALUE_ ## returnType(expectedResult)                                       \
  }                                                                                            \


#define DECLARE_RETURN_VALUE_BOOL() DECLARE_RETURN_VALUE_INT()
#define DECLARE_RETURN_VALUE_INT() Bond::bi32_t returnValue = 0;
#define DECLARE_RETURN_VALUE_UINT() Bond::bu32_t returnValue = 0;
#define DECLARE_RETURN_VALUE_LONG() Bond::bi64_t returnValue = 0;
#define DECLARE_RETURN_VALUE_ULONG() Bond::bu64_t returnValue = 0;
#define DECLARE_RETURN_VALUE_FLOAT() Bond::bf32_t returnValue = 0.0f;
#define DECLARE_RETURN_VALUE_DOUBLE() Bond::bf64_t returnValue = 0.0;
#define DECLARE_RETURN_VALUE_PTR() const void *returnValue = NULL;


#define VALIDATE_RETURN_VALUE_BOOL(expectedResult)                                            \
  VALIDATE_RETURN_VALUE_INT(expectedResult)                                                   \


#define VALIDATE_RETURN_VALUE_INT(expectedResult)                                             \
  ASSERT_FORMAT(returnValue == (expectedResult),                                              \
    ("Expected %" BOND_PRId32 ", but was %" BOND_PRId32 ".", (expectedResult), returnValue)); \


#define VALIDATE_RETURN_VALUE_UINT(expectedResult)                                            \
  ASSERT_FORMAT(returnValue == (expectedResult),                                              \
    ("Expected %" BOND_PRIu32 ", but was %" BOND_PRIu32 ".", (expectedResult), returnValue)); \


#define VALIDATE_RETURN_VALUE_LONG(expectedResult)                                            \
  ASSERT_FORMAT(returnValue == (expectedResult),                                              \
    ("Expected %" BOND_PRId64 ", but was %" BOND_PRId64 ".", (expectedResult), returnValue)); \


#define VALIDATE_RETURN_VALUE_ULONG(expectedResult)                                           \
  ASSERT_FORMAT(returnValue == (expectedResult),                                              \
    ("Expected %" BOND_PRIu64 ", but was %" BOND_PRIu64 ".", (expectedResult), returnValue)); \


#define VALIDATE_RETURN_VALUE_FLOAT(expectedResult)                                              \
  {                                                                                              \
    const Bond::bf32_t delta = Bond::Max(Bond::Abs((expectedResult) * bf32_t(1.0e-6)), bf32_t(1.0e-6)); \
    ASSERT_FORMAT((returnValue >= ((expectedResult) - delta)) &&                                 \
      (returnValue <= ((expectedResult) + delta)),                                               \
      ("Expected %" BOND_PRIf32 ", but was %" BOND_PRIf32 ".", (expectedResult), returnValue));  \
  }                                                                                              \


#define VALIDATE_RETURN_VALUE_DOUBLE(expectedResult)                                            \
  {                                                                                             \
    const Bond::bf64_t delta = Bond::Max(Bond::Abs((expectedResult) * bf64_t(1.0e-6)), bf64_t(1.0e-6)); \
    ASSERT_FORMAT((returnValue >= ((expectedResult) - delta)) &&                                \
      (returnValue <= ((expectedResult) + delta)),                                              \
      ("Expected %" BOND_PRIf64 ", but was %" BOND_PRIf64 ".", (expectedResult), returnValue)); \
  }                                                                                             \


#define VALIDATE_RETURN_VALUE_PTR(expectedResult)                                             \
  ASSERT_FORMAT(returnValue == (expectedResult),                                              \
    ("Expected %p, but was %p.", (expectedResult), returnValue));                             \


namespace TestFramework
{

typedef bool VMValidationFunction(
	Bond::TextWriter &logger,
	Bond::VM &vm);

bool RunVMTest(
	Bond::TextWriter &logger,
	const char *assertFile,
	int assertLine,
	const char *scriptName,
	VMValidationFunction *validationFunction);

}

#endif
