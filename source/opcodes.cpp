#include "bond/opcodes.h"

namespace Bond
{

const char *GetSignatureTypeMnemonic(SignatureType signatureType)
{
	static const char *const SIGNATURE_TYPE_MNEMONICS[] =
	{
#define BOND_SIGNATURE_TYPE_ITEM(type, mnemonic) mnemonic,
		BOND_SIGNATURE_TYPE_LIST
#undef BOND_SIGNATURE_TYPE_ITEM
	};
	return SIGNATURE_TYPE_MNEMONICS[signatureType];
}


const char *GetOpCodeMnemonic(OpCode opCode)
{
	static const char *const OPCODE_MNEMONICS[] =
	{
#define BOND_OPCODE_ITEM(opCode, mnemonic, param, stackDelta) #mnemonic,
		BOND_OPCODE_LIST
#undef BOND_OPCODE_ITEM
	};
	return OPCODE_MNEMONICS[opCode];
}

OpCodeParam GetOpCodeParamType(OpCode opCode)
{
	static const OpCodeParam OPCODE_PARAMS[] =
	{
#define BOND_OPCODE_ITEM(opCode, mnemonic, param, stackDelta) OC_PARAM_ ## param,
		BOND_OPCODE_LIST
#undef BOND_OPCODE_ITEM
	};
	return OPCODE_PARAMS[opCode];
}


bi32_t GetStackDelta(OpCode opCode)
{
	static const bi32_t OPCODE_STACK_DELTAS[] =
	{
#define BOND_OPCODE_ITEM(opCode, mnemonic, param, stackDelta) stackDelta * BOND_SLOT_SIZE,
		BOND_OPCODE_LIST
#undef BOND_OPCODE_ITEM
	};
	return OPCODE_STACK_DELTAS[opCode];
}

}
