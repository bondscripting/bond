#ifndef BOND_API_LIBIO_H
#define BOND_API_LIBIO_H

#include "bond/api/nativebinding.h"

namespace Bond
{
extern const Bond::NativeBindingCollection IO_BINDING_COLLECTION;
}

namespace Bond
{
namespace IO
{
enum Flags
{
	BoolAlpha = 1,
	ShowBase = 2,
	ShowPoint = 4,
	Zero = 8,
	Dec = 16,
	Hex = 32,
	Oct = 64,
	Fixed = 128,
	Scientific = 256,
	Left = 512,
};
enum Constants
{
	EOF = -1,
};
}
void InputStream__ScanStr(Bond::CalleeStackFrame &frame);
void InputStream__ScanB(Bond::CalleeStackFrame &frame);
void InputStream__ScanC(Bond::CalleeStackFrame &frame);
void InputStream__ScanI(Bond::CalleeStackFrame &frame);
void InputStream__ScanUI(Bond::CalleeStackFrame &frame);
void InputStream__ScanL(Bond::CalleeStackFrame &frame);
void InputStream__ScanUL(Bond::CalleeStackFrame &frame);
void InputStream__ScanF(Bond::CalleeStackFrame &frame);
void InputStream__ScanD(Bond::CalleeStackFrame &frame);
void InputStream__Get(Bond::CalleeStackFrame &frame);
void InputStream__Read(Bond::CalleeStackFrame &frame);
void InputStream__GetFlags(Bond::CalleeStackFrame &frame);
void InputStream__SetFlags(Bond::CalleeStackFrame &frame);
void InputStream__UnsetFlags(Bond::CalleeStackFrame &frame);
void InputStream__ClearFlags(Bond::CalleeStackFrame &frame);
void InputStream__GetPosition(Bond::CalleeStackFrame &frame);
void InputStream__SetPosition(Bond::CalleeStackFrame &frame);
void InputStream__SetPositionFromEnd(Bond::CalleeStackFrame &frame);
void InputStream__AddOffset(Bond::CalleeStackFrame &frame);
void InputStream__IsEof(Bond::CalleeStackFrame &frame);
void OutputStream__PrintStr(Bond::CalleeStackFrame &frame);
void OutputStream__PrintB(Bond::CalleeStackFrame &frame);
void OutputStream__PrintC(Bond::CalleeStackFrame &frame);
void OutputStream__PrintI(Bond::CalleeStackFrame &frame);
void OutputStream__PrintUI(Bond::CalleeStackFrame &frame);
void OutputStream__PrintL(Bond::CalleeStackFrame &frame);
void OutputStream__PrintUL(Bond::CalleeStackFrame &frame);
void OutputStream__PrintF(Bond::CalleeStackFrame &frame);
void OutputStream__PrintD(Bond::CalleeStackFrame &frame);
void OutputStream__Put(Bond::CalleeStackFrame &frame);
void OutputStream__Write(Bond::CalleeStackFrame &frame);
void OutputStream__GetFlags(Bond::CalleeStackFrame &frame);
void OutputStream__SetFlags(Bond::CalleeStackFrame &frame);
void OutputStream__UnsetFlags(Bond::CalleeStackFrame &frame);
void OutputStream__ClearFlags(Bond::CalleeStackFrame &frame);
void OutputStream__GetPrecision(Bond::CalleeStackFrame &frame);
void OutputStream__SetPrecision(Bond::CalleeStackFrame &frame);
void OutputStream__GetWidth(Bond::CalleeStackFrame &frame);
void OutputStream__SetWidth(Bond::CalleeStackFrame &frame);
void OutputStream__GetPosition(Bond::CalleeStackFrame &frame);
void OutputStream__SetPosition(Bond::CalleeStackFrame &frame);
void OutputStream__SetPositionFromEnd(Bond::CalleeStackFrame &frame);
void OutputStream__AddOffset(Bond::CalleeStackFrame &frame);
void OutputStream__IsEof(Bond::CalleeStackFrame &frame);
void StdIn(Bond::CalleeStackFrame &frame);
void StdOut(Bond::CalleeStackFrame &frame);
void StdErr(Bond::CalleeStackFrame &frame);
}

#endif
