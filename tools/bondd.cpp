#include "bond/defaultallocator.h"
#include "bond/defaultfileloader.h"
#include "bond/disassembler.h"
#include "bond/stdouttextwriter.h"

void Disassemble(const char *cboFileName)
{
	Bond::DefaultAllocator allocator;
	Bond::DefaultFileLoader fileLoader(allocator);
	Bond::FileData cboFile = fileLoader.LoadFile(cboFileName);
	if (cboFile.mData != NULL)
	{
		Bond::StdOutTextWriter writer;
		Bond::Disassembler disassembler(allocator);
		disassembler.Disassemble(writer, cboFile.mData, cboFile.mLength);
		fileLoader.DisposeFile(cboFile);
	}
}


int main(int argc, const char *argv[])
{
	for (int i = 1; i < argc; ++i)
	{
		Disassemble(argv[i]);
	}

	return 0;
}
