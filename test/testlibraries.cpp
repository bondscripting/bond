#include "framework/testvmframework.h"
#include <cctype>
#include <cstring>


DEFINE_VM_TEST(Math, "scripts/sanalyzer_IncludeLibraries.bond")
{
	using namespace Bond;

	const double PI = 3.14159265358979323846;
	const double TWO_PI = PI * 2.0;
	const double HALF_PI = PI / 2.0;
	const double THIRD_PI = PI / 3.0;
	const double SIXTH_PI = PI / 6.0;

	const float PI_F = float(3.14159265358979323846);
	const float TWO_PI_F = float(PI * 2.0);
	const float HALF_PI_F = float(PI / 2.0);
	const float THIRD_PI_F = float(PI / 3.0);
	const float SIXTH_PI_F = float(PI / 6.0);

	VALIDATE_FUNCTION_CALL_1(DOUBLE, "Bond.Sin", 0.0, 0.0);
	VALIDATE_FUNCTION_CALL_1(DOUBLE, "Bond.Sin", 1.0, HALF_PI);
	VALIDATE_FUNCTION_CALL_1(DOUBLE, "Bond.Sin", 0.5, SIXTH_PI);
	VALIDATE_FUNCTION_CALL_1(DOUBLE, "Bond.Sin", -1.0, -HALF_PI);
	VALIDATE_FUNCTION_CALL_1(DOUBLE, "Bond.Sin", -0.5, TWO_PI - SIXTH_PI);

	VALIDATE_FUNCTION_CALL_1(FLOAT, "Bond.Sinf", 0.0f, 0.0f);
	VALIDATE_FUNCTION_CALL_1(FLOAT, "Bond.Sinf", 1.0f, HALF_PI_F);
	VALIDATE_FUNCTION_CALL_1(FLOAT, "Bond.Sinf", 0.5f, SIXTH_PI_F);
	VALIDATE_FUNCTION_CALL_1(FLOAT, "Bond.Sinf", -1.0f, -HALF_PI_F);
	VALIDATE_FUNCTION_CALL_1(FLOAT, "Bond.Sinf", -0.5f, TWO_PI_F - SIXTH_PI_F);

	VALIDATE_FUNCTION_CALL_1(DOUBLE, "Bond.Cos", 1.0, 0.0);
	VALIDATE_FUNCTION_CALL_1(DOUBLE, "Bond.Cos", 0.0, HALF_PI);
	VALIDATE_FUNCTION_CALL_1(DOUBLE, "Bond.Cos", 0.5, THIRD_PI);
	VALIDATE_FUNCTION_CALL_1(DOUBLE, "Bond.Cos", 0.0, -HALF_PI);
	VALIDATE_FUNCTION_CALL_1(DOUBLE, "Bond.Cos", 0.5, TWO_PI - THIRD_PI);
	VALIDATE_FUNCTION_CALL_1(DOUBLE, "Bond.Cos", -0.5, HALF_PI + SIXTH_PI);

	VALIDATE_FUNCTION_CALL_1(FLOAT, "Bond.Cosf", 1.0f, 0.0f);
	VALIDATE_FUNCTION_CALL_1(FLOAT, "Bond.Cosf", 0.0f, HALF_PI_F);
	VALIDATE_FUNCTION_CALL_1(FLOAT, "Bond.Cosf", 0.5f, THIRD_PI_F);
	VALIDATE_FUNCTION_CALL_1(FLOAT, "Bond.Cosf", 0.0f, -HALF_PI_F);
	VALIDATE_FUNCTION_CALL_1(FLOAT, "Bond.Cosf", 0.5f, TWO_PI_F - THIRD_PI_F);
	VALIDATE_FUNCTION_CALL_1(FLOAT, "Bond.Cosf", -0.5f, HALF_PI_F + SIXTH_PI_F);

	return true;
}


