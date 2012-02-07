#include "bond/opcodes.h"

namespace Bond
{

const char *GetOpCodeMnemonic(OpCode opCode)
{
	static const char *const OPCODE_MNEMONICS[] =
	{
#define BOND_OPCODE_ITEM(opCode, mnemonic, arg) #mnemonic,
		BOND_OPCODE_LIST
#undef BOND_OPCODE_ITEM
	};

	return OPCODE_MNEMONICS[opCode];
}

OpCodeParam GetOpCodeParamType(OpCode opCode)
{
	static const OpCodeParam OPCODE_PARAMS[] =
	{
#define BOND_OPCODE_ITEM(opCode, mnemonic, param) OC_PARAM_ ## param,
		BOND_OPCODE_LIST
#undef BOND_OPCODE_ITEM
	};

	return OPCODE_PARAMS[opCode];
}

}
