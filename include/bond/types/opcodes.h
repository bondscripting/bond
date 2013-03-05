#ifndef BOND_OPCODES_H
#define BOND_OPCODES_H

#include "bond/types/types.h"

namespace Bond
{

#define BOND_OPCODE_LIST \
	/* Opcode,                     mnemonic,     parameter,    stack delta */ \
	BOND_OPCODE_ITEM(NOP,          nop,          NONE,         0)  \
	                                                               \
	BOND_OPCODE_ITEM(CONSTC,       constc,       CHAR,         1)  \
	BOND_OPCODE_ITEM(CONSTUC,      constuc,      UCHAR,        1)  \
	BOND_OPCODE_ITEM(CONSTS,       consts,       SHORT,        1)  \
	BOND_OPCODE_ITEM(CONSTUS,      constus,      USHORT,       1)  \
	BOND_OPCODE_ITEM(CONST32,      const32,      VAL32,        1)  \
	BOND_OPCODE_ITEM(CONST64,      const64,      VAL64,        1)  \
	BOND_OPCODE_ITEM(CONSTI_N2,    consti_n2,    NONE,         1)  \
	BOND_OPCODE_ITEM(CONSTI_N1,    consti_n1,    NONE,         1)  \
	BOND_OPCODE_ITEM(CONSTI_0,     consti_0,     NONE,         1)  \
	BOND_OPCODE_ITEM(CONSTI_1,     consti_1,     NONE,         1)  \
	BOND_OPCODE_ITEM(CONSTI_2,     consti_2,     NONE,         1)  \
	BOND_OPCODE_ITEM(CONSTI_3,     consti_3,     NONE,         1)  \
	BOND_OPCODE_ITEM(CONSTI_4,     consti_4,     NONE,         1)  \
	BOND_OPCODE_ITEM(CONSTI_8,     consti_8,     NONE,         1)  \
	BOND_OPCODE_ITEM(CONSTL_N2,    constl_n2,    NONE,         1)  \
	BOND_OPCODE_ITEM(CONSTL_N1,    constl_n1,    NONE,         1)  \
	BOND_OPCODE_ITEM(CONSTL_0,     constl_0,     NONE,         1)  \
	BOND_OPCODE_ITEM(CONSTL_1,     constl_1,     NONE,         1)  \
	BOND_OPCODE_ITEM(CONSTL_2,     constl_2,     NONE,         1)  \
	BOND_OPCODE_ITEM(CONSTL_3,     constl_3,     NONE,         1)  \
	BOND_OPCODE_ITEM(CONSTL_4,     constl_4,     NONE,         1)  \
	BOND_OPCODE_ITEM(CONSTL_8,     constl_8,     NONE,         1)  \
	BOND_OPCODE_ITEM(CONSTF_N2,    constf_n2,    NONE,         1)  \
	BOND_OPCODE_ITEM(CONSTF_N1,    constf_n1,    NONE,         1)  \
	BOND_OPCODE_ITEM(CONSTF_NH,    constf_nh,    NONE,         1)  \
	BOND_OPCODE_ITEM(CONSTF_0,     constf_0,     NONE,         1)  \
	BOND_OPCODE_ITEM(CONSTF_H,     constf_h,     NONE,         1)  \
	BOND_OPCODE_ITEM(CONSTF_1,     constf_1,     NONE,         1)  \
	BOND_OPCODE_ITEM(CONSTF_2,     constf_2,     NONE,         1)  \
	BOND_OPCODE_ITEM(CONSTD_N2,    constd_n2,    NONE,         1)  \
	BOND_OPCODE_ITEM(CONSTD_N1,    constd_n1,    NONE,         1)  \
	BOND_OPCODE_ITEM(CONSTD_NH,    constd_nh,    NONE,         1)  \
	BOND_OPCODE_ITEM(CONSTD_0,     constd_0,     NONE,         1)  \
	BOND_OPCODE_ITEM(CONSTD_H,     constd_h,     NONE,         1)  \
	BOND_OPCODE_ITEM(CONSTD_1,     constd_1,     NONE,         1)  \
	BOND_OPCODE_ITEM(CONSTD_2,     constd_2,     NONE,         1)  \
	                                                               \
	BOND_OPCODE_ITEM(LOADFP,       loadfp,       SHORT,        1)  \
	BOND_OPCODE_ITEM(LOADFPW,      loadfpw,      INT,          1)  \
	BOND_OPCODE_ITEM(LOADEA,       loadea,       HASH,         1)  \
	BOND_OPCODE_ITEM(LOADC,        loadc,        NONE,         0)  \
	BOND_OPCODE_ITEM(LOADUC,       loaduc,       NONE,         0)  \
	BOND_OPCODE_ITEM(LOADS,        loads,        NONE,         0)  \
	BOND_OPCODE_ITEM(LOADUS,       loadus,       NONE,         0)  \
	BOND_OPCODE_ITEM(LOAD32,       load32,       NONE,         0)  \
	BOND_OPCODE_ITEM(LOAD64,       load64,       NONE,         0)  \
	BOND_OPCODE_ITEM(LOADMEMW,     loadmemw,     INT,          1)  \
	                                                               \
	BOND_OPCODE_ITEM(STOREC,       storec,       NONE,         -2) \
	BOND_OPCODE_ITEM(STORES,       stores,       NONE,         -2) \
	BOND_OPCODE_ITEM(STORE32,      store32,      NONE,         -2) \
	BOND_OPCODE_ITEM(STORE64,      store64,      NONE,         -2) \
	                                                               \
	BOND_OPCODE_ITEM(PUSHC,        pushc,        SHORT,        1)  \
	BOND_OPCODE_ITEM(PUSHCW,       pushcw,       INT,          1)  \
	BOND_OPCODE_ITEM(PUSHUC,       pushuc,       SHORT,        1)  \
	BOND_OPCODE_ITEM(PUSHUCW,      pushucw,      INT,          1)  \
	BOND_OPCODE_ITEM(PUSHS,        pushs,        SHORT,        1)  \
	BOND_OPCODE_ITEM(PUSHSW,       pushsw,       INT,          1)  \
	BOND_OPCODE_ITEM(PUSHUS,       pushus,       SHORT,        1)  \
	BOND_OPCODE_ITEM(PUSHUSW,      pushusw,      INT,          1)  \
	BOND_OPCODE_ITEM(PUSH32,       push32,       SHORT,        1)  \
	BOND_OPCODE_ITEM(PUSH32W,      push32w,      INT,          1)  \
	BOND_OPCODE_ITEM(PUSH32_P0,    push32_p0,    NONE,         1)  \
	BOND_OPCODE_ITEM(PUSH32_P1,    push32_p1,    NONE,         1)  \
	BOND_OPCODE_ITEM(PUSH32_P2,    push32_p2,    NONE,         1)  \
	BOND_OPCODE_ITEM(PUSH32_P3,    push32_p3,    NONE,         1)  \
	BOND_OPCODE_ITEM(PUSH32_L0,    push32_l0,    NONE,         1)  \
	BOND_OPCODE_ITEM(PUSH32_L1,    push32_l1,    NONE,         1)  \
	BOND_OPCODE_ITEM(PUSH32_L2,    push32_l2,    NONE,         1)  \
	BOND_OPCODE_ITEM(PUSH32_L3,    push32_l3,    NONE,         1)  \
	BOND_OPCODE_ITEM(PUSH64,       push64,       SHORT,        1)  \
	BOND_OPCODE_ITEM(PUSH64W,      push64w,      INT,          1)  \
	BOND_OPCODE_ITEM(PUSH64_P0,    push64_p0,    NONE,         1)  \
	BOND_OPCODE_ITEM(PUSH64_P1,    push64_p1,    NONE,         1)  \
	BOND_OPCODE_ITEM(PUSH64_P2,    push64_p2,    NONE,         1)  \
	BOND_OPCODE_ITEM(PUSH64_P3,    push64_p3,    NONE,         1)  \
	BOND_OPCODE_ITEM(PUSH64_L0,    push64_l0,    NONE,         1)  \
	BOND_OPCODE_ITEM(PUSH64_L1,    push64_l1,    NONE,         1)  \
	BOND_OPCODE_ITEM(PUSH64_L2,    push64_l2,    NONE,         1)  \
	BOND_OPCODE_ITEM(PUSH64_L3,    push64_l3,    NONE,         1)  \
	                                                               \
	BOND_OPCODE_ITEM(POP,          pop,          NONE,         -1) \
	BOND_OPCODE_ITEM(POPC,         popc,         SHORT,        -1) \
	BOND_OPCODE_ITEM(POPCW,        popcw,        INT,          -1) \
	BOND_OPCODE_ITEM(POPS,         pops,         SHORT,        -1) \
	BOND_OPCODE_ITEM(POPSW,        popsw,        INT,          -1) \
	BOND_OPCODE_ITEM(POP32,        pop32,        SHORT,        -1) \
	BOND_OPCODE_ITEM(POP32W,       pop32w,       INT,          -1) \
	BOND_OPCODE_ITEM(POP32_P0,     pop32_p0,     NONE,         -1) \
	BOND_OPCODE_ITEM(POP32_P1,     pop32_p1,     NONE,         -1) \
	BOND_OPCODE_ITEM(POP32_P2,     pop32_p2,     NONE,         -1) \
	BOND_OPCODE_ITEM(POP32_P3,     pop32_p3,     NONE,         -1) \
	BOND_OPCODE_ITEM(POP32_L0,     pop32_l0,     NONE,         -1) \
	BOND_OPCODE_ITEM(POP32_L1,     pop32_l1,     NONE,         -1) \
	BOND_OPCODE_ITEM(POP32_L2,     pop32_l2,     NONE,         -1) \
	BOND_OPCODE_ITEM(POP32_L3,     pop32_l3,     NONE,         -1) \
	BOND_OPCODE_ITEM(POP64,        pop64,        SHORT,        -1) \
	BOND_OPCODE_ITEM(POP64W,       pop64w,       INT,          -1) \
	BOND_OPCODE_ITEM(POP64_P0,     pop64_p0,     NONE,         -1) \
	BOND_OPCODE_ITEM(POP64_P1,     pop64_p1,     NONE,         -1) \
	BOND_OPCODE_ITEM(POP64_P2,     pop64_p2,     NONE,         -1) \
	BOND_OPCODE_ITEM(POP64_P3,     pop64_p3,     NONE,         -1) \
	BOND_OPCODE_ITEM(POP64_L0,     pop64_l0,     NONE,         -1) \
	BOND_OPCODE_ITEM(POP64_L1,     pop64_l1,     NONE,         -1) \
	BOND_OPCODE_ITEM(POP64_L2,     pop64_l2,     NONE,         -1) \
	BOND_OPCODE_ITEM(POP64_L3,     pop64_l3,     NONE,         -1) \
	                                                               \
	BOND_OPCODE_ITEM(MEMCOPYW,     memcopyw,     INT,          -2) \
	                                                               \
	BOND_OPCODE_ITEM(DUP,          dup,          NONE,         1)  \
	BOND_OPCODE_ITEM(DUPINS,       dupins,       NONE,         1)  \
	                                                               \
	BOND_OPCODE_ITEM(ITOC,         itoc,         NONE,         0)  \
	BOND_OPCODE_ITEM(UITOUC,       uitouc,       NONE,         0)  \
	BOND_OPCODE_ITEM(ITOS,         itos,         NONE,         0)  \
	BOND_OPCODE_ITEM(UITOUS,       uitous,       NONE,         0)  \
	BOND_OPCODE_ITEM(ITOL,         itol,         NONE,         0)  \
	BOND_OPCODE_ITEM(UITOUL,       uitoul,       NONE,         0)  \
	BOND_OPCODE_ITEM(ITOF,         itof,         NONE,         0)  \
	BOND_OPCODE_ITEM(UITOF,        uitof,        NONE,         0)  \
	BOND_OPCODE_ITEM(ITOD,         itod,         NONE,         0)  \
	BOND_OPCODE_ITEM(UITOD,        uitod,        NONE,         0)  \
	                                                               \
	BOND_OPCODE_ITEM(LTOI,         ltoi,         NONE,         0)  \
	BOND_OPCODE_ITEM(LTOF,         ltof,         NONE,         0)  \
	BOND_OPCODE_ITEM(ULTOF,        ultof,        NONE,         0)  \
	BOND_OPCODE_ITEM(LTOD,         ltod,         NONE,         0)  \
	BOND_OPCODE_ITEM(ULTOD,        ultod,        NONE,         0)  \
	                                                               \
	BOND_OPCODE_ITEM(FTOI,         ftoi,         NONE,         0)  \
	BOND_OPCODE_ITEM(FTOUI,        ftoui,        NONE,         0)  \
	BOND_OPCODE_ITEM(FTOL,         ftol,         NONE,         0)  \
	BOND_OPCODE_ITEM(FTOUL,        ftoul,        NONE,         0)  \
	BOND_OPCODE_ITEM(FTOD,         ftod,         NONE,         0)  \
	BOND_OPCODE_ITEM(DTOI,         dtoi,         NONE,         0)  \
	BOND_OPCODE_ITEM(DTOUI,        dtoui,        NONE,         0)  \
	BOND_OPCODE_ITEM(DTOL,         dtol,         NONE,         0)  \
	BOND_OPCODE_ITEM(DTOUL,        dtoul,        NONE,         0)  \
	BOND_OPCODE_ITEM(DTOF,         dtof,         NONE,         0)  \
	                                                               \
	BOND_OPCODE_ITEM(PTROFF,       ptroff,       SHORT,        -1) \
	BOND_OPCODE_ITEM(PTRDIFF,      ptrdiff,      SHORT,        -1) \
	                                                               \
	BOND_OPCODE_ITEM(INCI,         inci,         UCHAR_CHAR,   0)  \
	BOND_OPCODE_ITEM(INCL,         incl,         UCHAR_CHAR,   0)  \
	                                                               \
	BOND_OPCODE_ITEM(ADDI,         addi,         NONE,         -1) \
	BOND_OPCODE_ITEM(ADDL,         addl,         NONE,         -1) \
	BOND_OPCODE_ITEM(ADDF,         addf,         NONE,         -1) \
	BOND_OPCODE_ITEM(ADDD,         addd,         NONE,         -1) \
	                                                               \
	BOND_OPCODE_ITEM(SUBI,         subi,         NONE,         -1) \
	BOND_OPCODE_ITEM(SUBL,         subl,         NONE,         -1) \
	BOND_OPCODE_ITEM(SUBF,         subf,         NONE,         -1) \
	BOND_OPCODE_ITEM(SUBD,         subd,         NONE,         -1) \
	                                                               \
	BOND_OPCODE_ITEM(MULI,         muli,         NONE,         -1) \
	BOND_OPCODE_ITEM(MULUI,        mului,        NONE,         -1) \
	BOND_OPCODE_ITEM(MULL,         mull,         NONE,         -1) \
	BOND_OPCODE_ITEM(MULUL,        mulul,        NONE,         -1) \
	BOND_OPCODE_ITEM(MULF,         mulf,         NONE,         -1) \
	BOND_OPCODE_ITEM(MULD,         muld,         NONE,         -1) \
	                                                               \
	BOND_OPCODE_ITEM(DIVI,         divi,         NONE,         -1) \
	BOND_OPCODE_ITEM(DIVUI,        divui,        NONE,         -1) \
	BOND_OPCODE_ITEM(DIVL,         divl,         NONE,         -1) \
	BOND_OPCODE_ITEM(DIVUL,        divul,        NONE,         -1) \
	BOND_OPCODE_ITEM(DIVF,         divf,         NONE,         -1) \
	BOND_OPCODE_ITEM(DIVD,         divd,         NONE,         -1) \
	                                                               \
	BOND_OPCODE_ITEM(REMI,         remi,         NONE,         -1) \
	BOND_OPCODE_ITEM(REMUI,        remui,        NONE,         -1) \
	BOND_OPCODE_ITEM(REML,         reml,         NONE,         -1) \
	BOND_OPCODE_ITEM(REMUL,        remul,        NONE,         -1) \
	                                                               \
	BOND_OPCODE_ITEM(LSHI,         lshi,         NONE,         -1) \
	BOND_OPCODE_ITEM(LSHL,         lshl,         NONE,         -1) \
	BOND_OPCODE_ITEM(RSHI,         rshi,         NONE,         -1) \
	BOND_OPCODE_ITEM(RSHUI,        rshui,        NONE,         -1) \
	BOND_OPCODE_ITEM(RSHL,         rshl,         NONE,         -1) \
	BOND_OPCODE_ITEM(RSHUL,        rshul,        NONE,         -1) \
	                                                               \
	BOND_OPCODE_ITEM(ANDI,         andi,         NONE,         -1) \
	BOND_OPCODE_ITEM(ANDL,         andl,         NONE,         -1) \
	BOND_OPCODE_ITEM(ORI,          ori,          NONE,         -1) \
	BOND_OPCODE_ITEM(ORL,          orl,          NONE,         -1) \
	BOND_OPCODE_ITEM(XORI,         xori,         NONE,         -1) \
	BOND_OPCODE_ITEM(XORL,         xorl,         NONE,         -1) \
	                                                               \
	BOND_OPCODE_ITEM(NEGI,         negi,         NONE,         0)  \
	BOND_OPCODE_ITEM(NEGL,         negl,         NONE,         0)  \
	BOND_OPCODE_ITEM(NEGF,         negf,         NONE,         0)  \
	BOND_OPCODE_ITEM(NEGD,         negd,         NONE,         0)  \
	                                                               \
	BOND_OPCODE_ITEM(NOT,          not,          NONE,         0)  \
	                                                               \
	BOND_OPCODE_ITEM(CMPEQI,       cmpeqi,       NONE,         -1) \
	BOND_OPCODE_ITEM(CMPEQL,       cmpeql,       NONE,         -1) \
	BOND_OPCODE_ITEM(CMPEQF,       cmpeqf,       NONE,         -1) \
	BOND_OPCODE_ITEM(CMPEQD,       cmpeqd,       NONE,         -1) \
	BOND_OPCODE_ITEM(CMPNEQI,      cmpneqi,      NONE,         -1) \
	BOND_OPCODE_ITEM(CMPNEQL,      cmpneql,      NONE,         -1) \
	BOND_OPCODE_ITEM(CMPNEQF,      cmpneqf,      NONE,         -1) \
	BOND_OPCODE_ITEM(CMPNEQD,      cmpneqd,      NONE,         -1) \
	BOND_OPCODE_ITEM(CMPLTI,       cmplti,       NONE,         -1) \
	BOND_OPCODE_ITEM(CMPLTUI,      cmpltui,      NONE,         -1) \
	BOND_OPCODE_ITEM(CMPLTL,       cmpltl,       NONE,         -1) \
	BOND_OPCODE_ITEM(CMPLTUL,      cmpltul,      NONE,         -1) \
	BOND_OPCODE_ITEM(CMPLTF,       cmpltf,       NONE,         -1) \
	BOND_OPCODE_ITEM(CMPLTD,       cmpltd,       NONE,         -1) \
	BOND_OPCODE_ITEM(CMPLEI,       cmplei,       NONE,         -1) \
	BOND_OPCODE_ITEM(CMPLEUI,      cmpleui,      NONE,         -1) \
	BOND_OPCODE_ITEM(CMPLEL,       cmplel,       NONE,         -1) \
	BOND_OPCODE_ITEM(CMPLEUL,      cmpleul,      NONE,         -1) \
	BOND_OPCODE_ITEM(CMPLEF,       cmplef,       NONE,         -1) \
	BOND_OPCODE_ITEM(CMPLED,       cmpled,       NONE,         -1) \
	BOND_OPCODE_ITEM(CMPGTI,       cmpgti,       NONE,         -1) \
	BOND_OPCODE_ITEM(CMPGTUI,      cmpgtui,      NONE,         -1) \
	BOND_OPCODE_ITEM(CMPGTL,       cmpgtl,       NONE,         -1) \
	BOND_OPCODE_ITEM(CMPGTUL,      cmpgtul,      NONE,         -1) \
	BOND_OPCODE_ITEM(CMPGTF,       cmpgtf,       NONE,         -1) \
	BOND_OPCODE_ITEM(CMPGTD,       cmpgtd,       NONE,         -1) \
	BOND_OPCODE_ITEM(CMPGEI,       cmpgei,       NONE,         -1) \
	BOND_OPCODE_ITEM(CMPGEUI,      cmpgeui,      NONE,         -1) \
	BOND_OPCODE_ITEM(CMPGEL,       cmpgel,       NONE,         -1) \
	BOND_OPCODE_ITEM(CMPGEUL,      cmpgeul,      NONE,         -1) \
	BOND_OPCODE_ITEM(CMPGEF,       cmpgef,       NONE,         -1) \
	BOND_OPCODE_ITEM(CMPGED,       cmpged,       NONE,         -1) \
	                                                               \
	BOND_OPCODE_ITEM(BRZ,          brz,          OFF16,        -1) \
	BOND_OPCODE_ITEM(BRZW,         brzw,         OFF32,        -1) \
	BOND_OPCODE_ITEM(BRNZ,         brnz,         OFF16,        -1) \
	BOND_OPCODE_ITEM(BRNZW,        brnzw,        OFF32,        -1) \
	BOND_OPCODE_ITEM(IFZ,          ifz,          OFF16,        -1) \
	BOND_OPCODE_ITEM(IFZW,         ifzw,         OFF32,        -1) \
	BOND_OPCODE_ITEM(IFNZ,         ifnz,         OFF16,        -1) \
	BOND_OPCODE_ITEM(IFNZW,        ifnzw,        OFF32,        -1) \
	                                                               \
	BOND_OPCODE_ITEM(GOTO,         goto,         OFF16,        0)  \
	BOND_OPCODE_ITEM(GOTOW,        gotow,        OFF32,        0)  \
	                                                               \
	BOND_OPCODE_ITEM(LOOKUPSWITCH, lookupswitch, LOOKUPSWITCH, -1) \
	BOND_OPCODE_ITEM(TABLESWITCH,  tableswitch,  TABLESWITCH,  -1) \
	                                                               \
	BOND_OPCODE_ITEM(INVOKE,       invoke,       HASH,         0)  \
	BOND_OPCODE_ITEM(INVOKENATIVE, invokenative, HASH,         0)  \
	                                                               \
	BOND_OPCODE_ITEM(RETURN,       return,       NONE,         0)  \
	BOND_OPCODE_ITEM(RETURN32,     return32,     NONE,         -1) \
	BOND_OPCODE_ITEM(RETURN64,     return64,     NONE,         -1) \
	BOND_OPCODE_ITEM(RETURNMEMW,   returnmemw,   INT,          -1) \


enum OpCode
{
#define BOND_OPCODE_ITEM(opCode, mnemonic, param, stackDelta) OPCODE_ ## opCode,
	BOND_OPCODE_LIST
#undef BOND_OPCODE_ITEM
	OPCODE_MAX
};


enum OpCodeParam
{
	OC_PARAM_NONE,
	OC_PARAM_CHAR,
	OC_PARAM_UCHAR,
	OC_PARAM_UCHAR_CHAR,
	OC_PARAM_SHORT,
	OC_PARAM_USHORT,
	OC_PARAM_INT,
	OC_PARAM_VAL32,
	OC_PARAM_VAL64,
	OC_PARAM_OFF16,
	OC_PARAM_OFF32,
	OC_PARAM_HASH,
	OC_PARAM_LOOKUPSWITCH,
	OC_PARAM_TABLESWITCH,
};


const char *GetSignatureTypeMnemonic(SignatureType signatureType);
const char *GetOpCodeMnemonic(OpCode opCode);
OpCodeParam GetOpCodeParamType(OpCode opCode);
bi32_t GetStackDelta(OpCode opCode);

}

#endif
