#include "bond/io/stdioinputstream.h"
#include "bond/io/stdiooutputstream.h"
#include "bond/systems/defaultallocator.h"
#include "bond/systems/exception.h"
#include "bond/tools/disassembler.h"

void Disassemble(const char *cboFileName)
{
	try
	{
		Bond::StdioInputStream cboStream(cboFileName);
		Bond::DefaultAllocator allocator;
		Bond::StdOutOutputStream outputStream;
		Bond::Disassembler disassembler(allocator);
		disassembler.Disassemble(cboStream, outputStream);
	}
	catch (const Bond::Exception &e)
	{
		fprintf(stderr, "%s\n", e.GetMessage());
	}
}


int main(int argc, const char *argv[])
{
	if (argc <= 1)
	{
		fprintf(stderr, "No input files specified\n");
		return 1;
	}

	for (int i = 1; i < argc; ++i)
	{
		Disassemble(argv[i]);
	}

	return 0;
}
