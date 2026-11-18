#include "bond/api/libdebug.h"
#include "bond/vm/vm.h"

#if defined(_WIN32) || defined(__CYGWIN__)
// Included last: windows.h defines macros (ERROR, GetMessage) that break Bond headers.
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#if defined(_MSC_VER)
#include <intrin.h>
#endif
#elif defined(__linux__)
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#elif defined(__APPLE__)
#include <csignal>
#include <sys/sysctl.h>
#include <sys/types.h>
#include <unistd.h>
#else
#include <csignal>
#endif

namespace Bond
{

// Returns whether a native debugger is attached to the process.
static bool DebuggerAttached()
{
#if defined(_WIN32) || defined(__CYGWIN__)
	// Qualified, since Bond::Debug::IsDebuggerPresent would otherwise hide it from callers in that
	// namespace.
	return ::IsDebuggerPresent() != 0;

#elif defined(__linux__)
	// A traced process has the PID of its tracer in the TracerPid field, and 0 otherwise.
	bool attached = false;
	FILE *file = fopen("/proc/self/status", "r");
	if (file != nullptr)
	{
		const char FIELD[] = "TracerPid:";
		const size_t FIELD_LENGTH = sizeof(FIELD) - 1;
		char line[256];
		while (fgets(line, int(sizeof(line)), file) != nullptr)
		{
			if (strncmp(line, FIELD, FIELD_LENGTH) == 0)
			{
				attached = strtol(line + FIELD_LENGTH, nullptr, 10) != 0;
				break;
			}
		}
		fclose(file);
	}
	return attached;

#elif defined(__APPLE__)
	int mib[] = {CTL_KERN, KERN_PROC, KERN_PROC_PID, int(getpid())};
	struct kinfo_proc info;
	info.kp_proc.p_flag = 0;
	size_t size = sizeof(info);
	const int result = sysctl(mib, u_int(sizeof(mib) / sizeof(*mib)), &info, &size, nullptr, 0);
	return (result == 0) && ((info.kp_proc.p_flag & P_TRACED) != 0);

#else
	return false;
#endif
}


// Raises a breakpoint exception or signal. Must only be called when a debugger is attached, since
// it otherwise terminates the process.
static void Trap()
{
#if defined(_MSC_VER)
	// Stops in Bond::Debug::Break itself rather than in a system function.
	__debugbreak();
#elif defined(_WIN32) || defined(__CYGWIN__)
	DebugBreak();
#else
	raise(SIGTRAP);
#endif
}


namespace Debug
{

void Break(Bond::StackFrame &frame)
{
	// Inspect frame, or frame.mVm for the whole Bond call stack. See docs/debugging.md.
	if (DebuggerAttached())
	{
		Trap();
	}
}


void IsDebuggerPresent(Bond::StackFrame &frame)
{
	const bool result = DebuggerAttached();
	frame.SetReturnValue(result);
}

}

}
