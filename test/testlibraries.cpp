#include "framework/testvmframework.h"
#include <cctype>
#include <cmath>
#include <cstring>
#include <limits>


DEFINE_VM_TEST(Math, "scripts/sanalyzer_IncludeLibraries.bond")
{
	using namespace Bond;

	const double PI = 3.14159265358979323846;
	const double TWO_PI = PI * 2.0;
	const double HALF_PI = PI / 2.0;
	const double THIRD_PI = PI / 3.0;
	const double SIXTH_PI = PI / 6.0;

	//const float PI_F = float(3.14159265358979323846);
	const float TWO_PI_F = float(PI * 2.0);
	const float HALF_PI_F = float(PI / 2.0);
	const float THIRD_PI_F = float(PI / 3.0);
	const float SIXTH_PI_F = float(PI / 6.0);

	VALIDATE_FUNCTION_CALL(double, "Bond.SinD", 0.0, 0.0);
	VALIDATE_FUNCTION_CALL(double, "Bond.SinD", 1.0, HALF_PI);
	VALIDATE_FUNCTION_CALL(double, "Bond.SinD", 0.5, SIXTH_PI);
	VALIDATE_FUNCTION_CALL(double, "Bond.SinD", -1.0, -HALF_PI);
	VALIDATE_FUNCTION_CALL(double, "Bond.SinD", -0.5, TWO_PI - SIXTH_PI);

	VALIDATE_FUNCTION_CALL(float, "Bond.SinF", 0.0f, 0.0f);
	VALIDATE_FUNCTION_CALL(float, "Bond.SinF", 1.0f, HALF_PI_F);
	VALIDATE_FUNCTION_CALL(float, "Bond.SinF", 0.5f, SIXTH_PI_F);
	VALIDATE_FUNCTION_CALL(float, "Bond.SinF", -1.0f, -HALF_PI_F);
	VALIDATE_FUNCTION_CALL(float, "Bond.SinF", -0.5f, TWO_PI_F - SIXTH_PI_F);

	VALIDATE_FUNCTION_CALL(double, "Bond.CosD", 1.0, 0.0);
	VALIDATE_FUNCTION_CALL(double, "Bond.CosD", 0.0, HALF_PI);
	VALIDATE_FUNCTION_CALL(double, "Bond.CosD", 0.5, THIRD_PI);
	VALIDATE_FUNCTION_CALL(double, "Bond.CosD", 0.0, -HALF_PI);
	VALIDATE_FUNCTION_CALL(double, "Bond.CosD", 0.5, TWO_PI - THIRD_PI);
	VALIDATE_FUNCTION_CALL(double, "Bond.CosD", -0.5, HALF_PI + SIXTH_PI);

	VALIDATE_FUNCTION_CALL(float, "Bond.CosF", 1.0f, 0.0f);
	VALIDATE_FUNCTION_CALL(float, "Bond.CosF", 0.0f, HALF_PI_F);
	VALIDATE_FUNCTION_CALL(float, "Bond.CosF", 0.5f, THIRD_PI_F);
	VALIDATE_FUNCTION_CALL(float, "Bond.CosF", 0.0f, -HALF_PI_F);
	VALIDATE_FUNCTION_CALL(float, "Bond.CosF", 0.5f, TWO_PI_F - THIRD_PI_F);
	VALIDATE_FUNCTION_CALL(float, "Bond.CosF", -0.5f, HALF_PI_F + SIXTH_PI_F);

	VALIDATE_FUNCTION_CALL(double, "Bond.CoshD", 1.0, 0.0);
	VALIDATE_FUNCTION_CALL(float, "Bond.CoshF", 1.0f, 0.0f);

	VALIDATE_FUNCTION_CALL(double, "Bond.SqrtD", 4.0, 16.0);
	VALIDATE_FUNCTION_CALL(float, "Bond.SqrtF", 4.0f, 16.0f);
	VALIDATE_FUNCTION_CALL(double, "Bond.CbrtD", -3.0, -27.0);
	VALIDATE_FUNCTION_CALL(float, "Bond.CbrtF", -3.0f, -27.0f);
	VALIDATE_FUNCTION_CALL(double, "Bond.HypotD", 5.0, 3.0, 4.0);
	VALIDATE_FUNCTION_CALL(float, "Bond.HypotF", 5.0f, 3.0f, 4.0f);

	VALIDATE_FUNCTION_CALL(double, "Bond.CeilD", 4.0, PI);
	VALIDATE_FUNCTION_CALL(float, "Bond.CeilF", 4.0f, float(PI));
	VALIDATE_FUNCTION_CALL(double, "Bond.FloorD", -2.0, -1.5);
	VALIDATE_FUNCTION_CALL(float, "Bond.FloorF", -2.0f, -1.5f);
	VALIDATE_FUNCTION_CALL(double, "Bond.TruncD", -1.0, -1.5);
	VALIDATE_FUNCTION_CALL(float, "Bond.TruncF", -1.0f, -1.5f);
	VALIDATE_FUNCTION_CALL(double, "Bond.RoundD", 3.0, 2.5);
	VALIDATE_FUNCTION_CALL(float, "Bond.RoundF", -3.0f, -2.5f);
	VALIDATE_FUNCTION_CALL(double, "Bond.RintD", 2.0, 2.5);
	VALIDATE_FUNCTION_CALL(float, "Bond.NearbyintF", 4.0f, 3.5f);

	VALIDATE_FUNCTION_CALL(double, "Bond.AbsD", 2.5, -2.5);
	VALIDATE_FUNCTION_CALL(float, "Bond.AbsF", 2.5f, -2.5f);
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.AbsS", 7, int16_t(-7));
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.AbsI", 7, int32_t(-7));
	VALIDATE_FUNCTION_CALL(int64_t, "Bond.AbsL", 7, int64_t(-7));

	VALIDATE_FUNCTION_CALL(double, "Bond.LdexpD", 12.0, 1.5, int32_t(3));
	VALIDATE_FUNCTION_CALL(float, "Bond.LdexpF", 12.0f, 1.5f, int32_t(3));
	VALIDATE_FUNCTION_CALL(double, "Bond.FmodD", 1.5, 7.5, 2.0);
	VALIDATE_FUNCTION_CALL(float, "Bond.FmodF", 1.5f, 7.5f, 2.0f);
	VALIDATE_FUNCTION_CALL(double, "Bond.CopySignD", -3.0, 3.0, -0.0);
	VALIDATE_FUNCTION_CALL(float, "Bond.CopySignF", 3.0f, -3.0f, 1.0f);

	VALIDATE_FUNCTION_CALL(int32_t, "Bond.IsNaND", int32_t(1), std::nan(""));
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.IsNaND", int32_t(0), 1.0);
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.IsInfF", int32_t(1), std::numeric_limits<float>::infinity());
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.IsFiniteD", int32_t(0), std::numeric_limits<double>::infinity());
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.SignBitD", int32_t(1), -0.0);

	VALIDATE_FUNCTION_CALL(double, "Bond.MinD", 1.0, 1.0, 2.0);
	VALIDATE_FUNCTION_CALL(float, "Bond.MaxF", 2.0f, 1.0f, 2.0f);
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.MinI", -3, -3, 5);
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.MaxI", 5, -3, 5);
	VALIDATE_FUNCTION_CALL(int64_t, "Bond.MinL", -3, int64_t(-3), int64_t(5));
	VALIDATE_FUNCTION_CALL(uint64_t, "Bond.MaxUL", uint64_t(5), uint64_t(3), uint64_t(5));
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.MinUC", 3, uint8_t(3), uint8_t(200));
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.MaxUS", 200, uint16_t(3), uint16_t(200));

	VALIDATE_FUNCTION_CALL(double, "Bond.ClampD", 1.0, 5.0, 0.0, 1.0);
	VALIDATE_FUNCTION_CALL(float, "Bond.ClampF", 0.5f, 0.5f, 0.0f, 1.0f);
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.ClampI", 0, -5, 0, 10);
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.ClampS", 10, int16_t(15), int16_t(0), int16_t(10));
}


