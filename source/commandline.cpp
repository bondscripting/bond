#include "bond/tools/commandline.h"
#include <cstdio>

#if defined(_WIN32)
// Included last: windows.h defines macros (ERROR, GetMessage) that break Bond headers.
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <shellapi.h>
#endif

namespace Bond
{

#if defined(_WIN32)

namespace
{

// Owns the array returned by CommandLineToArgvW.
class WideArgvHandle
{
public:
	explicit WideArgvHandle(LPWSTR *argv): mArgv(argv) {}
	~WideArgvHandle() { if (mArgv != nullptr) LocalFree(mArgv); }
	WideArgvHandle(const WideArgvHandle &other) = delete;
	WideArgvHandle &operator=(const WideArgvHandle &other) = delete;
	LPWSTR *Get() const { return mArgv; }

private:
	LPWSTR *mArgv;
};

}


CommandLineArgs::CommandLineArgs(Allocator &allocator, int argc, const char *argv[]):
	mArgv(argv),
	mArgc(argc)
{
	int wideArgc = 0;
	WideArgvHandle wideArgv(CommandLineToArgvW(GetCommandLineW(), &wideArgc));
	if ((wideArgv.Get() == nullptr) || (wideArgc <= 0))
	{
		return;
	}

	// Measure the UTF-8 length of each argument, including its terminating null.
	size_t charCount = 0;
	for (int i = 0; i < wideArgc; ++i)
	{
		const int length = WideCharToMultiByte(CP_UTF8, 0, wideArgv.Get()[i], -1, nullptr, 0, nullptr, nullptr);
		if (length <= 0)
		{
			return;
		}
		charCount += size_t(length);
	}

	// One block holds the pointer array, including a terminating null pointer, then the strings.
	const size_t pointerBytes = (size_t(wideArgc) + 1) * sizeof(const char *);
	Allocator::Handle<char> buffer = allocator.AllocOwned<char>(pointerBytes + charCount);
	const char **pointers = reinterpret_cast<const char **>(buffer.get());
	char *chars = buffer.get() + pointerBytes;
	size_t remaining = charCount;

	for (int i = 0; i < wideArgc; ++i)
	{
		const int length = WideCharToMultiByte(CP_UTF8, 0, wideArgv.Get()[i], -1, chars, int(remaining), nullptr, nullptr);
		if (length <= 0)
		{
			return;
		}
		pointers[i] = chars;
		chars += length;
		remaining -= size_t(length);
	}
	pointers[wideArgc] = nullptr;

	mBuffer = Bond::move(buffer);
	mArgv = pointers;
	mArgc = wideArgc;
}


Utf8ConsoleOutput::Utf8ConsoleOutput():
	mOriginalCodePage(GetConsoleOutputCP())
{
	// GetConsoleOutputCP returns 0 when the process has no console.
	if ((mOriginalCodePage != 0) && (mOriginalCodePage != CP_UTF8))
	{
		SetConsoleOutputCP(CP_UTF8);
	}
}


Utf8ConsoleOutput::~Utf8ConsoleOutput()
{
	if ((mOriginalCodePage != 0) && (mOriginalCodePage != CP_UTF8))
	{
		fflush(stdout);
		fflush(stderr);
		SetConsoleOutputCP(mOriginalCodePage);
	}
}

#else

CommandLineArgs::CommandLineArgs(Allocator &, int argc, const char *argv[]):
	mArgv(argv),
	mArgc(argc)
{}


Utf8ConsoleOutput::Utf8ConsoleOutput():
	mOriginalCodePage(0)
{}


Utf8ConsoleOutput::~Utf8ConsoleOutput()
{}

#endif

}
