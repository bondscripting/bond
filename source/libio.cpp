#include "bond/api/libio.h"
#include "bond/io/inputstream.h"
#include "bond/io/inputstreamadaptor.h"
#include "bond/io/outputstream.h"
#include "bond/io/outputstreamadaptor.h"
#include "bond/systems/allocator.h"
#include "bond/vm/vm.h"

namespace Bond
{

void InputStream__ScanStr(Bond::CalleeStackFrame &frame)
{
	InputStreamAdaptor *stream = frame.GetArg<InputStreamAdaptor *>(0);
	char *str = (frame.GetArg<char *>(1));
	const uint32_t bufferSize = frame.GetArg<uint32_t>(2);
	stream->ScanStr(str, bufferSize);
}


void InputStream__ScanB(Bond::CalleeStackFrame &frame)
{
	InputStreamAdaptor *stream = frame.GetArg<InputStreamAdaptor *>(0);
	const bool value = stream->ScanBool();
	frame.SetReturnValue(value);
}


void InputStream__ScanC(Bond::CalleeStackFrame &frame)
{
	InputStreamAdaptor *stream = frame.GetArg<InputStreamAdaptor *>(0);
	const int8_t value = stream->ScanChar();
	frame.SetReturnValue(value);
}


void InputStream__ScanI(Bond::CalleeStackFrame &frame)
{
	InputStreamAdaptor *stream = frame.GetArg<InputStreamAdaptor *>(0);
	const int32_t value = stream->ScanInt();
	frame.SetReturnValue(value);
}


void InputStream__ScanUI(Bond::CalleeStackFrame &frame)
{
	InputStreamAdaptor *stream = frame.GetArg<InputStreamAdaptor *>(0);
	const uint32_t value = stream->ScanUInt();
	frame.SetReturnValue(value);
}


void InputStream__ScanL(Bond::CalleeStackFrame &frame)
{
	InputStreamAdaptor *stream = frame.GetArg<InputStreamAdaptor *>(0);
	const int64_t value = stream->ScanLong();
	frame.SetReturnValue(value);
}


void InputStream__ScanUL(Bond::CalleeStackFrame &frame)
{
	InputStreamAdaptor *stream = frame.GetArg<InputStreamAdaptor *>(0);
	const uint64_t value = stream->ScanULong();
	frame.SetReturnValue(value);
}


void InputStream__ScanF(Bond::CalleeStackFrame &frame)
{
	InputStreamAdaptor *stream = frame.GetArg<InputStreamAdaptor *>(0);
	const double value = stream->ScanDouble();
	frame.SetReturnValue(float(value));
}


void InputStream__ScanD(Bond::CalleeStackFrame &frame)
{
	InputStreamAdaptor *stream = frame.GetArg<InputStreamAdaptor *>(0);
	const double value = stream->ScanDouble();
	frame.SetReturnValue(value);
}


void InputStream__Get(Bond::CalleeStackFrame &frame)
{
	InputStreamAdaptor *stream = frame.GetArg<InputStreamAdaptor *>(0);
	const int value = stream->GetStream()->Read();
	frame.SetReturnValue(int32_t(value));
}


void InputStream__Read(Bond::CalleeStackFrame &frame)
{
	InputStreamAdaptor *stream = frame.GetArg<InputStreamAdaptor *>(0);
	uint8_t *bytes = (frame.GetArg<uint8_t *>(1));
	const size_t numBytes = size_t(frame.GetArg<uint32_t>(2));
	const size_t numRead = stream->GetStream()->Read(bytes, numBytes);
	frame.SetReturnValue(uint32_t(numRead));
}


void InputStream__GetFlags(Bond::CalleeStackFrame &frame)
{
	InputStreamAdaptor *stream = frame.GetArg<InputStreamAdaptor *>(0);
	const int32_t flags = int32_t(stream->GetFlags());
	frame.SetReturnValue(flags);
}


void InputStream__SetFlags(Bond::CalleeStackFrame &frame)
{
	InputStreamAdaptor *stream = frame.GetArg<InputStreamAdaptor *>(0);
	const int flags = int(frame.GetArg<int32_t>(1));
	stream->SetFlags(flags);
	frame.SetReturnValue(stream);
}


void InputStream__UnsetFlags(Bond::CalleeStackFrame &frame)
{
	InputStreamAdaptor *stream = frame.GetArg<InputStreamAdaptor *>(0);
	const int flags = int(frame.GetArg<int32_t>(1));
	stream->UnsetFlags(flags);
	frame.SetReturnValue(stream);
}


void InputStream__ClearFlags(Bond::CalleeStackFrame &frame)
{
	InputStreamAdaptor *stream = frame.GetArg<InputStreamAdaptor *>(0);
	stream->ClearFlags();
	frame.SetReturnValue(stream);
}


void InputStream__GetPosition(Bond::CalleeStackFrame &frame)
{
	InputStreamAdaptor *stream = frame.GetArg<InputStreamAdaptor *>(0);
	int32_t pos = int32_t(stream->GetStream()->GetPosition());
	frame.SetReturnValue(pos);
}


void InputStream__SetPosition(Bond::CalleeStackFrame &frame)
{
	InputStreamAdaptor *stream = frame.GetArg<InputStreamAdaptor *>(0);
	const int32_t offset = frame.GetArg<int32_t>(1);
	stream->GetStream()->SetPosition(Stream::pos_t(offset));
}


void InputStream__SetPositionFromEnd(Bond::CalleeStackFrame &frame)
{
	InputStreamAdaptor *stream = frame.GetArg<InputStreamAdaptor *>(0);
	const int32_t offset = frame.GetArg<int32_t>(1);
	stream->GetStream()->SetPositionFromEnd(Stream::pos_t(offset));
}


void InputStream__AddOffset(Bond::CalleeStackFrame &frame)
{
	InputStreamAdaptor *stream = frame.GetArg<InputStreamAdaptor *>(0);
	const int32_t value = frame.GetArg<int32_t>(1);
	stream->GetStream()->AddOffset(Stream::pos_t(value));
}


void InputStream__IsEof(Bond::CalleeStackFrame &frame)
{
	InputStreamAdaptor *stream = frame.GetArg<InputStreamAdaptor *>(0);
	const bool isEof = stream->GetStream()->IsEof();
	frame.SetReturnValue(isEof);
}


void OutputStream__PrintStr(Bond::CalleeStackFrame &frame)
{
	OutputStreamAdaptor *stream = frame.GetArg<OutputStreamAdaptor *>(0);
	const char *str = frame.GetArg<const char *>(1);
	stream->Print(str);
	frame.SetReturnValue(stream);
}


void OutputStream__PrintB(Bond::CalleeStackFrame &frame)
{
	OutputStreamAdaptor *stream = frame.GetArg<OutputStreamAdaptor *>(0);
	const bool value = frame.GetArg<bool>(1);
	stream->Print(value);
	frame.SetReturnValue(stream);
}


void OutputStream__PrintC(Bond::CalleeStackFrame &frame)
{
	OutputStreamAdaptor *stream = frame.GetArg<OutputStreamAdaptor *>(0);
	const char value = char(frame.GetArg<int8_t>(1));
	stream->Print(value);
	frame.SetReturnValue(stream);
}


void OutputStream__PrintI(Bond::CalleeStackFrame &frame)
{
	OutputStreamAdaptor *stream = frame.GetArg<OutputStreamAdaptor *>(0);
	const int32_t value = frame.GetArg<int32_t>(1);
	stream->Print(value);
	frame.SetReturnValue(stream);
}


void OutputStream__PrintUI(Bond::CalleeStackFrame &frame)
{
	OutputStreamAdaptor *stream = frame.GetArg<OutputStreamAdaptor *>(0);
	const uint32_t value = frame.GetArg<uint32_t>(1);
	stream->Print(value);
	frame.SetReturnValue(stream);
}


void OutputStream__PrintL(Bond::CalleeStackFrame &frame)
{
	OutputStreamAdaptor *stream = frame.GetArg<OutputStreamAdaptor *>(0);
	const int64_t value = frame.GetArg<int64_t>(1);
	stream->Print(value);
	frame.SetReturnValue(stream);
}


void OutputStream__PrintUL(Bond::CalleeStackFrame &frame)
{
	OutputStreamAdaptor *stream = frame.GetArg<OutputStreamAdaptor *>(0);
	const uint64_t value = frame.GetArg<uint64_t>(1);
	stream->Print(value);
	frame.SetReturnValue(stream);
}


void OutputStream__PrintF(Bond::CalleeStackFrame &frame)
{
	OutputStreamAdaptor *stream = frame.GetArg<OutputStreamAdaptor *>(0);
	const float value = frame.GetArg<float>(1);
	stream->Print(double(value));
	frame.SetReturnValue(stream);
}


void OutputStream__PrintD(Bond::CalleeStackFrame &frame)
{
	OutputStreamAdaptor *stream = frame.GetArg<OutputStreamAdaptor *>(0);
	const double value = frame.GetArg<double>(1);
	stream->Print(value);
	frame.SetReturnValue(stream);
}


void OutputStream__Put(Bond::CalleeStackFrame &frame)
{
	OutputStreamAdaptor *stream = frame.GetArg<OutputStreamAdaptor *>(0);
	const char byte = char(frame.GetArg<int8_t>(1));
	stream->GetStream()->Write(byte);
	frame.SetReturnValue(stream);
}


void OutputStream__Write(Bond::CalleeStackFrame &frame)
{
	OutputStreamAdaptor *stream = frame.GetArg<OutputStreamAdaptor *>(0);
	const uint8_t *bytes = (frame.GetArg<const uint8_t *>(1));
	const size_t numBytes = size_t(frame.GetArg<uint32_t>(2));
	stream->GetStream()->Write(bytes, numBytes);
	frame.SetReturnValue(stream);
}


void OutputStream__GetFlags(Bond::CalleeStackFrame &frame)
{
	OutputStreamAdaptor *stream = frame.GetArg<OutputStreamAdaptor *>(0);
	const int32_t flags = int32_t(stream->GetFlags());
	frame.SetReturnValue(flags);
}


void OutputStream__SetFlags(Bond::CalleeStackFrame &frame)
{
	OutputStreamAdaptor *stream = frame.GetArg<OutputStreamAdaptor *>(0);
	const int flags = int(frame.GetArg<int32_t>(1));
	stream->SetFlags(flags);
	frame.SetReturnValue(stream);
}


void OutputStream__UnsetFlags(Bond::CalleeStackFrame &frame)
{
	OutputStreamAdaptor *stream = frame.GetArg<OutputStreamAdaptor *>(0);
	const int flags = int(frame.GetArg<int32_t>(1));
	stream->UnsetFlags(flags);
	frame.SetReturnValue(stream);
}


void OutputStream__ClearFlags(Bond::CalleeStackFrame &frame)
{
	OutputStreamAdaptor *stream = frame.GetArg<OutputStreamAdaptor *>(0);
	stream->ClearFlags();
	frame.SetReturnValue(stream);
}


void OutputStream__GetPrecision(Bond::CalleeStackFrame &frame)
{
	OutputStreamAdaptor *stream = frame.GetArg<OutputStreamAdaptor *>(0);
	const int32_t precision = int32_t(stream->GetPrecision());
	frame.SetReturnValue(precision);
}


void OutputStream__SetPrecision(Bond::CalleeStackFrame &frame)
{
	OutputStreamAdaptor *stream = frame.GetArg<OutputStreamAdaptor *>(0);
	const int precision = int(frame.GetArg<int32_t>(1));
	stream->SetPrecision(precision);
	frame.SetReturnValue(stream);
}


void OutputStream__GetWidth(Bond::CalleeStackFrame &frame)
{
	OutputStreamAdaptor *stream = frame.GetArg<OutputStreamAdaptor *>(0);
	const int32_t width = int32_t(stream->GetWidth());
	frame.SetReturnValue(width);
}


void OutputStream__SetWidth(Bond::CalleeStackFrame &frame)
{
	OutputStreamAdaptor *stream = frame.GetArg<OutputStreamAdaptor *>(0);
	const int width = int(frame.GetArg<int32_t>(1));
	stream->SetWidth(width);
	frame.SetReturnValue(stream);
}


void OutputStream__GetPosition(Bond::CalleeStackFrame &frame)
{
	OutputStreamAdaptor *stream = frame.GetArg<OutputStreamAdaptor *>(0);
	int32_t pos = int32_t(stream->GetStream()->GetPosition());
	frame.SetReturnValue(pos);
}


void OutputStream__SetPosition(Bond::CalleeStackFrame &frame)
{
	OutputStreamAdaptor *stream = frame.GetArg<OutputStreamAdaptor *>(0);
	const int32_t offset = frame.GetArg<int32_t>(1);
	stream->GetStream()->SetPosition(Stream::pos_t(offset));
}


void OutputStream__SetPositionFromEnd(Bond::CalleeStackFrame &frame)
{
	OutputStreamAdaptor *stream = frame.GetArg<OutputStreamAdaptor *>(0);
	const int32_t offset = frame.GetArg<int32_t>(1);
	stream->GetStream()->SetPositionFromEnd(Stream::pos_t(offset));
}


void OutputStream__AddOffset(Bond::CalleeStackFrame &frame)
{
	OutputStreamAdaptor *stream = frame.GetArg<OutputStreamAdaptor *>(0);
	const int32_t value = frame.GetArg<int32_t>(1);
	stream->GetStream()->AddOffset(Stream::pos_t(value));
}


void OutputStream__IsEof(Bond::CalleeStackFrame &frame)
{
	OutputStreamAdaptor *stream = frame.GetArg<OutputStreamAdaptor *>(0);
	const bool isEof = stream->GetStream()->IsEof();
	frame.SetReturnValue(isEof);
}


void StdIn(Bond::CalleeStackFrame &frame)
{
	InputStreamAdaptor &inStream = frame.GetVM().GetStdIn();
	frame.SetReturnValue(&inStream);
}


void StdOut(Bond::CalleeStackFrame &frame)
{
	OutputStreamAdaptor &outStream = frame.GetVM().GetStdOut();
	frame.SetReturnValue(&outStream);
}


void StdErr(Bond::CalleeStackFrame &frame)
{
	OutputStreamAdaptor &errStream = frame.GetVM().GetStdErr();
	frame.SetReturnValue(&errStream);
}

}