DEFINE_VM_TEST(Type, "scripts/sanalyzer_IncludeLibraries.bond")
{
	using namespace Bond;

	VALIDATE_FUNCTION_CALL_1(BOOL, "Bond.IsAlnum", int32_t(isalnum('w') != 0), int32_t('w'));
	VALIDATE_FUNCTION_CALL_1(BOOL, "Bond.IsAlnum", int32_t(isalnum('4') != 0), int32_t('4'));
	VALIDATE_FUNCTION_CALL_1(BOOL, "Bond.IsAlnum", int32_t(isalnum(' ') != 0), int32_t(' '));

	VALIDATE_FUNCTION_CALL_1(BOOL, "Bond.IsAlpha", int32_t(isalpha('w') != 0), int32_t('w'));
	VALIDATE_FUNCTION_CALL_1(BOOL, "Bond.IsAlpha", int32_t(isalpha('4') != 0), int32_t('4'));

	VALIDATE_FUNCTION_CALL_1(BOOL, "Bond.IsCntrl", int32_t(iscntrl('w') != 0), int32_t('w'));
	VALIDATE_FUNCTION_CALL_1(BOOL, "Bond.IsCntrl", int32_t(iscntrl(0x07) != 0), int32_t(0x07));

	VALIDATE_FUNCTION_CALL_1(BOOL, "Bond.IsDigit", int32_t(isdigit('w') != 0), int32_t('w'));
	VALIDATE_FUNCTION_CALL_1(BOOL, "Bond.IsDigit", int32_t(isdigit('4') != 0), int32_t('4'));

	VALIDATE_FUNCTION_CALL_1(BOOL, "Bond.IsGraph", int32_t(isgraph('w') != 0), int32_t('w'));
	VALIDATE_FUNCTION_CALL_1(BOOL, "Bond.IsGraph", int32_t(isgraph(' ') != 0), int32_t(' '));

	VALIDATE_FUNCTION_CALL_1(BOOL, "Bond.IsLower", int32_t(islower('w') != 0), int32_t('w'));
	VALIDATE_FUNCTION_CALL_1(BOOL, "Bond.IsLower", int32_t(islower('W') != 0), int32_t('W'));
	VALIDATE_FUNCTION_CALL_1(BOOL, "Bond.IsLower", int32_t(islower('4') != 0), int32_t('4'));

	VALIDATE_FUNCTION_CALL_1(BOOL, "Bond.IsPrint", int32_t(isprint('w') != 0), int32_t('w'));
	VALIDATE_FUNCTION_CALL_1(BOOL, "Bond.IsPrint", int32_t(isprint(0x07) != 0), int32_t(0x07));

	VALIDATE_FUNCTION_CALL_1(BOOL, "Bond.IsPunct", int32_t(ispunct('w') != 0), int32_t('w'));
	VALIDATE_FUNCTION_CALL_1(BOOL, "Bond.IsPunct", int32_t(ispunct('.') != 0), int32_t('.'));

	VALIDATE_FUNCTION_CALL_1(BOOL, "Bond.IsSpace", int32_t(isspace('w') != 0), int32_t('w'));
	VALIDATE_FUNCTION_CALL_1(BOOL, "Bond.IsSpace", int32_t(isspace(' ') != 0), int32_t(' '));

	VALIDATE_FUNCTION_CALL_1(BOOL, "Bond.IsUpper", int32_t(isupper('w') != 0), int32_t('w'));
	VALIDATE_FUNCTION_CALL_1(BOOL, "Bond.IsUpper", int32_t(isupper('W') != 0), int32_t('W'));
	VALIDATE_FUNCTION_CALL_1(BOOL, "Bond.IsUpper", int32_t(isupper('4') != 0), int32_t('4'));

	VALIDATE_FUNCTION_CALL_1(BOOL, "Bond.IsXDigit", int32_t(isxdigit('w') != 0), int32_t('w'));
	VALIDATE_FUNCTION_CALL_1(BOOL, "Bond.IsXDigit", int32_t(isxdigit('a') != 0), int32_t('a'));

	VALIDATE_FUNCTION_CALL_1(INT, "Bond.ToLower", int32_t(tolower('w')), int32_t('w'));
	VALIDATE_FUNCTION_CALL_1(INT, "Bond.ToLower", int32_t(tolower('W')), int32_t('W'));
	VALIDATE_FUNCTION_CALL_1(INT, "Bond.ToLower", int32_t(tolower('4')), int32_t('4'));

	VALIDATE_FUNCTION_CALL_1(INT, "Bond.ToUpper", int32_t(toupper('w')), int32_t('w'));
	VALIDATE_FUNCTION_CALL_1(INT, "Bond.ToUpper", int32_t(toupper('W')), int32_t('W'));
	VALIDATE_FUNCTION_CALL_1(INT, "Bond.ToUpper", int32_t(toupper('4')), int32_t('4'));

	return true;
}


#define TEST_ITEMS                              \
  TEST_ITEM(Math)                               \
  TEST_ITEM(Type)                               \

RUN_TESTS(Libraries, TEST_ITEMS)
