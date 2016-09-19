#include "bond/api/libio.h"
#include "bond/io/inputstream.h"
#include "bond/io/inputstreamadaptor.h"
#include "bond/io/outputstream.h"
#include "bond/io/outputstreamadaptor.h"
#include "bond/io/streamfactory.h"
#include "bond/systems/allocator.h"
#include "bond/vm/vm.h"

namespace Bond
{

struct InputStreamPair
{
	InputStreamPair(InputStreamHandle &&handle): mAdaptor(handle.get()), mHandle(move(handle)) {}
	InputStreamAdaptor mAdaptor;
	InputStreamHandle mHandle;
};

typedef WrapperCollectable<InputStreamPair> InputStreamCollectable;

struct OutputStreamPair
{
	OutputStreamPair(OutputStreamHandle &&handle): mAdaptor(handle.get()), mHandle(move(handle)) {}
	OutputStreamAdaptor mAdaptor;
	OutputStreamHandle mHandle;
};

typedef WrapperCollectable<OutputStreamPair> OutputStreamCollectable;

void InputStream__Close(Bond::StackFrame &frame)
{
	InputStreamAdaptor *stream = frame.GetArg<InputStreamAdaptor *>(0);
	stream->GetStream()->Close();
}


void InputStream__ScanStr(Bond::StackFrame &frame)
{
	InputStreamAdaptor *stream = frame.GetArg<InputStreamAdaptor *>(0);
	char *str = frame.GetArg<char *>(1);
	const uint32_t bufferSize = frame.GetArg<uint32_t>(2);
	stream->ScanStr(str, bufferSize);
}


void InputStream__ScanB(Bond::StackFrame &frame)
{
	InputStreamAdaptor *stream = frame.GetArg<InputStreamAdaptor *>(0);
	const bool value = stream->ScanBool();
	frame.SetReturnValue(value);
}


void InputStream__ScanC(Bond::StackFrame &frame)
{
	InputStreamAdaptor *stream = frame.GetArg<InputStreamAdaptor *>(0);
	const int8_t value = stream->ScanChar();
	frame.SetReturnValue(value);
}


void InputStream__ScanI(Bond::StackFrame &frame)
{
	InputStreamAdaptor *stream = frame.GetArg<InputStreamAdaptor *>(0);
	const int32_t value = stream->ScanInt();
	frame.SetReturnValue(value);
}


void InputStream__ScanUI(Bond::StackFrame &frame)
{
	InputStreamAdaptor *stream = frame.GetArg<InputStreamAdaptor *>(0);
	const uint32_t value = stream->ScanUInt();
	frame.SetReturnValue(value);
}


void InputStream__ScanL(Bond::StackFrame &frame)
{
	InputStreamAdaptor *stream = frame.GetArg<InputStreamAdaptor *>(0);
	const int64_t value = stream->ScanLong();
	frame.SetReturnValue(value);
}


void InputStream__ScanUL(Bond::StackFrame &frame)
{
	InputStreamAdaptor *stream = frame.GetArg<InputStreamAdaptor *>(0);
	const uint64_t value = stream->ScanULong();
	frame.SetReturnValue(value);
}


void InputStream__ScanF(Bond::StackFrame &frame)
{
	InputStreamAdaptor *stream = frame.GetArg<InputStreamAdaptor *>(0);
	const double value = stream->ScanDouble();
	frame.SetReturnValue(float(value));
}


void InputStream__ScanD(Bond::StackFrame &frame)
{
	InputStreamAdaptor *stream = frame.GetArg<InputStreamAdaptor *>(0);
	const double value = stream->ScanDouble();
	frame.SetReturnValue(value);
}


void InputStream__Get(Bond::StackFrame &frame)
{
	InputStreamAdaptor *stream = frame.GetArg<InputStreamAdaptor *>(0);
	const int value = stream->GetStream()->Read();
	frame.SetReturnValue(int32_t(value));
}


void InputStream__Read(Bond::StackFrame &frame)
{
	InputStreamAdaptor *stream = frame.GetArg<InputStreamAdaptor *>(0);
	uint8_t *bytes = frame.GetArg<uint8_t *>(1);
	const size_t numBytes = size_t(frame.GetArg<uint32_t>(2));
	const size_t numRead = stream->GetStream()->Read(bytes, numBytes);
	frame.SetReturnValue(uint32_t(numRead));
}


void InputStream__GetFlags(Bond::StackFrame &frame)
{
	InputStreamAdaptor *stream = frame.GetArg<InputStreamAdaptor *>(0);
	const int32_t flags = int32_t(stream->GetFlags());
	frame.SetReturnValue(flags);
}


void InputStream__SetFlags(Bond::StackFrame &frame)
{
	InputStreamAdaptor *stream = frame.GetArg<InputStreamAdaptor *>(0);
	const int flags = int(frame.GetArg<int32_t>(1));
	stream->SetFlags(flags);
	frame.SetReturnValue(stream);
}


void InputStream__UnsetFlags(Bond::StackFrame &frame)
{
	InputStreamAdaptor *stream = frame.GetArg<InputStreamAdaptor *>(0);
	const int flags = int(frame.GetArg<int32_t>(1));
	stream->UnsetFlags(flags);
	frame.SetReturnValue(stream);
}


void InputStream__ClearFlags(Bond::StackFrame &frame)
{
	InputStreamAdaptor *stream = frame.GetArg<InputStreamAdaptor *>(0);
	stream->ClearFlags();
	frame.SetReturnValue(stream);
}


void InputStream__GetPosition(Bond::StackFrame &frame)
{
	InputStreamAdaptor *stream = frame.GetArg<InputStreamAdaptor *>(0);
	int32_t pos = int32_t(stream->GetStream()->GetPosition());
	frame.SetReturnValue(pos);
}


void InputStream__GetEndPosition(Bond::StackFrame &frame)
{
	InputStreamAdaptor *stream = frame.GetArg<InputStreamAdaptor *>(0);
	int32_t pos = int32_t(stream->GetStream()->GetEndPosition());
	frame.SetReturnValue(pos);
}


void InputStream__SetPosition(Bond::StackFrame &frame)
{
	InputStreamAdaptor *stream = frame.GetArg<InputStreamAdaptor *>(0);
	const int32_t offset = frame.GetArg<int32_t>(1);
	stream->GetStream()->SetPosition(Stream::pos_t(offset));
}


void InputStream__SetPositionFromEnd(Bond::StackFrame &frame)
{
	InputStreamAdaptor *stream = frame.GetArg<InputStreamAdaptor *>(0);
	const int32_t offset = frame.GetArg<int32_t>(1);
	stream->GetStream()->SetPositionFromEnd(Stream::pos_t(offset));
}


void InputStream__AddOffset(Bond::StackFrame &frame)
{
	InputStreamAdaptor *stream = frame.GetArg<InputStreamAdaptor *>(0);
	const int32_t value = frame.GetArg<int32_t>(1);
	stream->GetStream()->AddOffset(Stream::pos_t(value));
}


void InputStream__IsEof(Bond::StackFrame &frame)
{
	InputStreamAdaptor *stream = frame.GetArg<InputStreamAdaptor *>(0);
	const bool isEof = stream->GetStream()->IsEof();
	frame.SetReturnValue(isEof);
}


void OutputStream__Close(Bond::StackFrame &frame)
{
	OutputStreamAdaptor *stream = frame.GetArg<OutputStreamAdaptor *>(0);
	stream->GetStream()->Close();
}


void OutputStream__Flush(Bond::StackFrame &frame)
{
	OutputStreamAdaptor *stream = frame.GetArg<OutputStreamAdaptor *>(0);
	stream->GetStream()->Flush();
}


void OutputStream__PrintStr(Bond::StackFrame &frame)
{
	OutputStreamAdaptor *stream = frame.GetArg<OutputStreamAdaptor *>(0);
	const char *str = frame.GetArg<const char *>(1);
	stream->Print(str);
	frame.SetReturnValue(stream);
}


void OutputStream__PrintB(Bond::StackFrame &frame)
{
	OutputStreamAdaptor *stream = frame.GetArg<OutputStreamAdaptor *>(0);
	const bool value = frame.GetArg<bool>(1);
	stream->Print(value);
	frame.SetReturnValue(stream);
}


void OutputStream__PrintC(Bond::StackFrame &frame)
{
	OutputStreamAdaptor *stream = frame.GetArg<OutputStreamAdaptor *>(0);
	const char value = char(frame.GetArg<int8_t>(1));
	stream->Print(value);
	frame.SetReturnValue(stream);
}


void OutputStream__PrintI(Bond::StackFrame &frame)
{
	OutputStreamAdaptor *stream = frame.GetArg<OutputStreamAdaptor *>(0);
	const int32_t value = frame.GetArg<int32_t>(1);
	stream->Print(value);
	frame.SetReturnValue(stream);
}


void OutputStream__PrintUI(Bond::StackFrame &frame)
{
	OutputStreamAdaptor *stream = frame.GetArg<OutputStreamAdaptor *>(0);
	const uint32_t value = frame.GetArg<uint32_t>(1);
	stream->Print(value);
	frame.SetReturnValue(stream);
}


void OutputStream__PrintL(Bond::StackFrame &frame)
{
	OutputStreamAdaptor *stream = frame.GetArg<OutputStreamAdaptor *>(0);
	const int64_t value = frame.GetArg<int64_t>(1);
	stream->Print(value);
	frame.SetReturnValue(stream);
}


void OutputStream__PrintUL(Bond::StackFrame &frame)
{
	OutputStreamAdaptor *stream = frame.GetArg<OutputStreamAdaptor *>(0);
	const uint64_t value = frame.GetArg<uint64_t>(1);
	stream->Print(value);
	frame.SetReturnValue(stream);
}


void OutputStream__PrintF(Bond::StackFrame &frame)
{
	OutputStreamAdaptor *stream = frame.GetArg<OutputStreamAdaptor *>(0);
	const float value = frame.GetArg<float>(1);
	stream->Print(double(value));
	frame.SetReturnValue(stream);
}


void OutputStream__PrintD(Bond::StackFrame &frame)
{
	OutputStreamAdaptor *stream = frame.GetArg<OutputStreamAdaptor *>(0);
	const double value = frame.GetArg<double>(1);
	stream->Print(value);
	frame.SetReturnValue(stream);
}


void OutputStream__Put(Bond::StackFrame &frame)
{
	OutputStreamAdaptor *stream = frame.GetArg<OutputStreamAdaptor *>(0);
	const char byte = char(frame.GetArg<int8_t>(1));
	stream->GetStream()->Write(byte);
	frame.SetReturnValue(stream);
}


void OutputStream__Write(Bond::StackFrame &frame)
{
	OutputStreamAdaptor *stream = frame.GetArg<OutputStreamAdaptor *>(0);
	const uint8_t *bytes = frame.GetArg<const uint8_t *>(1);
	const size_t numBytes = size_t(frame.GetArg<uint32_t>(2));
	stream->GetStream()->Write(bytes, numBytes);
	frame.SetReturnValue(stream);
}


void OutputStream__GetFlags(Bond::StackFrame &frame)
{
	OutputStreamAdaptor *stream = frame.GetArg<OutputStreamAdaptor *>(0);
	const int32_t flags = int32_t(stream->GetFlags());
	frame.SetReturnValue(flags);
}


void OutputStream__SetFlags(Bond::StackFrame &frame)
{
	OutputStreamAdaptor *stream = frame.GetArg<OutputStreamAdaptor *>(0);
	const int flags = int(frame.GetArg<int32_t>(1));
	stream->SetFlags(flags);
	frame.SetReturnValue(stream);
}


void OutputStream__UnsetFlags(Bond::StackFrame &frame)
{
	OutputStreamAdaptor *stream = frame.GetArg<OutputStreamAdaptor *>(0);
	const int flags = int(frame.GetArg<int32_t>(1));
	stream->UnsetFlags(flags);
	frame.SetReturnValue(stream);
}


void OutputStream__ClearFlags(Bond::StackFrame &frame)
{
	OutputStreamAdaptor *stream = frame.GetArg<OutputStreamAdaptor *>(0);
	stream->ClearFlags();
	frame.SetReturnValue(stream);
}


void OutputStream__GetPrecision(Bond::StackFrame &frame)
{
	OutputStreamAdaptor *stream = frame.GetArg<OutputStreamAdaptor *>(0);
	const int32_t precision = int32_t(stream->GetPrecision());
	frame.SetReturnValue(precision);
}


void OutputStream__SetPrecision(Bond::StackFrame &frame)
{
	OutputStreamAdaptor *stream = frame.GetArg<OutputStreamAdaptor *>(0);
	const int precision = int(frame.GetArg<int32_t>(1));
	stream->SetPrecision(precision);
	frame.SetReturnValue(stream);
}


void OutputStream__GetWidth(Bond::StackFrame &frame)
{
	OutputStreamAdaptor *stream = frame.GetArg<OutputStreamAdaptor *>(0);
	const int32_t width = int32_t(stream->GetWidth());
	frame.SetReturnValue(width);
}


void OutputStream__SetWidth(Bond::StackFrame &frame)
{
	OutputStreamAdaptor *stream = frame.GetArg<OutputStreamAdaptor *>(0);
	const int width = int(frame.GetArg<int32_t>(1));
	stream->SetWidth(width);
	frame.SetReturnValue(stream);
}


void OutputStream__GetPosition(Bond::StackFrame &frame)
{
	OutputStreamAdaptor *stream = frame.GetArg<OutputStreamAdaptor *>(0);
	int32_t pos = int32_t(stream->GetStream()->GetPosition());
	frame.SetReturnValue(pos);
}


void OutputStream__GetEndPosition(Bond::StackFrame &frame)
{
	OutputStreamAdaptor *stream = frame.GetArg<OutputStreamAdaptor *>(0);
	int32_t pos = int32_t(stream->GetStream()->GetEndPosition());
	frame.SetReturnValue(pos);
}


void OutputStream__SetPosition(Bond::StackFrame &frame)
{
	OutputStreamAdaptor *stream = frame.GetArg<OutputStreamAdaptor *>(0);
	const int32_t offset = frame.GetArg<int32_t>(1);
	stream->GetStream()->SetPosition(Stream::pos_t(offset));
}


void OutputStream__SetPositionFromEnd(Bond::StackFrame &frame)
{
	OutputStreamAdaptor *stream = frame.GetArg<OutputStreamAdaptor *>(0);
	const int32_t offset = frame.GetArg<int32_t>(1);
	stream->GetStream()->SetPositionFromEnd(Stream::pos_t(offset));
}


void OutputStream__AddOffset(Bond::StackFrame &frame)
{
	OutputStreamAdaptor *stream = frame.GetArg<OutputStreamAdaptor *>(0);
	const int32_t value = frame.GetArg<int32_t>(1);
	stream->GetStream()->AddOffset(Stream::pos_t(value));
}


void OutputStream__IsEof(Bond::StackFrame &frame)
{
	OutputStreamAdaptor *stream = frame.GetArg<OutputStreamAdaptor *>(0);
	const bool isEof = stream->GetStream()->IsEof();
	frame.SetReturnValue(isEof);
}


void StdIn(Bond::StackFrame &frame)
{
	InputStreamAdaptor &inStream = frame.GetVM().GetStdIn();
	frame.SetReturnValue(&inStream);
}


void StdOut(Bond::StackFrame &frame)
{
	OutputStreamAdaptor &outStream = frame.GetVM().GetStdOut();
	frame.SetReturnValue(&outStream);
}


void StdErr(Bond::StackFrame &frame)
{
	OutputStreamAdaptor &errStream = frame.GetVM().GetStdErr();
	frame.SetReturnValue(&errStream);
}


void CreateInputStream(Bond::StackFrame &frame)
{
	StreamFactory &streamFactory = *frame.GetVM().GetStreamFactory();
	Allocator &allocator = frame.GetVM().GetAllocator();
	const char *fileName = frame.GetArg<const char *>(0);
	auto streamHandle = streamFactory.CreateInputStream(fileName);
	void *adaptor = nullptr;
	if (streamHandle)
	{
		adaptor = allocator.AllocObject<InputStreamPair>(move(streamHandle));
	}
	frame.SetReturnValue(adaptor);
}


InputStreamAdaptor *CreateInputStreamCollected(
	StreamFactory &streamFactory,
	Allocator &allocator,
	Collector &collector,
	const char *fileName)
{
	InputStreamAdaptor *adaptor = nullptr;
	auto streamHandle = streamFactory.CreateInputStream(fileName);
	if (streamHandle)
	{
		auto collectableHandle = allocator.AllocOwnedObject<InputStreamCollectable>(move(streamHandle));
		adaptor = &collectableHandle->GetContent().mAdaptor;
		collector.Register(move(collectableHandle));
	}
	return adaptor;
}


void CreateInputStreamCollected(Bond::StackFrame &frame)
{
	VM &vm = frame.GetVM();
	StreamFactory &streamFactory = *vm.GetStreamFactory();
	Allocator &allocator = vm.GetAllocator();
	Collector &collector = vm.GetCallerStackFrame().GetCollector();
	const char *fileName = frame.GetArg<const char *>(0);
	auto adaptor = CreateInputStreamCollected(streamFactory, allocator, collector, fileName);
	frame.SetReturnValue(adaptor);
}


void CreateInputStreamWithCollector(Bond::StackFrame &frame)
{
	VM &vm = frame.GetVM();
	StreamFactory &streamFactory = *vm.GetStreamFactory();
	Allocator &allocator = vm.GetAllocator();
	Collector &collector = vm.GetCallerStackFrame().GetCollector();
	const char *fileName = frame.GetArg<const char *>(0);
	auto adaptor = CreateInputStreamCollected(streamFactory, allocator, collector, fileName);
	frame.SetReturnValue(adaptor);
}


void FreeInputStream(Bond::StackFrame &frame)
{
	// Let the handle do the clean up.
	InputStreamPair *stream = frame.GetArg<InputStreamPair *>(0);
	Allocator::ObjectHandle<InputStreamPair>(frame.GetVM().GetAllocator(), stream);
}


void CreateOutputStream(Bond::StackFrame &frame)
{
	StreamFactory &streamFactory = *frame.GetVM().GetStreamFactory();
	Allocator &allocator = frame.GetVM().GetAllocator();
	const char *fileName = frame.GetArg<const char *>(0);
	const bool append = frame.GetArg<bool>(1);
	auto streamHandle = streamFactory.CreateOutputStream(fileName, append);
	void *adaptor = nullptr;
	if (streamHandle)
	{
		adaptor = allocator.AllocObject<OutputStreamPair>(move(streamHandle));
	}
	frame.SetReturnValue(adaptor);
}


OutputStreamAdaptor *CreateOutputStreamCollected(
	StreamFactory &streamFactory,
	Allocator &allocator,
	Collector &collector,
	const char *fileName,
	bool append)
{
	OutputStreamAdaptor *adaptor = nullptr;
	auto streamHandle = streamFactory.CreateOutputStream(fileName, append);
	if (streamHandle)
	{
		auto collectableHandle = allocator.AllocOwnedObject<OutputStreamCollectable>(move(streamHandle));
		adaptor = &collectableHandle->GetContent().mAdaptor;
		collector.Register(move(collectableHandle));
	}
	return adaptor;
}


void CreateOutputStreamCollected(Bond::StackFrame &frame)
{
	VM &vm = frame.GetVM();
	StreamFactory &streamFactory = *vm.GetStreamFactory();
	Allocator &allocator = vm.GetAllocator();
	Collector &collector = vm.GetCallerStackFrame().GetCollector();
	const char *fileName = frame.GetArg<const char *>(0);
	const bool append = frame.GetArg<bool>(1);
	auto adaptor = CreateOutputStreamCollected(streamFactory, allocator, collector, fileName, append);
	frame.SetReturnValue(adaptor);
}


void CreateOutputStreamWithCollector(Bond::StackFrame &frame)
{
	VM &vm = frame.GetVM();
	StreamFactory &streamFactory = *vm.GetStreamFactory();
	Allocator &allocator = vm.GetAllocator();
	Collector &collector = vm.GetCallerStackFrame().GetCollector();
	const char *fileName = frame.GetArg<const char *>(0);
	const bool append = frame.GetArg<bool>(1);
	auto adaptor = CreateOutputStreamCollected(streamFactory, allocator, collector, fileName, append);
	frame.SetReturnValue(adaptor);
}

void FreeOutputStream(Bond::StackFrame &frame)
{
	// Let the handle do the clean up.
	OutputStreamPair *stream = frame.GetArg<OutputStreamPair *>(0);
	Allocator::ObjectHandle<OutputStreamPair>(frame.GetVM().GetAllocator(), stream);
}

}
