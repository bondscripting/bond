#include "bond/api/libio.h"
#include "bond/io/outputstream.h"
#include "bond/io/outputstreamadaptor.h"
#include "bond/systems/allocator.h"
#include "bond/vm/vm.h"

namespace Bond
{

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


void OutputStream__GetFlags(Bond::CalleeStackFrame &frame)
{
	OutputStreamAdaptor *stream = frame.GetArg<OutputStreamAdaptor *>(0);
	const int32_t flags = int32_t(stream->GetFlags());
	frame.SetReturnValue(stream);
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
	frame.SetReturnValue(stream);
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
	frame.SetReturnValue(stream);
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
