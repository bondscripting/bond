#ifndef BOND_API_LIBIO_H
#define BOND_API_LIBIO_H

#include "bond/api/nativebinding.h"

#include "bond/conf.h"

namespace Bond
{
extern BOND_API const Bond::NativeBindingCollection IO_BINDING_COLLECTION;
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
BOND_API void InputStream__Close(Bond::StackFrame &frame);
BOND_API void InputStream__ScanStr(Bond::StackFrame &frame);
BOND_API void InputStream__ScanB(Bond::StackFrame &frame);
BOND_API void InputStream__ScanC(Bond::StackFrame &frame);
BOND_API void InputStream__ScanI(Bond::StackFrame &frame);
BOND_API void InputStream__ScanUI(Bond::StackFrame &frame);
BOND_API void InputStream__ScanL(Bond::StackFrame &frame);
BOND_API void InputStream__ScanUL(Bond::StackFrame &frame);
BOND_API void InputStream__ScanF(Bond::StackFrame &frame);
BOND_API void InputStream__ScanD(Bond::StackFrame &frame);
BOND_API void InputStream__Get(Bond::StackFrame &frame);
BOND_API void InputStream__Read(Bond::StackFrame &frame);
BOND_API void InputStream__GetFlags(Bond::StackFrame &frame);
BOND_API void InputStream__SetFlags(Bond::StackFrame &frame);
BOND_API void InputStream__UnsetFlags(Bond::StackFrame &frame);
BOND_API void InputStream__ClearFlags(Bond::StackFrame &frame);
BOND_API void InputStream__GetPosition(Bond::StackFrame &frame);
BOND_API void InputStream__GetEndPosition(Bond::StackFrame &frame);
BOND_API void InputStream__SetPosition(Bond::StackFrame &frame);
BOND_API void InputStream__SetPositionFromEnd(Bond::StackFrame &frame);
BOND_API void InputStream__AddOffset(Bond::StackFrame &frame);
BOND_API void InputStream__IsEof(Bond::StackFrame &frame);
BOND_API void InputStream__HasError(Bond::StackFrame &frame);
BOND_API void InputStream__ClearError(Bond::StackFrame &frame);
BOND_API void OutputStream__Close(Bond::StackFrame &frame);
BOND_API void OutputStream__Flush(Bond::StackFrame &frame);
BOND_API void OutputStream__PrintStr(Bond::StackFrame &frame);
BOND_API void OutputStream__PrintB(Bond::StackFrame &frame);
BOND_API void OutputStream__PrintC(Bond::StackFrame &frame);
BOND_API void OutputStream__PrintI(Bond::StackFrame &frame);
BOND_API void OutputStream__PrintUI(Bond::StackFrame &frame);
BOND_API void OutputStream__PrintL(Bond::StackFrame &frame);
BOND_API void OutputStream__PrintUL(Bond::StackFrame &frame);
BOND_API void OutputStream__PrintF(Bond::StackFrame &frame);
BOND_API void OutputStream__PrintD(Bond::StackFrame &frame);
BOND_API void OutputStream__Put(Bond::StackFrame &frame);
BOND_API void OutputStream__Write(Bond::StackFrame &frame);
BOND_API void OutputStream__GetFlags(Bond::StackFrame &frame);
BOND_API void OutputStream__SetFlags(Bond::StackFrame &frame);
BOND_API void OutputStream__UnsetFlags(Bond::StackFrame &frame);
BOND_API void OutputStream__ClearFlags(Bond::StackFrame &frame);
BOND_API void OutputStream__GetPrecision(Bond::StackFrame &frame);
BOND_API void OutputStream__SetPrecision(Bond::StackFrame &frame);
BOND_API void OutputStream__GetWidth(Bond::StackFrame &frame);
BOND_API void OutputStream__SetWidth(Bond::StackFrame &frame);
BOND_API void OutputStream__GetPosition(Bond::StackFrame &frame);
BOND_API void OutputStream__GetEndPosition(Bond::StackFrame &frame);
BOND_API void OutputStream__SetPosition(Bond::StackFrame &frame);
BOND_API void OutputStream__SetPositionFromEnd(Bond::StackFrame &frame);
BOND_API void OutputStream__AddOffset(Bond::StackFrame &frame);
BOND_API void OutputStream__IsEof(Bond::StackFrame &frame);
BOND_API void OutputStream__HasError(Bond::StackFrame &frame);
BOND_API void OutputStream__ClearError(Bond::StackFrame &frame);
BOND_API void StdIn(Bond::StackFrame &frame);
BOND_API void StdOut(Bond::StackFrame &frame);
BOND_API void StdErr(Bond::StackFrame &frame);
BOND_API void CreateInputStream(Bond::StackFrame &frame);
BOND_API void CreateInputStreamCollected(Bond::StackFrame &frame);
BOND_API void CreateInputStreamWithCollector(Bond::StackFrame &frame);
BOND_API void FreeInputStream(Bond::StackFrame &frame);
BOND_API void CreateOutputStream(Bond::StackFrame &frame);
BOND_API void CreateOutputStreamCollected(Bond::StackFrame &frame);
BOND_API void CreateOutputStreamWithCollector(Bond::StackFrame &frame);
BOND_API void FreeOutputStream(Bond::StackFrame &frame);
}

#endif
