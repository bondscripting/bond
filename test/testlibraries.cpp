#include "framework/testvmframework.h"
#include <string.h>


DEFINE_VM_TEST(Math, "scripts/sanalyzer_IncludeLibraries.bond")
{
	using namespace Bond;

	const bf64_t PI = 3.14159265358979323846;
	const bf64_t TWO_PI = PI * 2.0;
	const bf64_t HALF_PI = PI / 2.0;
	const bf64_t THIRD_PI = PI / 3.0;
	const bf64_t SIXTH_PI = PI / 6.0;

	const bf32_t PI_F = 3.14159265358979323846;
	const bf32_t TWO_PI_F = PI * 2.0;
	const bf32_t HALF_PI_F = PI / 2.0;
	const bf32_t THIRD_PI_F = PI / 3.0;
	const bf32_t SIXTH_PI_F = PI / 6.0;

	VALIDATE_FUNCTION_CALL_1(DOUBLE, "::Bond::Sin", bf64_t(0.0), bf64_t(0.0));
	VALIDATE_FUNCTION_CALL_1(DOUBLE, "::Bond::Sin", bf64_t(1.0), HALF_PI);
	VALIDATE_FUNCTION_CALL_1(DOUBLE, "::Bond::Sin", bf64_t(0.5), SIXTH_PI);
	VALIDATE_FUNCTION_CALL_1(DOUBLE, "::Bond::Sin", bf64_t(-1.0), -HALF_PI);
	VALIDATE_FUNCTION_CALL_1(DOUBLE, "::Bond::Sin", bf64_t(-0.5), TWO_PI - SIXTH_PI);

	VALIDATE_FUNCTION_CALL_1(FLOAT, "::Bond::Sinf", bf32_t(0.0), bf32_t(0.0));
	VALIDATE_FUNCTION_CALL_1(FLOAT, "::Bond::Sinf", bf32_t(1.0), HALF_PI_F);
	VALIDATE_FUNCTION_CALL_1(FLOAT, "::Bond::Sinf", bf32_t(0.5), SIXTH_PI_F);
	VALIDATE_FUNCTION_CALL_1(FLOAT, "::Bond::Sinf", bf32_t(-1.0), -HALF_PI_F);
	VALIDATE_FUNCTION_CALL_1(FLOAT, "::Bond::Sinf", bf32_t(-0.5), TWO_PI_F - SIXTH_PI_F);

	VALIDATE_FUNCTION_CALL_1(DOUBLE, "::Bond::Cos", bf64_t(1.0), bf64_t(0.0));
	VALIDATE_FUNCTION_CALL_1(DOUBLE, "::Bond::Cos", bf64_t(0.0), HALF_PI);
	VALIDATE_FUNCTION_CALL_1(DOUBLE, "::Bond::Cos", bf64_t(0.5), THIRD_PI);
	VALIDATE_FUNCTION_CALL_1(DOUBLE, "::Bond::Cos", bf64_t(0.0), -HALF_PI);
	VALIDATE_FUNCTION_CALL_1(DOUBLE, "::Bond::Cos", bf64_t(0.5), TWO_PI - THIRD_PI);
	VALIDATE_FUNCTION_CALL_1(DOUBLE, "::Bond::Cos", bf64_t(-0.5), HALF_PI + SIXTH_PI);

	VALIDATE_FUNCTION_CALL_1(FLOAT, "::Bond::Cosf", bf32_t(1.0), bf32_t(0.0));
	VALIDATE_FUNCTION_CALL_1(FLOAT, "::Bond::Cosf", bf32_t(0.0), HALF_PI_F);
	VALIDATE_FUNCTION_CALL_1(FLOAT, "::Bond::Cosf", bf32_t(0.5), THIRD_PI_F);
	VALIDATE_FUNCTION_CALL_1(FLOAT, "::Bond::Cosf", bf32_t(0.0), -HALF_PI_F);
	VALIDATE_FUNCTION_CALL_1(FLOAT, "::Bond::Cosf", bf32_t(0.5), TWO_PI_F - THIRD_PI_F);
	VALIDATE_FUNCTION_CALL_1(FLOAT, "::Bond::Cosf", bf32_t(-0.5), HALF_PI_F + SIXTH_PI_F);

	return true;
}


#define TEST_ITEMS                              \
  TEST_ITEM(Math)                               \

RUN_TESTS(Libraries, TEST_ITEMS)