DEFINE_VM_TEST(Type, "scripts/sanalyzer_IncludeLibraries.bond")
{
	using namespace Bond;

	VALIDATE_FUNCTION_CALL(int32_t, "Bond.IsAlnum", int32_t(isalnum('w') != 0), int32_t('w'));
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.IsAlnum", int32_t(isalnum('4') != 0), int32_t('4'));
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.IsAlnum", int32_t(isalnum(' ') != 0), int32_t(' '));

	VALIDATE_FUNCTION_CALL(int32_t, "Bond.IsAlpha", int32_t(isalpha('w') != 0), int32_t('w'));
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.IsAlpha", int32_t(isalpha('4') != 0), int32_t('4'));

	VALIDATE_FUNCTION_CALL(int32_t, "Bond.IsCntrl", int32_t(iscntrl('w') != 0), int32_t('w'));
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.IsCntrl", int32_t(iscntrl(0x07) != 0), int32_t(0x07));

	VALIDATE_FUNCTION_CALL(int32_t, "Bond.IsDigit", int32_t(isdigit('w') != 0), int32_t('w'));
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.IsDigit", int32_t(isdigit('4') != 0), int32_t('4'));

	VALIDATE_FUNCTION_CALL(int32_t, "Bond.IsGraph", int32_t(isgraph('w') != 0), int32_t('w'));
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.IsGraph", int32_t(isgraph(' ') != 0), int32_t(' '));

	VALIDATE_FUNCTION_CALL(int32_t, "Bond.IsLower", int32_t(islower('w') != 0), int32_t('w'));
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.IsLower", int32_t(islower('W') != 0), int32_t('W'));
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.IsLower", int32_t(islower('4') != 0), int32_t('4'));

	VALIDATE_FUNCTION_CALL(int32_t, "Bond.IsPrint", int32_t(isprint('w') != 0), int32_t('w'));
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.IsPrint", int32_t(isprint(0x07) != 0), int32_t(0x07));

	VALIDATE_FUNCTION_CALL(int32_t, "Bond.IsPunct", int32_t(ispunct('w') != 0), int32_t('w'));
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.IsPunct", int32_t(ispunct('.') != 0), int32_t('.'));

	VALIDATE_FUNCTION_CALL(int32_t, "Bond.IsSpace", int32_t(isspace('w') != 0), int32_t('w'));
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.IsSpace", int32_t(isspace(' ') != 0), int32_t(' '));

	VALIDATE_FUNCTION_CALL(int32_t, "Bond.IsUpper", int32_t(isupper('w') != 0), int32_t('w'));
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.IsUpper", int32_t(isupper('W') != 0), int32_t('W'));
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.IsUpper", int32_t(isupper('4') != 0), int32_t('4'));

	VALIDATE_FUNCTION_CALL(int32_t, "Bond.IsXDigit", int32_t(isxdigit('w') != 0), int32_t('w'));
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.IsXDigit", int32_t(isxdigit('a') != 0), int32_t('a'));

	VALIDATE_FUNCTION_CALL(int32_t, "Bond.ToLower", int32_t(tolower('w')), int32_t('w'));
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.ToLower", int32_t(tolower('W')), int32_t('W'));
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.ToLower", int32_t(tolower('4')), int32_t('4'));

	VALIDATE_FUNCTION_CALL(int32_t, "Bond.ToUpper", int32_t(toupper('w')), int32_t('w'));
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.ToUpper", int32_t(toupper('W')), int32_t('W'));
	VALIDATE_FUNCTION_CALL(int32_t, "Bond.ToUpper", int32_t(toupper('4')), int32_t('4'));
}


DEFINE_VM_TEST(Collector, "scripts/vm_Collector.bond")
{
	using namespace Bond;

	VALIDATE_FUNCTION_CALL(int32_t, "RunCollectorTest", int32_t(1));
}


#define TEST_ITEMS                              \
  TEST_ITEM(Math)                               \
  TEST_ITEM(Type)                               \
  TEST_ITEM(Collector)                          \

RUN_TESTS(Libraries, TEST_ITEMS)
