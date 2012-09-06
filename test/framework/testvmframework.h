#ifndef TEST_FRAMEWORK_TESTVMFRAMEWORK_H
#define TEST_FRAMEWORK_TESTVMFRAMEWORK_H

#include "framework/testframework.h"
#include "bond/vm.h"

#define DEFINE_VM_TEST(testName, scriptName)                                  \
  bool __Validate ## testName ## __(                                          \
    Bond::TextWriter &logger,                                                 \
    Bond::VM &vm);                                                            \
                                                                              \
  bool __Test ## testName ## __(Bond::TextWriter &logger)                     \
  {                                                                           \
    return TestFramework::RunVMTest(                                          \
      logger, __FILE__, __LINE__, scriptName, &__Validate ## testName ## __); \
  }                                                                           \
                                                                              \
  bool __Validate ## testName ## __(                                          \
    Bond::TextWriter &logger,                                                 \
    Bond::VM &vm)                                                             \


#define VALIDATE_INT_CALL(functionName, arg0, arg1, expectedResult)           \
  {                                                                           \
    Bond::bi32_t returnValue(-9999);                                          \
    Bond::VM::CallerStackFrame stackFrame(vm, functionName, &returnValue);    \
    stackFrame.PushArg(arg0);                                                 \
    stackFrame.PushArg(arg1);                                                 \
    stackFrame.Call();                                                        \
    ASSERT_FORMAT(returnValue == expectedResult,                              \
      ("Expected result to be %" BOND_PRId32 ", but was %" BOND_PRId32 ".",   \
      expectedResult, returnValue));                                          \
  }                                                                           \


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
