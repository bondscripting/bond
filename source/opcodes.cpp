#include "bond/opcodes.h"

namespace Bond
{

const char *GetOpCodeMnemonic(OpCode opCode)
{
	static const char *const OPCODE_MNEMONICS[] =
	{
#define BOND_OPCODE_ITEM(opCode, mnemonic) #mnemonic,
		BOND_OPCODE_LIST
#undef BOND_OPCODE_ITEM
	};

	return OPCODE_MNEMONICS[opCode];
}

}

