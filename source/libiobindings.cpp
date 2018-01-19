#include "bond/api/libio.h"

namespace Bond
{

const Bond::NativeFunctionBinding IO_BINDING_COLLECTION_FUNCTIONS[] =
{
	{"Bond.InputStream.Close", Bond::InputStream__Close},
	{"Bond.InputStream.ScanStr", Bond::InputStream__ScanStr},
	{"Bond.InputStream.ScanB", Bond::InputStream__ScanB},
	{"Bond.InputStream.ScanC", Bond::InputStream__ScanC},
	{"Bond.InputStream.ScanI", Bond::InputStream__ScanI},
	{"Bond.InputStream.ScanUI", Bond::InputStream__ScanUI},
	{"Bond.InputStream.ScanL", Bond::InputStream__ScanL},
	{"Bond.InputStream.ScanUL", Bond::InputStream__ScanUL},
	{"Bond.InputStream.ScanF", Bond::InputStream__ScanF},
	{"Bond.InputStream.ScanD", Bond::InputStream__ScanD},
	{"Bond.InputStream.Get", Bond::InputStream__Get},
	{"Bond.InputStream.Read", Bond::InputStream__Read},
	{"Bond.InputStream.GetFlags", Bond::InputStream__GetFlags},
	{"Bond.InputStream.SetFlags", Bond::InputStream__SetFlags},
	{"Bond.InputStream.UnsetFlags", Bond::InputStream__UnsetFlags},
	{"Bond.InputStream.ClearFlags", Bond::InputStream__ClearFlags},
	{"Bond.InputStream.GetPosition", Bond::InputStream__GetPosition},
	{"Bond.InputStream.GetEndPosition", Bond::InputStream__GetEndPosition},
	{"Bond.InputStream.SetPosition", Bond::InputStream__SetPosition},
	{"Bond.InputStream.SetPositionFromEnd", Bond::InputStream__SetPositionFromEnd},
	{"Bond.InputStream.AddOffset", Bond::InputStream__AddOffset},
	{"Bond.InputStream.IsEof", Bond::InputStream__IsEof},
	{"Bond.InputStream.HasError", Bond::InputStream__HasError},
	{"Bond.InputStream.ClearError", Bond::InputStream__ClearError},
	{"Bond.OutputStream.Close", Bond::OutputStream__Close},
	{"Bond.OutputStream.Flush", Bond::OutputStream__Flush},
	{"Bond.OutputStream.PrintStr", Bond::OutputStream__PrintStr},
	{"Bond.OutputStream.PrintB", Bond::OutputStream__PrintB},
	{"Bond.OutputStream.PrintC", Bond::OutputStream__PrintC},
	{"Bond.OutputStream.PrintI", Bond::OutputStream__PrintI},
	{"Bond.OutputStream.PrintUI", Bond::OutputStream__PrintUI},
	{"Bond.OutputStream.PrintL", Bond::OutputStream__PrintL},
	{"Bond.OutputStream.PrintUL", Bond::OutputStream__PrintUL},
	{"Bond.OutputStream.PrintF", Bond::OutputStream__PrintF},
	{"Bond.OutputStream.PrintD", Bond::OutputStream__PrintD},
	{"Bond.OutputStream.Put", Bond::OutputStream__Put},
	{"Bond.OutputStream.Write", Bond::OutputStream__Write},
	{"Bond.OutputStream.GetFlags", Bond::OutputStream__GetFlags},
	{"Bond.OutputStream.SetFlags", Bond::OutputStream__SetFlags},
	{"Bond.OutputStream.UnsetFlags", Bond::OutputStream__UnsetFlags},
	{"Bond.OutputStream.ClearFlags", Bond::OutputStream__ClearFlags},
	{"Bond.OutputStream.GetPrecision", Bond::OutputStream__GetPrecision},
	{"Bond.OutputStream.SetPrecision", Bond::OutputStream__SetPrecision},
	{"Bond.OutputStream.GetWidth", Bond::OutputStream__GetWidth},
	{"Bond.OutputStream.SetWidth", Bond::OutputStream__SetWidth},
	{"Bond.OutputStream.GetPosition", Bond::OutputStream__GetPosition},
	{"Bond.OutputStream.GetEndPosition", Bond::OutputStream__GetEndPosition},
	{"Bond.OutputStream.SetPosition", Bond::OutputStream__SetPosition},
	{"Bond.OutputStream.SetPositionFromEnd", Bond::OutputStream__SetPositionFromEnd},
	{"Bond.OutputStream.AddOffset", Bond::OutputStream__AddOffset},
	{"Bond.OutputStream.IsEof", Bond::OutputStream__IsEof},
	{"Bond.OutputStream.HasError", Bond::OutputStream__HasError},
	{"Bond.OutputStream.ClearError", Bond::OutputStream__ClearError},
	{"Bond.StdIn", Bond::StdIn},
	{"Bond.StdOut", Bond::StdOut},
	{"Bond.StdErr", Bond::StdErr},
	{"Bond.CreateInputStream", Bond::CreateInputStream},
	{"Bond.CreateInputStreamCollected", Bond::CreateInputStreamCollected},
	{"Bond.CreateInputStreamWithCollector", Bond::CreateInputStreamWithCollector},
	{"Bond.FreeInputStream", Bond::FreeInputStream},
	{"Bond.CreateOutputStream", Bond::CreateOutputStream},
	{"Bond.CreateOutputStreamCollected", Bond::CreateOutputStreamCollected},
	{"Bond.CreateOutputStreamWithCollector", Bond::CreateOutputStreamWithCollector},
	{"Bond.FreeOutputStream", Bond::FreeOutputStream},
	{nullptr, nullptr}
};

const Bond::NativeBindingCollection IO_BINDING_COLLECTION =
{
	IO_BINDING_COLLECTION_FUNCTIONS,
	64
};

}
