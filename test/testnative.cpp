#include "framework/testvmframework.h"
#include "nativestructs.inl"

class Vector3
{
public:
	Vector3(): mX(0), mY(0), mZ(0) {}
	Vector3(int x, int y, int z): mX(x), mY(y), mZ(z) {}

	int GetX() const { return mX; }
	void SetX(const int x) { mX = x; }

	int GetY() const { return mY; }
	void SetY(const int y) { mY = y; }

	int GetZ() const { return mZ; }
	void SetZ(const int z) { mZ = z; }

	int Dot(const Vector3 &other) const { return mX * other.mX + mY * other.mY + mZ * other.mZ; }

private:
	int mX;
	int mY;
	int mZ;
};


void Vector3__Dot(Bond::CalleeStackFrame &frame)
{
	const Vector3 &v1 = *frame.GetArg<const Vector3 *>(0);
	const Vector3 &v2 = *frame.GetArg<const Vector3 *>(1);
	const int dot = v1.Dot(v2);
	frame.SetReturnValue(int32_t(dot));
}


void Vector3__X__get(Bond::CalleeStackFrame &frame)
{
	const Vector3 &v = *frame.GetArg<const Vector3 *>(0);
	frame.SetReturnValue(int32_t(v.GetX()));
}


void Vector3__X__set(Bond::CalleeStackFrame &frame)
{
	Vector3 &v = *frame.GetArg<Vector3 *>(0);
	const int x = int(frame.GetArg<int32_t>(1));
	v.SetX(x);
}


void Vector3__Y__get(Bond::CalleeStackFrame &frame)
{
	const Vector3 &v = *frame.GetArg<const Vector3 *>(0);
	frame.SetReturnValue(int32_t(v.GetY()));
}


void Vector3__Y__set(Bond::CalleeStackFrame &frame)
{
	Vector3 &v = *frame.GetArg<Vector3 *>(0);
	const int y = int(frame.GetArg<int32_t>(1));
	v.SetY(y);
}


void Vector3__Z__get(Bond::CalleeStackFrame &frame)
{
	const Vector3 &v = *frame.GetArg<const Vector3 *>(0);
	frame.SetReturnValue(int32_t(v.GetZ()));
}


void Vector3__Z__set(Bond::CalleeStackFrame &frame)
{
	Vector3 &v = *frame.GetArg<Vector3 *>(0);
	const int z = int(frame.GetArg<int32_t>(1));
	v.SetZ(z);
}


DEFINE_VM_TEST_WITH_BINDING(NativeStructs, "scripts/vm_NativeStructs.bond", &NATIVE_STRUCTS_BINDING_COLLECTION)
{
	using namespace Bond;

	Vector3 v3a;
	Vector3 v3b;

	vm.CallVoidFunction("::SetVector3", &v3a, int32_t(21), int32_t(-22), int32_t(23));
	ASSERT_FORMAT(v3a.GetX() == 21, ("Expected 21, but was %" BOND_PRId32 ".", v3a.GetX()));
	ASSERT_FORMAT(v3a.GetY() == -22, ("Expected -22, but was %" BOND_PRId32 ".", v3a.GetY()));
	ASSERT_FORMAT(v3a.GetZ() == 23, ("Expected 23, but was %" BOND_PRId32 ".", v3a.GetZ()));

	v3a = Vector3(44, 444, 4444);
	VALIDATE_FUNCTION_CALL_1(INT, "::GetVector3X", int32_t(v3a.GetX()), &v3a);
	VALIDATE_FUNCTION_CALL_1(INT, "::Vector3::X" BOND_NATIVE_GETTER_SUFFIX, int32_t(v3a.GetX()), &v3a);
	VALIDATE_FUNCTION_CALL_1(INT, "::GetVector3Y", int32_t(v3a.GetY()), &v3a);
	VALIDATE_FUNCTION_CALL_1(INT, "::Vector3::Y" BOND_NATIVE_GETTER_SUFFIX, int32_t(v3a.GetY()), &v3a);
	VALIDATE_FUNCTION_CALL_1(INT, "::GetVector3Z", int32_t(v3a.GetZ()), &v3a);
	VALIDATE_FUNCTION_CALL_1(INT, "::Vector3::Z" BOND_NATIVE_GETTER_SUFFIX, int32_t(v3a.GetZ()), &v3a);

	v3a = Vector3(2, 3, 4);
	v3b = Vector3(5, -6, 7);
	VALIDATE_FUNCTION_CALL_2(INT, "::DotVector3", int32_t(20), &v3a, &v3b);
	VALIDATE_FUNCTION_CALL_2(INT, "::Vector3::Dot", int32_t(20), &v3a, &v3b);

	v3a = Vector3(17, 17, 17);
	int32_t oi = 91; int32_t ri = v3a.GetY() + oi;
	VALIDATE_FUNCTION_CALL_2(INT, "::Vector3AddY", ri, &v3a, oi);
	ASSERT_FORMAT(v3a.GetX() == 17, ("Expected 17, but was %" BOND_PRId32 ".", v3a.GetX()));
	ASSERT_FORMAT(v3a.GetY() == ri, ("Expected %" BOND_PRId32 ", but was %" BOND_PRId32 ".", ri, v3a.GetY()));
	ASSERT_FORMAT(v3a.GetZ() == 17, ("Expected 17, but was %" BOND_PRId32 ".", v3a.GetZ()));

	v3a = Vector3(23, 23, 23);
	ri = v3a.GetY() + 1;
	VALIDATE_FUNCTION_CALL_1(INT, "::Vector3PreincrementY", ri, &v3a);
	ASSERT_FORMAT(v3a.GetY() == ri, ("Expected %" BOND_PRId32 ", but was %" BOND_PRId32 ".", ri, v3a.GetY()));

	v3a = Vector3(56, 56, 56);
	ri = v3a.GetY();
	VALIDATE_FUNCTION_CALL_1(INT, "::Vector3PostincrementY", ri, &v3a);
	ASSERT_FORMAT(v3a.GetY() == (ri + 1), ("Expected %" BOND_PRId32 ", but was %" BOND_PRId32 ".", ri, v3a.GetY()));

	return true;
}


#define TEST_ITEMS                              \
  TEST_ITEM(NativeStructs)                      \

RUN_TESTS(Native, TEST_ITEMS)
