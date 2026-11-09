#include "bond/io/stdiofilehandle.h"

#if defined(_WIN32)
// Included last: windows.h defines macros (ERROR, GetMessage) that break Bond headers.
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

namespace Bond
{

namespace Private
{

#if defined(_WIN32)

FILE *OpenStdioFile(const char *fileName, const char *mode)
{
	// The mode is ASCII, so it can be widened one char at a time.
	const size_t MAX_MODE_LENGTH = 16;
	wchar_t wideMode[MAX_MODE_LENGTH];
	size_t modeLength = 0;
	while (mode[modeLength] != '\0')
	{
		if ((modeLength >= (MAX_MODE_LENGTH - 1)) || ((mode[modeLength] & 0x80) != 0))
		{
			return nullptr;
		}
		wideMode[modeLength] = wchar_t(mode[modeLength]);
		++modeLength;
	}
	wideMode[modeLength] = L'\0';

	// A UTF-8 name never needs more UTF-16 code units than it has bytes, so a buffer of
	// BOND_MAX_PATH_LENGTH units holds any name within the limit. MultiByteToWideChar returns 0
	// if the name is not valid UTF-8 or does not fit.
	wchar_t wideName[BOND_MAX_PATH_LENGTH];
	if (MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, fileName, -1, wideName, int(BOND_MAX_PATH_LENGTH)) <= 0)
	{
		return nullptr;
	}

#if defined(_MSC_VER)
	FILE *file = nullptr;
	return (_wfopen_s(&file, wideName, wideMode) == 0) ? file : nullptr;
#else
	return _wfopen(wideName, wideMode);
#endif
}

#else

FILE *OpenStdioFile(const char *fileName, const char *mode)
{
	return fopen(fileName, mode);
}

#endif

}

}
