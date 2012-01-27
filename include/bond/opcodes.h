#ifndef BOND_OPCODES_H
#define BOND_OPCODES_H

namespace Bond
{

#define BOND_OPCODE_LIST \
	/* Opcode, mnemonic */                   \
	BOND_OPCODE_ITEM(CONSTC,     constc)     \
	BOND_OPCODE_ITEM(CONSTUC,    constuc)    \
	BOND_OPCODE_ITEM(CONSTS,     consts)     \
	BOND_OPCODE_ITEM(CONSTUS,    constus)    \
	BOND_OPCODE_ITEM(CONST32,    const32)    \
	BOND_OPCODE_ITEM(CONST64,    const64)    \
	BOND_OPCODE_ITEM(CONSTI_N2,  consti_n2)  \
	BOND_OPCODE_ITEM(CONSTI_N1,  consti_n1)  \
	BOND_OPCODE_ITEM(CONSTI_0,   consti_0)   \
	BOND_OPCODE_ITEM(CONSTI_1,   consti_1)   \
	BOND_OPCODE_ITEM(CONSTI_2,   consti_2)   \
	BOND_OPCODE_ITEM(CONSTI_3,   consti_3)   \
	BOND_OPCODE_ITEM(CONSTI_4,   consti_4)   \
	BOND_OPCODE_ITEM(CONSTI_5,   consti_5)   \
	BOND_OPCODE_ITEM(CONSTI_6,   consti_6)   \
	BOND_OPCODE_ITEM(CONSTI_7,   consti_7)   \
	BOND_OPCODE_ITEM(CONSTI_8,   consti_8)   \
	BOND_OPCODE_ITEM(CONSTL_N1,  constl_n1)  \
	BOND_OPCODE_ITEM(CONSTL_0,   constl_0)   \
	BOND_OPCODE_ITEM(CONSTL_1,   constl_1)   \
	BOND_OPCODE_ITEM(CONSTF_N2,  constf_n2)  \
	BOND_OPCODE_ITEM(CONSTF_N1,  constf_n1)  \
	BOND_OPCODE_ITEM(CONSTF_NH,  constf_nh)  \
	BOND_OPCODE_ITEM(CONSTF_0,   constf_0)   \
	BOND_OPCODE_ITEM(CONSTF_H,   constf_h)   \
	BOND_OPCODE_ITEM(CONSTF_1,   constf_1)   \
	BOND_OPCODE_ITEM(CONSTF_2,   constf_2)   \
	BOND_OPCODE_ITEM(CONSTD_N2,  constd_n2)  \
	BOND_OPCODE_ITEM(CONSTD_N1,  constd_n1)  \
	BOND_OPCODE_ITEM(CONSTD_NH,  constd_nh)  \
	BOND_OPCODE_ITEM(CONSTD_0,   constd_0)   \
	BOND_OPCODE_ITEM(CONSTD_H,   constd_h)   \
	BOND_OPCODE_ITEM(CONSTD_1,   constd_1)   \
	BOND_OPCODE_ITEM(CONSTD_2,   constd_2)   \
	                                         \
	BOND_OPCODE_ITEM(LOADFP,     loadfp)     \
	BOND_OPCODE_ITEM(LOADFPW,    loadfpw)    \
	BOND_OPCODE_ITEM(LOADEA,     loadea)     \
	BOND_OPCODE_ITEM(LOADC,      loadc)      \
	BOND_OPCODE_ITEM(LOADUC,     loaduc)     \
	BOND_OPCODE_ITEM(LOADS,      loads)      \
	BOND_OPCODE_ITEM(LOADUS,     loadus)     \
	BOND_OPCODE_ITEM(LOAD32,     load32)     \
	BOND_OPCODE_ITEM(LOAD64,     load64)     \
	                                         \
	BOND_OPCODE_ITEM(STOREC,     storec)     \
	BOND_OPCODE_ITEM(STORES,     stores)     \
	BOND_OPCODE_ITEM(STORE32,    store32)    \
	BOND_OPCODE_ITEM(STORE64,    store64)    \
	                                         \
	BOND_OPCODE_ITEM(PUSHC,      pushc)      \
	BOND_OPCODE_ITEM(PUSHCW,     pushcw)     \
	BOND_OPCODE_ITEM(PUSHUC,     pushuc)     \
	BOND_OPCODE_ITEM(PUSHUCW,    pushucw)    \
	BOND_OPCODE_ITEM(PUSHS,      pushs)      \
	BOND_OPCODE_ITEM(PUSHSW,     pushsw)     \
	BOND_OPCODE_ITEM(PUSHUS,     pushus)     \
	BOND_OPCODE_ITEM(PUSHUSW,    pushusw)    \
	BOND_OPCODE_ITEM(PUSH32,     push32)     \
	BOND_OPCODE_ITEM(PUSH32W,    push32w)    \
	BOND_OPCODE_ITEM(PUSH32_A0,  push32_a0)  \
	BOND_OPCODE_ITEM(PUSH32_A1,  push32_a1)  \
	BOND_OPCODE_ITEM(PUSH32_A2,  push32_a2)  \
	BOND_OPCODE_ITEM(PUSH32_A3,  push32_a3)  \
	BOND_OPCODE_ITEM(PUSH32_V0,  push32_v0)  \
	BOND_OPCODE_ITEM(PUSH32_V1,  push32_v1)  \
	BOND_OPCODE_ITEM(PUSH32_V2,  push32_v2)  \
	BOND_OPCODE_ITEM(PUSH32_V3,  push32_v3)  \
	BOND_OPCODE_ITEM(PUSH64,     push64)     \
	BOND_OPCODE_ITEM(PUSH64W,    push64w)    \
	BOND_OPCODE_ITEM(PUSH64_A0,  push64_a0)  \
	BOND_OPCODE_ITEM(PUSH64_A1,  push64_a1)  \
	BOND_OPCODE_ITEM(PUSH64_A2,  push64_a2)  \
	BOND_OPCODE_ITEM(PUSH64_A3,  push64_a3)  \
	BOND_OPCODE_ITEM(PUSH64_V0,  push64_v0)  \
	BOND_OPCODE_ITEM(PUSH64_V1,  push64_v1)  \
	BOND_OPCODE_ITEM(PUSH64_V2,  push64_v2)  \
	BOND_OPCODE_ITEM(PUSH64_V3,  push64_v3)  \
	                                         \
	BOND_OPCODE_ITEM(POPC,       popc)       \
	BOND_OPCODE_ITEM(POPCW,      popcw)      \
	BOND_OPCODE_ITEM(POPS,       pops)       \
	BOND_OPCODE_ITEM(POPSW,      popsw)      \
	BOND_OPCODE_ITEM(POP32,      pop32)      \
	BOND_OPCODE_ITEM(POP32W,     pop32w)     \
	BOND_OPCODE_ITEM(POP32_A0,   pop32_a0)   \
	BOND_OPCODE_ITEM(POP32_A1,   pop32_a1)   \
	BOND_OPCODE_ITEM(POP32_A2,   pop32_a2)   \
	BOND_OPCODE_ITEM(POP32_A3,   pop32_a3)   \
	BOND_OPCODE_ITEM(POP32_L0,   pop32_l0)   \
	BOND_OPCODE_ITEM(POP32_L1,   pop32_l1)   \
	BOND_OPCODE_ITEM(POP32_L2,   pop32_l2)   \
	BOND_OPCODE_ITEM(POP32_L3,   pop32_l3)   \
	BOND_OPCODE_ITEM(POP64,      pop64)      \
	BOND_OPCODE_ITEM(POP64W,     pop64w)     \
	BOND_OPCODE_ITEM(POP64_A0,   pop64_a0)   \
	BOND_OPCODE_ITEM(POP64_A1,   pop64_a1)   \
	BOND_OPCODE_ITEM(POP64_A2,   pop64_a2)   \
	BOND_OPCODE_ITEM(POP64_A3,   pop64_a3)   \
	BOND_OPCODE_ITEM(POP64_L0,   pop64_l0)   \
	BOND_OPCODE_ITEM(POP64_L1,   pop64_l1)   \
	BOND_OPCODE_ITEM(POP64_L2,   pop64_l2)   \
	BOND_OPCODE_ITEM(POP64_L3,   pop64_l3)   \
	                                         \
	BOND_OPCODE_ITEM(MOVESP,     movesp)     \
	BOND_OPCODE_ITEM(MOVESPW,    movespw)    \
	                                         \
	BOND_OPCODE_ITEM(DUP32,      dup32)      \
	BOND_OPCODE_ITEM(DUP64,      dup64)      \
	                                         \
	BOND_OPCODE_ITEM(CTOI,       ctoi)       \
	BOND_OPCODE_ITEM(UCTOUI,     uctoui)     \
	                                         \
	BOND_OPCODE_ITEM(STOI,       stoi)       \
	BOND_OPCODE_ITEM(USTOUI,     ustoui)     \
	                                         \
	BOND_OPCODE_ITEM(ITOC,       itoc)       \
	BOND_OPCODE_ITEM(ITOS,       itos)       \
	BOND_OPCODE_ITEM(ITOL,       itol)       \
	BOND_OPCODE_ITEM(UITOUL,     uitoul)     \
	BOND_OPCODE_ITEM(ITOF,       itof)       \
	BOND_OPCODE_ITEM(UITOF,      uitof)      \
	BOND_OPCODE_ITEM(ITOD,       itod)       \
	BOND_OPCODE_ITEM(UITOD,      uitod)      \
	                                         \
	BOND_OPCODE_ITEM(LTOI,       ltoi)       \
	BOND_OPCODE_ITEM(LTOF,       ltof)       \
	BOND_OPCODE_ITEM(ULTOF,      ultof)      \
	BOND_OPCODE_ITEM(LTOD,       ltod)       \
	BOND_OPCODE_ITEM(ULTOD,      ultod)      \
	                                         \
	BOND_OPCODE_ITEM(FTOI,       ftoi)       \
	BOND_OPCODE_ITEM(FTOUI,      ftoui)      \
	BOND_OPCODE_ITEM(FTOL,       ftol)       \
	BOND_OPCODE_ITEM(FTOUL,      ftoul)      \
	BOND_OPCODE_ITEM(FTOD,       ftod)       \
	BOND_OPCODE_ITEM(DTOI,       dtoi)       \
	BOND_OPCODE_ITEM(DTOUI,      dtoui)      \
	BOND_OPCODE_ITEM(DTOL,       dtol)       \
	BOND_OPCODE_ITEM(DTOUL,      dtoul)      \
	BOND_OPCODE_ITEM(DTOF,       dtof)       \
	                                         \
	BOND_OPCODE_ITEM(ADDI,       addi)       \
	BOND_OPCODE_ITEM(ADDL,       addl)       \
	BOND_OPCODE_ITEM(ADDF,       addf)       \
	BOND_OPCODE_ITEM(ADDD,       addd)       \
	                                         \
	BOND_OPCODE_ITEM(SUBI,       subi)       \
	BOND_OPCODE_ITEM(SUBL,       subl)       \
	BOND_OPCODE_ITEM(SUBF,       subf)       \
	BOND_OPCODE_ITEM(SUBD,       subd)       \
	                                         \
	BOND_OPCODE_ITEM(MULI,       muli)       \
	BOND_OPCODE_ITEM(MULUI,      mului)      \
	BOND_OPCODE_ITEM(MULL,       mull)       \
	BOND_OPCODE_ITEM(MULUL,      mulul)      \
	BOND_OPCODE_ITEM(MULF,       mulf)       \
	BOND_OPCODE_ITEM(MULD,       muld)       \
	                                         \
	BOND_OPCODE_ITEM(DIVI,       divi)       \
	BOND_OPCODE_ITEM(DIVUI,      divui)      \
	BOND_OPCODE_ITEM(DIVL,       divl)       \
	BOND_OPCODE_ITEM(DIVUL,      divul)      \
	BOND_OPCODE_ITEM(DIVF,       divf)       \
	BOND_OPCODE_ITEM(DIVD,       divd)       \
	                                         \
	BOND_OPCODE_ITEM(REMI,       remi)       \
	BOND_OPCODE_ITEM(REMUI,      remui)      \
	BOND_OPCODE_ITEM(REML,       reml)       \
	BOND_OPCODE_ITEM(REMUL,      remul)      \
	                                         \
	BOND_OPCODE_ITEM(LSHI,       lshi)       \
	BOND_OPCODE_ITEM(LSHL,       lshl)       \
	BOND_OPCODE_ITEM(RSHI,       rshi)       \
	BOND_OPCODE_ITEM(RSHUI,      rshui)      \
	BOND_OPCODE_ITEM(RSHL,       rshl)       \
	BOND_OPCODE_ITEM(RSHUL,      rshul)      \
	                                         \
	BOND_OPCODE_ITEM(ANDI,       andi)       \
	BOND_OPCODE_ITEM(ANDL,       andl)       \
	BOND_OPCODE_ITEM(ORI,        ori)        \
	BOND_OPCODE_ITEM(ORL,        orl)        \
	BOND_OPCODE_ITEM(XORI,       xori)       \
	BOND_OPCODE_ITEM(XORL,       xorl)       \
	                                         \
	BOND_OPCODE_ITEM(NEGI,       negi)       \
	BOND_OPCODE_ITEM(NEGL,       negl)       \
	BOND_OPCODE_ITEM(NEGF,       negf)       \
	BOND_OPCODE_ITEM(NEGD,       negd)       \
	                                         \
	BOND_OPCODE_ITEM(NOTI,       noti)       \
	BOND_OPCODE_ITEM(NOTL,       notl)       \
	                                         \
	BOND_OPCODE_ITEM(CMPI,       cmpi)       \
	BOND_OPCODE_ITEM(CMPUI,      cmpui)      \
	BOND_OPCODE_ITEM(CMPL,       cmpl)       \
	BOND_OPCODE_ITEM(CMPUL,      cmpul)      \
	BOND_OPCODE_ITEM(CMPF,       cmpf)       \
	                                         \
	BOND_OPCODE_ITEM(CMPEQ,      cmpeq)      \
	BOND_OPCODE_ITEM(CMPNE,      cmpne)      \
	BOND_OPCODE_ITEM(CMPLT,      cmplt)      \
	BOND_OPCODE_ITEM(CMPLE,      cmple)      \
	BOND_OPCODE_ITEM(CMPGT,      cmpgt)      \
	BOND_OPCODE_ITEM(CMPGE,      cmpge)      \
	                                         \
	BOND_OPCODE_ITEM(IFEQ,       ifeq)       \
	BOND_OPCODE_ITEM(IFEQW,      ifeqw)      \
	BOND_OPCODE_ITEM(IFNE,       ifne)       \
	BOND_OPCODE_ITEM(IFNEW,      ifnew)      \
	BOND_OPCODE_ITEM(IFLT,       iflt)       \
	BOND_OPCODE_ITEM(IFLTW,      ifltw)      \
	BOND_OPCODE_ITEM(IFLE,       ifle)       \
	BOND_OPCODE_ITEM(IFLEW,      iflew)      \
	BOND_OPCODE_ITEM(IFGT,       ifgt)       \
	BOND_OPCODE_ITEM(IFGTW,      ifgtw)      \
	BOND_OPCODE_ITEM(IFGE,       ifge)       \
	BOND_OPCODE_ITEM(IFGEW,      ifgew)      \
	                                         \
	BOND_OPCODE_ITEM(IFCMPIEQ,   ifcmpieq)   \
	BOND_OPCODE_ITEM(IFCMPIEQW,  ifcmpieqw)  \
	BOND_OPCODE_ITEM(IFCMPINE,   ifcmpine)   \
	BOND_OPCODE_ITEM(IFCMPINEW,  ifcmpinew)  \
	BOND_OPCODE_ITEM(IFCMPILT,   ifcmpilt)   \
	BOND_OPCODE_ITEM(IFCMPILTW,  ifcmpiltw)  \
	BOND_OPCODE_ITEM(IFCMPILE,   ifcmpile)   \
	BOND_OPCODE_ITEM(IFCMPILEW,  ifcmpilew)  \
	BOND_OPCODE_ITEM(IFCMPIGT,   ifcmpigt)   \
	BOND_OPCODE_ITEM(IFCMPIGTW,  ifcmpigtw)  \
	BOND_OPCODE_ITEM(IFCMPIGE,   ifcmpige)   \
	BOND_OPCODE_ITEM(IFCMPIGEW,  ifcmpigew)  \
	                                         \
	BOND_OPCODE_ITEM(IFCMPUILT,  ifcmpuilt)  \
	BOND_OPCODE_ITEM(IFCMPUILTW, ifcmpuiltw) \
	BOND_OPCODE_ITEM(IFCMPUILE,  ifcmpuile)  \
	BOND_OPCODE_ITEM(IFCMPUILEW, ifcmpuilew) \
	BOND_OPCODE_ITEM(IFCMPUIGT,  ifcmpuigt)  \
	BOND_OPCODE_ITEM(IFCMPUIGTW, ifcmpuigtw) \
	BOND_OPCODE_ITEM(IFCMPUIGE,  ifcmpuige)  \
	BOND_OPCODE_ITEM(IFCMPUIGEW, ifcmpuigew) \
	                                         \
	BOND_OPCODE_ITEM(GOTO,       goto)       \
	BOND_OPCODE_ITEM(GOTOW,      gotow)      \
	                                         \
	BOND_OPCODE_ITEM(INVOKE,       invoke)       \
	BOND_OPCODE_ITEM(INVOKENATIVE, invokenative) \
	                                         \
	BOND_OPCODE_ITEM(RETURN,     return)     \


enum OpCode
{
#define BOND_OPCODE_ITEM(opCode, mnemonic) OPCODE_ ## opCode,
	BOND_OPCODE_LIST
#undef BOND_OPCODE_ITEM
	OPCODE_MAX
};


const char *GetOpCodeMnemonic(OpCode opCode);

}

#endif
