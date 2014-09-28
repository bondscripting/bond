#include "bond/io/memoryoutputstream.h"
#include "bond/types/opcodes.h"

namespace Bond
{

const char *GetBondTypeMnemonic(SignatureType signatureType)
{
	static const char *const BOND_TYPE_MNEMONICS[] =
	{
#define BOND_SIGNATURE_TYPE_ITEM(type, bondType, apiType) bondType,
		BOND_SIGNATURE_TYPE_LIST
#undef BOND_SIGNATURE_TYPE_ITEM
	};
	return BOND_TYPE_MNEMONICS[signatureType];
}


char *ExpandBondTypeMnemonic(char *buffer, size_t length, SignatureType signatureType, bu32_t size)
{
	MemoryOutputStream stream(buffer, OutputStream::pos_t(length));
	stream.Print(GetBondTypeMnemonic(SignatureType(signatureType)), size);
	return buffer;
}


const char *GetApiTypeMnemonic(SignatureType signatureType)
{
	static const char *const API_TYPE_MNEMONICS[] =
	{
#define BOND_SIGNATURE_TYPE_ITEM(type, bondType, apiType) apiType,
		BOND_SIGNATURE_TYPE_LIST
#undef BOND_SIGNATURE_TYPE_ITEM
	};
	return API_TYPE_MNEMONICS[signatureType];
}


char *ExpandApiTypeMnemonic(char *buffer, size_t length, SignatureType signatureType, bu32_t size)
{
	MemoryOutputStream stream(buffer, OutputStream::pos_t(length));
	stream.Print(GetApiTypeMnemonic(SignatureType(signatureType)), size);
	return buffer;
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
