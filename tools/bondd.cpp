#include "bond/defaultallocator.h"
#include "bond/defaultfileloader.h"
#include "bond/disassembler.h"
#include "bond/exception.h"
#include "bond/stdouttextwriter.h"
#include <stdio.h>

void Disassemble(const char *cboFileName)
{
	Bond::DefaultAllocator allocator;
	Bond::DefaultFileLoader fileLoader(allocator);
	Bond::FileData cboFile;

	try
	{
		cboFile = fileLoader.LoadFile(cboFileName);
		Bond::StdOutTextWriter writer;
		Bond::Disassembler disassembler(allocator);
		disassembler.Disassemble(writer, static_cast<const Bond::bu8_t *>(cboFile.mData), cboFile.mLength);
	}
	catch (const Bond::Exception &e)
	{
		fprintf(stderr, "%s\n", e.GetMessage());
	}

	fileLoader.DisposeFile(cboFile);
}


int main(int argc, const char *argv[])
{
	for (int i = 1; i < argc; ++i)
	{
		Disassemble(argv[i]);
	}

	return 0;
}
