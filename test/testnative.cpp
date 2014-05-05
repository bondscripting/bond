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
	frame.SetReturnValue(Bond::bi32_t(dot));
}


void Vector3__X__get(Bond::CalleeStackFrame &frame)
{
	const Vector3 &v = *frame.GetArg<const Vector3 *>(0);
	frame.SetReturnValue(Bond::bi32_t(v.GetX()));
}


void Vector3__X__set(Bond::CalleeStackFrame &frame)
{
	Vector3 &v = *frame.GetArg<Vector3 *>(0);
	const int x = int(frame.GetArg<Bond::bi32_t>(1));
	v.SetX(x);
}


void Vector3__Y__get(Bond::CalleeStackFrame &frame)
{
	const Vector3 &v = *frame.GetArg<const Vector3 *>(0);
	frame.SetReturnValue(Bond::bi32_t(v.GetY()));
}


void Vector3__Y__set(Bond::CalleeStackFrame &frame)
{
	Vector3 &v = *frame.GetArg<Vector3 *>(0);
	const int y = int(frame.GetArg<Bond::bi32_t>(1));
	v.SetY(y);
}


void Vector3__Z__get(Bond::CalleeStackFrame &frame)
{
	const Vector3 &v = *frame.GetArg<const Vector3 *>(0);
	frame.SetReturnValue(Bond::bi32_t(v.GetZ()));
}


void Vector3__Z__set(Bond::CalleeStackFrame &frame)
{
	Vector3 &v = *frame.GetArg<Vector3 *>(0);
	const int z = int(frame.GetArg<Bond::bi32_t>(1));
	v.SetZ(z);
}


DEFINE_VM_TEST_WITH_BINDING(NativeStructs, "scripts/vm_NativeStructs.bond", &NATIVE_STRUCTS_BINDING_COLLECTION)
{
	using namespace Bond;

	Vector3 v3a(44, 444, 4444);
	Vector3 v3b;
	VALIDATE_FUNCTION_CALL_1(INT, "::GetVector3X", bi32_t(v3a.GetX()), &v3a);
	VALIDATE_FUNCTION_CALL_1(INT, "::Vector3::X" BOND_NATIVE_GETTER_SUFFIX, bi32_t(v3a.GetX()), &v3a);
	VALIDATE_FUNCTION_CALL_1(INT, "::GetVector3Y", bi32_t(v3a.GetY()), &v3a);
	VALIDATE_FUNCTION_CALL_1(INT, "::Vector3::Y" BOND_NATIVE_GETTER_SUFFIX, bi32_t(v3a.GetY()), &v3a);
	VALIDATE_FUNCTION_CALL_1(INT, "::GetVector3Z", bi32_t(v3a.GetZ()), &v3a);
	VALIDATE_FUNCTION_CALL_1(INT, "::Vector3::Z" BOND_NATIVE_GETTER_SUFFIX, bi32_t(v3a.GetZ()), &v3a);

	v3a = Vector3(2, 3, 4);
	v3b = Vector3(5, -6, 7);
	VALIDATE_FUNCTION_CALL_2(INT, "::DotVector3", bi32_t(20), &v3a, &v3b);
	VALIDATE_FUNCTION_CALL_2(INT, "::Vector3::Dot", bi32_t(20), &v3a, &v3b);

	return true;
}


#define TEST_ITEMS                              \
  TEST_ITEM(NativeStructs)                      \

RUN_TESTS(Native, TEST_ITEMS)
