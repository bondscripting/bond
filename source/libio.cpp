#include "bond/api/libio.h"
#include "bond/io/outputstream.h"
#include "bond/systems/allocator.h"
#include "bond/vm/vm.h"

namespace Bond
{

void OutputStream__PrintStr(Bond::CalleeStackFrame &frame)
{
	OutputStream *stream = frame.GetArg<OutputStream *>(0);
	const char *str = frame.GetArg<const char *>(1);
	stream->Print("%s", str);
	frame.SetReturnValue(stream);
}


void OutputStream__PrintB(Bond::CalleeStackFrame &frame)
{
	OutputStream *stream = frame.GetArg<OutputStream *>(0);
	const bool value = frame.GetArg<bool>(1);
	stream->Print("%s", value ? "true" : "false");
	frame.SetReturnValue(stream);
}


void OutputStream__PrintC(Bond::CalleeStackFrame &frame)
{
	OutputStream *stream = frame.GetArg<OutputStream *>(0);
	const int8_t value = frame.GetArg<int8_t>(1);
	stream->Print("%c", char(value));
	frame.SetReturnValue(stream);
}


void OutputStream__PrintI(Bond::CalleeStackFrame &frame)
{
	OutputStream *stream = frame.GetArg<OutputStream *>(0);
	const int32_t value = frame.GetArg<int32_t>(1);
	stream->Print("%" BOND_PRId32, value);
	frame.SetReturnValue(stream);
}


void OutputStream__PrintUI(Bond::CalleeStackFrame &frame)
{
	OutputStream *stream = frame.GetArg<OutputStream *>(0);
	const uint32_t value = frame.GetArg<uint32_t>(1);
	stream->Print("%" BOND_PRIu32, value);
	frame.SetReturnValue(stream);
}


void OutputStream__PrintL(Bond::CalleeStackFrame &frame)
{
	OutputStream *stream = frame.GetArg<OutputStream *>(0);
	const int64_t value = frame.GetArg<int64_t>(1);
	stream->Print("%" BOND_PRId64, value);
	frame.SetReturnValue(stream);
}


void OutputStream__PrintUL(Bond::CalleeStackFrame &frame)
{
	OutputStream *stream = frame.GetArg<OutputStream *>(0);
	const uint64_t value = frame.GetArg<uint64_t>(1);
	stream->Print("%" BOND_PRIu64, value);
	frame.SetReturnValue(stream);
}


void OutputStream__PrintF(Bond::CalleeStackFrame &frame)
{
	OutputStream *stream = frame.GetArg<OutputStream *>(0);
	const float value = frame.GetArg<float>(1);
	stream->Print("%f", value);
	frame.SetReturnValue(stream);
}


void OutputStream__PrintD(Bond::CalleeStackFrame &frame)
{
	OutputStream *stream = frame.GetArg<OutputStream *>(0);
	const double value = frame.GetArg<double>(1);
	stream->Print("%f", value);
	frame.SetReturnValue(stream);
}


void OutputStream__GetPosition(Bond::CalleeStackFrame &frame)
{
	OutputStream *stream = frame.GetArg<OutputStream *>(0);
	int32_t pos = int32_t(stream->GetPosition());
	frame.SetReturnValue(pos);
}


void OutputStream__SetPosition(Bond::CalleeStackFrame &frame)
{
	OutputStream *stream = frame.GetArg<OutputStream *>(0);
	const int32_t offset = frame.GetArg<int32_t>(1);
	stream->SetPosition(Stream::pos_t(offset));
}


void OutputStream__SetPositionFromEnd(Bond::CalleeStackFrame &frame)
{
	OutputStream *stream = frame.GetArg<OutputStream *>(0);
	const int32_t offset = frame.GetArg<int32_t>(1);
	stream->SetPositionFromEnd(Stream::pos_t(offset));
}


void OutputStream__AddOffset(Bond::CalleeStackFrame &frame)
{
	OutputStream *stream = frame.GetArg<OutputStream *>(0);
	const int32_t value = frame.GetArg<int32_t>(1);
	stream->AddOffset(Stream::pos_t(value));
}


void StdOut(Bond::CalleeStackFrame &frame)
{
	OutputStream *outStream = frame.GetVM().GetStdOut();
	frame.SetReturnValue(outStream);
}


void StdErr(Bond::CalleeStackFrame &frame)
{
	OutputStream *errStream = frame.GetVM().GetStdErr();
	frame.SetReturnValue(errStream);
}

}
