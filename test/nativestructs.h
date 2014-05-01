#ifndef NATIVESTRUCTS_H
#define NATIVESTRUCTS_H

#include "bond/api/nativebinding.h"

extern const Bond::NativeBindingCollection NATIVE_STRUCTS_BINDING_COLLECTION;

void Vector3__Dot(Bond::CalleeStackFrame &frame);
void Vector3__X__get(Bond::CalleeStackFrame &frame);
void Vector3__X__set(Bond::CalleeStackFrame &frame);
void Vector3__Y__get(Bond::CalleeStackFrame &frame);
void Vector3__Y__set(Bond::CalleeStackFrame &frame);
void Vector3__Z__get(Bond::CalleeStackFrame &frame);
void Vector3__Z__set(Bond::CalleeStackFrame &frame);

#endif
