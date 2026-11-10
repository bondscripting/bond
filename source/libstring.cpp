#include "bond/api/libstring.h"
#include "bond/stl/cstdlib.h"
#include "bond/vm/vm.h"
#include "private/utf8.h"
#include <cstring>

namespace Bond
{

static const char *GetStringArg(Bond::StackFrame &frame, size_t index)
{
	const char *str = frame.GetArg<const char *>(index);
#if BOND_RUNTIME_CHECKS_ENABLED
	if (str == nullptr)
	{
		frame.GetVM().RaiseError("Attempt to pass a null string.");
	}
#endif
	return str;
}


void Strlen(Bond::StackFrame &frame)
{
	const char *str = GetStringArg(frame, 0);
	const uint32_t length = uint32_t(strlen(str));
	frame.SetReturnValue(length);
}


void Strcmp(Bond::StackFrame &frame)
{
	const char *str1 = GetStringArg(frame, 0);
	const char *str2 = GetStringArg(frame, 1);
	const uint32_t result = uint32_t(strcmp(str1, str2));
	frame.SetReturnValue(result);
}


void Utf8Length(Bond::StackFrame &frame)
{
	const char *str = GetStringArg(frame, 0);
	size_t remaining = strlen(str);
	uint32_t length = 0;
	while (remaining > 0)
	{
		const Private::Utf8Decoded decoded = Private::DecodeUtf8(str, remaining);
		str += decoded.length;
		remaining -= decoded.length;
		++length;
	}
	frame.SetReturnValue(length);
}


void Utf8IsValid(Bond::StackFrame &frame)
{
	const char *str = GetStringArg(frame, 0);
	size_t remaining = strlen(str);
	bool valid = true;
	while (valid && (remaining > 0))
	{
		const Private::Utf8Decoded decoded = Private::DecodeUtf8(str, remaining);
		str += decoded.length;
		remaining -= decoded.length;
		valid = decoded.valid;
	}
	frame.SetReturnValue(valid);
}


void Utf8Decode(Bond::StackFrame &frame)
{
	const char *str = GetStringArg(frame, 0);
	uint32_t *codePoint = frame.GetArg<uint32_t *>(1);
	uint32_t length = 0;
	if (*str != '\0')
	{
		// Bound the decoder by the string's length so that it never reads past the terminator.
		const Private::Utf8Decoded decoded = Private::DecodeUtf8(str, strlen(str));
		length = uint32_t(decoded.length);
		if (codePoint != nullptr)
		{
			*codePoint = decoded.codePoint;
		}
	}
	frame.SetReturnValue(length);
}


void Utf8Encode(Bond::StackFrame &frame)
{
	const uint32_t codePoint = frame.GetArg<uint32_t>(0);
	char *buffer = frame.GetArg<char *>(1);
	const size_t length = (buffer != nullptr) ?
		Private::EncodeUtf8(codePoint, buffer) :
		Private::GetUtf8EncodedLength(codePoint);
	frame.SetReturnValue(uint32_t(length));
}


void TryParseInt(Bond::StackFrame &frame)
{
	const char *str = frame.GetArg<const char *>(0);
	int32_t *result = frame.GetArg<int32_t *>(1);
	bool success = false;
	if ((str != nullptr) && (result != nullptr))
	{
		const char *end;
		bool outOfRange = false;
		const int64_t value = ParseInt64(str, &end, 10, &outOfRange);
		success = (end != str) && !outOfRange && (value >= BOND_INT_MIN) && (value <= BOND_INT_MAX);
		if (success)
		{
			*result = int32_t(value);
		}
	}
	frame.SetReturnValue(success);
}


void TryParseLong(Bond::StackFrame &frame)
{
	const char *str = frame.GetArg<const char *>(0);
	int64_t *result = frame.GetArg<int64_t *>(1);
	bool success = false;
	if ((str != nullptr) && (result != nullptr))
	{
		const char *end;
		bool outOfRange = false;
		const int64_t value = ParseInt64(str, &end, 10, &outOfRange);
		success = (end != str) && !outOfRange;
		if (success)
		{
			*result = value;
		}
	}
	frame.SetReturnValue(success);
}


void TryParseFloat(Bond::StackFrame &frame)
{
	const char *str = frame.GetArg<const char *>(0);
	float *result = frame.GetArg<float *>(1);
	bool success = false;
	if ((str != nullptr) && (result != nullptr))
	{
		const char *end;
		const float value = ParseFloat(str, &end, nullptr);
		success = end != str;
		if (success)
		{
			*result = value;
		}
	}
	frame.SetReturnValue(success);
}


void TryParseDouble(Bond::StackFrame &frame)
{
	const char *str = frame.GetArg<const char *>(0);
	double *result = frame.GetArg<double *>(1);
	bool success = false;
	if ((str != nullptr) && (result != nullptr))
	{
		const char *end;
		const double value = ParseDouble(str, &end, nullptr);
		success = end != str;
		if (success)
		{
			*result = value;
		}
	}
	frame.SetReturnValue(success);
}

}
