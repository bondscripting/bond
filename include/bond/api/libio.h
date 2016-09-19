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
	SkipWhite = 8,
	Zero = 16,
	Dec = 32,
	Hex = 64,
	Oct = 128,
	Fixed = 256,
	Scientific = 512,
	Left = 1024,
};
enum Constants
{
	Eof = -1,
};
}
void InputStream__Close(Bond::StackFrame &frame);
void InputStream__ScanStr(Bond::StackFrame &frame);
void InputStream__ScanB(Bond::StackFrame &frame);
void InputStream__ScanC(Bond::StackFrame &frame);
void InputStream__ScanI(Bond::StackFrame &frame);
void InputStream__ScanUI(Bond::StackFrame &frame);
void InputStream__ScanL(Bond::StackFrame &frame);
void InputStream__ScanUL(Bond::StackFrame &frame);
void InputStream__ScanF(Bond::StackFrame &frame);
void InputStream__ScanD(Bond::StackFrame &frame);
void InputStream__Get(Bond::StackFrame &frame);
void InputStream__Read(Bond::StackFrame &frame);
void InputStream__GetFlags(Bond::StackFrame &frame);
void InputStream__SetFlags(Bond::StackFrame &frame);
void InputStream__UnsetFlags(Bond::StackFrame &frame);
void InputStream__ClearFlags(Bond::StackFrame &frame);
void InputStream__GetPosition(Bond::StackFrame &frame);
void InputStream__GetEndPosition(Bond::StackFrame &frame);
void InputStream__SetPosition(Bond::StackFrame &frame);
void InputStream__SetPositionFromEnd(Bond::StackFrame &frame);
void InputStream__AddOffset(Bond::StackFrame &frame);
void InputStream__IsEof(Bond::StackFrame &frame);
void OutputStream__Close(Bond::StackFrame &frame);
void OutputStream__Flush(Bond::StackFrame &frame);
void OutputStream__PrintStr(Bond::StackFrame &frame);
void OutputStream__PrintB(Bond::StackFrame &frame);
void OutputStream__PrintC(Bond::StackFrame &frame);
void OutputStream__PrintI(Bond::StackFrame &frame);
void OutputStream__PrintUI(Bond::StackFrame &frame);
void OutputStream__PrintL(Bond::StackFrame &frame);
void OutputStream__PrintUL(Bond::StackFrame &frame);
void OutputStream__PrintF(Bond::StackFrame &frame);
void OutputStream__PrintD(Bond::StackFrame &frame);
void OutputStream__Put(Bond::StackFrame &frame);
void OutputStream__Write(Bond::StackFrame &frame);
void OutputStream__GetFlags(Bond::StackFrame &frame);
void OutputStream__SetFlags(Bond::StackFrame &frame);
void OutputStream__UnsetFlags(Bond::StackFrame &frame);
void OutputStream__ClearFlags(Bond::StackFrame &frame);
void OutputStream__GetPrecision(Bond::StackFrame &frame);
void OutputStream__SetPrecision(Bond::StackFrame &frame);
void OutputStream__GetWidth(Bond::StackFrame &frame);
void OutputStream__SetWidth(Bond::StackFrame &frame);
void OutputStream__GetPosition(Bond::StackFrame &frame);
void OutputStream__GetEndPosition(Bond::StackFrame &frame);
void OutputStream__SetPosition(Bond::StackFrame &frame);
void OutputStream__SetPositionFromEnd(Bond::StackFrame &frame);
void OutputStream__AddOffset(Bond::StackFrame &frame);
void OutputStream__IsEof(Bond::StackFrame &frame);
void StdIn(Bond::StackFrame &frame);
void StdOut(Bond::StackFrame &frame);
void StdErr(Bond::StackFrame &frame);
void CreateInputStream(Bond::StackFrame &frame);
void CreateInputStreamCollected(Bond::StackFrame &frame);
void CreateInputStreamWithCollector(Bond::StackFrame &frame);
void FreeInputStream(Bond::StackFrame &frame);
void CreateOutputStream(Bond::StackFrame &frame);
void CreateOutputStreamCollected(Bond::StackFrame &frame);
void CreateOutputStreamWithCollector(Bond::StackFrame &frame);
void FreeOutputStream(Bond::StackFrame &frame);
}

#endif
