#include "bond/api/libinclude.h"

namespace Bond
{

#include "private/libinclude_embedded_cpp.h"


#define INCLUDE_FILE_LIST \
	INCLUDE_FILE_ITEM("math.bond",   INCLUDE_MATH_BOND)   \
	INCLUDE_FILE_ITEM("string.bond", INCLUDE_STRING_BOND) \


static const SimpleString INCLUDE_FILE_NAMES[] =
{
#define INCLUDE_FILE_ITEM(name, fileData) SimpleString(name),
	INCLUDE_FILE_LIST
#undef INCLUDE_FILE_ITEM
};


static const FileData INCLUDE_FILE_DATA[] =
{
#define INCLUDE_FILE_ITEM(name, fileData) fileData,
	INCLUDE_FILE_LIST
#undef INCLUDE_FILE_ITEM
};


const MemoryFileLoader::Index INCLUDE_FILE_INDEX(
	INCLUDE_FILE_NAMES,
	INCLUDE_FILE_DATA,
	sizeof(INCLUDE_FILE_NAMES) / sizeof(*INCLUDE_FILE_NAMES));

}
