#ifndef BOND_OPCODES_H
#define BOND_OPCODES_H

namespace Bond
{

#define BOND_OPCODE_LIST \
	/* Opcode, mnemonic, argument */                     \
	BOND_OPCODE_ITEM(CONSTC,       constc,       CHAR)   \
	BOND_OPCODE_ITEM(CONSTUC,      constuc,      UCHAR)  \
	BOND_OPCODE_ITEM(CONSTS,       consts,       SHORT)  \
	BOND_OPCODE_ITEM(CONSTUS,      constus,      USHORT) \
	BOND_OPCODE_ITEM(CONST32,      const32,      VAL32)  \
	BOND_OPCODE_ITEM(CONST64,      const64,      VAL64)  \
	BOND_OPCODE_ITEM(CONSTI_N2,    consti_n2,    NONE)   \
	BOND_OPCODE_ITEM(CONSTI_N1,    consti_n1,    NONE)   \
	BOND_OPCODE_ITEM(CONSTI_0,     consti_0,     NONE)   \
	BOND_OPCODE_ITEM(CONSTI_1,     consti_1,     NONE)   \
	BOND_OPCODE_ITEM(CONSTI_2,     consti_2,     NONE)   \
	BOND_OPCODE_ITEM(CONSTI_3,     consti_3,     NONE)   \
	BOND_OPCODE_ITEM(CONSTI_4,     consti_4,     NONE)   \
	BOND_OPCODE_ITEM(CONSTI_5,     consti_5,     NONE)   \
	BOND_OPCODE_ITEM(CONSTI_6,     consti_6,     NONE)   \
	BOND_OPCODE_ITEM(CONSTI_7,     consti_7,     NONE)   \
	BOND_OPCODE_ITEM(CONSTI_8,     consti_8,     NONE)   \
	BOND_OPCODE_ITEM(CONSTL_N1,    constl_n1,    NONE)   \
	BOND_OPCODE_ITEM(CONSTL_0,     constl_0,     NONE)   \
	BOND_OPCODE_ITEM(CONSTL_1,     constl_1,     NONE)   \
	BOND_OPCODE_ITEM(CONSTF_N2,    constf_n2,    NONE)   \
	BOND_OPCODE_ITEM(CONSTF_N1,    constf_n1,    NONE)   \
	BOND_OPCODE_ITEM(CONSTF_NH,    constf_nh,    NONE)   \
	BOND_OPCODE_ITEM(CONSTF_0,     constf_0,     NONE)   \
	BOND_OPCODE_ITEM(CONSTF_H,     constf_h,     NONE)   \
	BOND_OPCODE_ITEM(CONSTF_1,     constf_1,     NONE)   \
	BOND_OPCODE_ITEM(CONSTF_2,     constf_2,     NONE)   \
	BOND_OPCODE_ITEM(CONSTD_N2,    constd_n2,    NONE)   \
	BOND_OPCODE_ITEM(CONSTD_N1,    constd_n1,    NONE)   \
	BOND_OPCODE_ITEM(CONSTD_NH,    constd_nh,    NONE)   \
	BOND_OPCODE_ITEM(CONSTD_0,     constd_0,     NONE)   \
	BOND_OPCODE_ITEM(CONSTD_H,     constd_h,     NONE)   \
	BOND_OPCODE_ITEM(CONSTD_1,     constd_1,     NONE)   \
	BOND_OPCODE_ITEM(CONSTD_2,     constd_2,     NONE)   \
	                                                     \
	BOND_OPCODE_ITEM(LOADFP,       loadfp,       SHORT)  \
	BOND_OPCODE_ITEM(LOADFPW,      loadfpw,      INT)    \
	BOND_OPCODE_ITEM(LOADEA,       loadea,       HASH)   \
	BOND_OPCODE_ITEM(LOADC,        loadc,        NONE)   \
	BOND_OPCODE_ITEM(LOADUC,       loaduc,       NONE)   \
	BOND_OPCODE_ITEM(LOADS,        loads,        NONE)   \
	BOND_OPCODE_ITEM(LOADUS,       loadus,       NONE)   \
	BOND_OPCODE_ITEM(LOAD32,       load32,       NONE)   \
	BOND_OPCODE_ITEM(LOAD64,       load64,       NONE)   \
	                                                     \
	BOND_OPCODE_ITEM(STOREC,       storec,       NONE)   \
	BOND_OPCODE_ITEM(STORES,       stores,       NONE)   \
	BOND_OPCODE_ITEM(STORE32,      store32,      NONE)   \
	BOND_OPCODE_ITEM(STORE64,      store64,      NONE)   \
	                                                     \
	BOND_OPCODE_ITEM(PUSHC,        pushc,        SHORT)  \
	BOND_OPCODE_ITEM(PUSHCW,       pushcw,       INT)    \
	BOND_OPCODE_ITEM(PUSHUC,       pushuc,       SHORT)  \
	BOND_OPCODE_ITEM(PUSHUCW,      pushucw,      INT)    \
	BOND_OPCODE_ITEM(PUSHS,        pushs,        SHORT)  \
	BOND_OPCODE_ITEM(PUSHSW,       pushsw,       INT)    \
	BOND_OPCODE_ITEM(PUSHUS,       pushus,       SHORT)  \
	BOND_OPCODE_ITEM(PUSHUSW,      pushusw,      INT)    \
	BOND_OPCODE_ITEM(PUSH32,       push32,       SHORT)  \
	BOND_OPCODE_ITEM(PUSH32W,      push32w,      INT)    \
	BOND_OPCODE_ITEM(PUSH32_P0,    push32_p0,    NONE)   \
	BOND_OPCODE_ITEM(PUSH32_P1,    push32_p1,    NONE)   \
	BOND_OPCODE_ITEM(PUSH32_P2,    push32_p2,    NONE)   \
	BOND_OPCODE_ITEM(PUSH32_P3,    push32_p3,    NONE)   \
	BOND_OPCODE_ITEM(PUSH32_L0,    push32_l0,    NONE)   \
	BOND_OPCODE_ITEM(PUSH32_L1,    push32_l1,    NONE)   \
	BOND_OPCODE_ITEM(PUSH32_L2,    push32_l2,    NONE)   \
	BOND_OPCODE_ITEM(PUSH32_L3,    push32_l3,    NONE)   \
	BOND_OPCODE_ITEM(PUSH64,       push64,       SHORT)  \
	BOND_OPCODE_ITEM(PUSH64W,      push64w,      INT)    \
	BOND_OPCODE_ITEM(PUSH64_P0,    push64_p0,    NONE)   \
	BOND_OPCODE_ITEM(PUSH64_P1,    push64_p1,    NONE)   \
	BOND_OPCODE_ITEM(PUSH64_P2,    push64_p2,    NONE)   \
	BOND_OPCODE_ITEM(PUSH64_P3,    push64_p3,    NONE)   \
	BOND_OPCODE_ITEM(PUSH64_L0,    push64_l0,    NONE)   \
	BOND_OPCODE_ITEM(PUSH64_L1,    push64_l1,    NONE)   \
	BOND_OPCODE_ITEM(PUSH64_L2,    push64_l2,    NONE)   \
	BOND_OPCODE_ITEM(PUSH64_L3,    push64_l3,    NONE)   \
	                                                     \
	BOND_OPCODE_ITEM(POPC,         popc,         SHORT)  \
	BOND_OPCODE_ITEM(POPCW,        popcw,        INT)    \
	BOND_OPCODE_ITEM(POPS,         pops,         SHORT)  \
	BOND_OPCODE_ITEM(POPSW,        popsw,        INT)    \
	BOND_OPCODE_ITEM(POP32,        pop32,        SHORT)  \
	BOND_OPCODE_ITEM(POP32W,       pop32w,       INT)    \
	BOND_OPCODE_ITEM(POP32_P0,     pop32_p0,     NONE)   \
	BOND_OPCODE_ITEM(POP32_P1,     pop32_p1,     NONE)   \
	BOND_OPCODE_ITEM(POP32_P2,     pop32_p2,     NONE)   \
	BOND_OPCODE_ITEM(POP32_P3,     pop32_p3,     NONE)   \
	BOND_OPCODE_ITEM(POP32_L0,     pop32_l0,     NONE)   \
	BOND_OPCODE_ITEM(POP32_L1,     pop32_l1,     NONE)   \
	BOND_OPCODE_ITEM(POP32_L2,     pop32_l2,     NONE)   \
	BOND_OPCODE_ITEM(POP32_L3,     pop32_l3,     NONE)   \
	BOND_OPCODE_ITEM(POP64,        pop64,        SHORT)  \
	BOND_OPCODE_ITEM(POP64W,       pop64w,       INT)    \
	BOND_OPCODE_ITEM(POP64_P0,     pop64_p0,     NONE)   \
	BOND_OPCODE_ITEM(POP64_P1,     pop64_p1,     NONE)   \
	BOND_OPCODE_ITEM(POP64_P2,     pop64_p2,     NONE)   \
	BOND_OPCODE_ITEM(POP64_P3,     pop64_p3,     NONE)   \
	BOND_OPCODE_ITEM(POP64_L0,     pop64_l0,     NONE)   \
	BOND_OPCODE_ITEM(POP64_L1,     pop64_l1,     NONE)   \
	BOND_OPCODE_ITEM(POP64_L2,     pop64_l2,     NONE)   \
	BOND_OPCODE_ITEM(POP64_L3,     pop64_l3,     NONE)   \
	                                                     \
	BOND_OPCODE_ITEM(MOVESP,       movesp,       SHORT)  \
	BOND_OPCODE_ITEM(MOVESPW,      movespw,      INT)    \
	                                                     \
	BOND_OPCODE_ITEM(DUP32,        dup32,        NONE)   \
	BOND_OPCODE_ITEM(DUP64,        dup64,        NONE)   \
	                                                     \
	BOND_OPCODE_ITEM(CTOI,         ctoi,         NONE)   \
	BOND_OPCODE_ITEM(UCTOUI,       uctoui,       NONE)   \
	                                                     \
	BOND_OPCODE_ITEM(STOI,         stoi,         NONE)   \
	BOND_OPCODE_ITEM(USTOUI,       ustoui,       NONE)   \
	                                                     \
	BOND_OPCODE_ITEM(ITOC,         itoc,         NONE)   \
	BOND_OPCODE_ITEM(ITOS,         itos,         NONE)   \
	BOND_OPCODE_ITEM(ITOL,         itol,         NONE)   \
	BOND_OPCODE_ITEM(UITOUL,       uitoul,       NONE)   \
	BOND_OPCODE_ITEM(ITOF,         itof,         NONE)   \
	BOND_OPCODE_ITEM(UITOF,        uitof,        NONE)   \
	BOND_OPCODE_ITEM(ITOD,         itod,         NONE)   \
	BOND_OPCODE_ITEM(UITOD,        uitod,        NONE)   \
	                                                     \
	BOND_OPCODE_ITEM(LTOI,         ltoi,         NONE)   \
	BOND_OPCODE_ITEM(LTOF,         ltof,         NONE)   \
	BOND_OPCODE_ITEM(ULTOF,        ultof,        NONE)   \
	BOND_OPCODE_ITEM(LTOD,         ltod,         NONE)   \
	BOND_OPCODE_ITEM(ULTOD,        ultod,        NONE)   \
	                                                     \
	BOND_OPCODE_ITEM(FTOI,         ftoi,         NONE)   \
	BOND_OPCODE_ITEM(FTOUI,        ftoui,        NONE)   \
	BOND_OPCODE_ITEM(FTOL,         ftol,         NONE)   \
	BOND_OPCODE_ITEM(FTOUL,        ftoul,        NONE)   \
	BOND_OPCODE_ITEM(FTOD,         ftod,         NONE)   \
	BOND_OPCODE_ITEM(DTOI,         dtoi,         NONE)   \
	BOND_OPCODE_ITEM(DTOUI,        dtoui,        NONE)   \
	BOND_OPCODE_ITEM(DTOL,         dtol,         NONE)   \
	BOND_OPCODE_ITEM(DTOUL,        dtoul,        NONE)   \
	BOND_OPCODE_ITEM(DTOF,         dtof,         NONE)   \
	                                                     \
	BOND_OPCODE_ITEM(ADDI,         addi,         NONE)   \
	BOND_OPCODE_ITEM(ADDL,         addl,         NONE)   \
	BOND_OPCODE_ITEM(ADDF,         addf,         NONE)   \
	BOND_OPCODE_ITEM(ADDD,         addd,         NONE)   \
	                                                     \
	BOND_OPCODE_ITEM(SUBI,         subi,         NONE)   \
	BOND_OPCODE_ITEM(SUBL,         subl,         NONE)   \
	BOND_OPCODE_ITEM(SUBF,         subf,         NONE)   \
	BOND_OPCODE_ITEM(SUBD,         subd,         NONE)   \
	                                                     \
	BOND_OPCODE_ITEM(MULI,         muli,         NONE)   \
	BOND_OPCODE_ITEM(MULUI,        mului,        NONE)   \
	BOND_OPCODE_ITEM(MULL,         mull,         NONE)   \
	BOND_OPCODE_ITEM(MULUL,        mulul,        NONE)   \
	BOND_OPCODE_ITEM(MULF,         mulf,         NONE)   \
	BOND_OPCODE_ITEM(MULD,         muld,         NONE)   \
	                                                     \
	BOND_OPCODE_ITEM(DIVI,         divi,         NONE)   \
	BOND_OPCODE_ITEM(DIVUI,        divui,        NONE)   \
	BOND_OPCODE_ITEM(DIVL,         divl,         NONE)   \
	BOND_OPCODE_ITEM(DIVUL,        divul,        NONE)   \
	BOND_OPCODE_ITEM(DIVF,         divf,         NONE)   \
	BOND_OPCODE_ITEM(DIVD,         divd,         NONE)   \
	                                                     \
	BOND_OPCODE_ITEM(REMI,         remi,         NONE)   \
	BOND_OPCODE_ITEM(REMUI,        remui,        NONE)   \
	BOND_OPCODE_ITEM(REML,         reml,         NONE)   \
	BOND_OPCODE_ITEM(REMUL,        remul,        NONE)   \
	                                                     \
	BOND_OPCODE_ITEM(LSHI,         lshi,         NONE)   \
	BOND_OPCODE_ITEM(LSHL,         lshl,         NONE)   \
	BOND_OPCODE_ITEM(RSHI,         rshi,         NONE)   \
	BOND_OPCODE_ITEM(RSHUI,        rshui,        NONE)   \
	BOND_OPCODE_ITEM(RSHL,         rshl,         NONE)   \
	BOND_OPCODE_ITEM(RSHUL,        rshul,        NONE)   \
	                                                     \
	BOND_OPCODE_ITEM(ANDI,         andi,         NONE)   \
	BOND_OPCODE_ITEM(ANDL,         andl,         NONE)   \
	BOND_OPCODE_ITEM(ORI,          ori,          NONE)   \
	BOND_OPCODE_ITEM(ORL,          orl,          NONE)   \
	BOND_OPCODE_ITEM(XORI,         xori,         NONE)   \
	BOND_OPCODE_ITEM(XORL,         xorl,         NONE)   \
	                                                     \
	BOND_OPCODE_ITEM(NEGI,         negi,         NONE)   \
	BOND_OPCODE_ITEM(NEGL,         negl,         NONE)   \
	BOND_OPCODE_ITEM(NEGF,         negf,         NONE)   \
	BOND_OPCODE_ITEM(NEGD,         negd,         NONE)   \
	                                                     \
	BOND_OPCODE_ITEM(NOTI,         noti,         NONE)   \
	BOND_OPCODE_ITEM(NOTL,         notl,         NONE)   \
	                                                     \
	BOND_OPCODE_ITEM(CMPI,         cmpi,         NONE)   \
	BOND_OPCODE_ITEM(CMPUI,        cmpui,        NONE)   \
	BOND_OPCODE_ITEM(CMPL,         cmpl,         NONE)   \
	BOND_OPCODE_ITEM(CMPUL,        cmpul,        NONE)   \
	BOND_OPCODE_ITEM(CMPF,         cmpf,         NONE)   \
	                                                     \
	BOND_OPCODE_ITEM(CMPEQ,        cmpeq,        NONE)   \
	BOND_OPCODE_ITEM(CMPNE,        cmpne,        NONE)   \
	BOND_OPCODE_ITEM(CMPLT,        cmplt,        NONE)   \
	BOND_OPCODE_ITEM(CMPLE,        cmple,        NONE)   \
	BOND_OPCODE_ITEM(CMPGT,        cmpgt,        NONE)   \
	BOND_OPCODE_ITEM(CMPGE,        cmpge,        NONE)   \
	                                                     \
	BOND_OPCODE_ITEM(IFEQ,         ifeq,         OFF16)  \
	BOND_OPCODE_ITEM(IFEQW,        ifeqw,        OFF32)  \
	BOND_OPCODE_ITEM(IFNE,         ifne,         OFF16)  \
	BOND_OPCODE_ITEM(IFNEW,        ifnew,        OFF32)  \
	BOND_OPCODE_ITEM(IFLT,         iflt,         OFF16)  \
	BOND_OPCODE_ITEM(IFLTW,        ifltw,        OFF32)  \
	BOND_OPCODE_ITEM(IFLE,         ifle,         OFF16)  \
	BOND_OPCODE_ITEM(IFLEW,        iflew,        OFF32)  \
	BOND_OPCODE_ITEM(IFGT,         ifgt,         OFF16)  \
	BOND_OPCODE_ITEM(IFGTW,        ifgtw,        OFF32)  \
	BOND_OPCODE_ITEM(IFGE,         ifge,         OFF16)  \
	BOND_OPCODE_ITEM(IFGEW,        ifgew,        OFF32)  \
	                                                     \
	BOND_OPCODE_ITEM(IFCMPIEQ,     ifcmpieq,     OFF16)  \
	BOND_OPCODE_ITEM(IFCMPIEQW,    ifcmpieqw,    OFF32)  \
	BOND_OPCODE_ITEM(IFCMPINE,     ifcmpine,     OFF16)  \
	BOND_OPCODE_ITEM(IFCMPINEW,    ifcmpinew,    OFF32)  \
	BOND_OPCODE_ITEM(IFCMPILT,     ifcmpilt,     OFF16)  \
	BOND_OPCODE_ITEM(IFCMPILTW,    ifcmpiltw,    OFF32)  \
	BOND_OPCODE_ITEM(IFCMPILE,     ifcmpile,     OFF16)  \
	BOND_OPCODE_ITEM(IFCMPILEW,    ifcmpilew,    OFF32)  \
	BOND_OPCODE_ITEM(IFCMPIGT,     ifcmpigt,     OFF16)  \
	BOND_OPCODE_ITEM(IFCMPIGTW,    ifcmpigtw,    OFF32)  \
	BOND_OPCODE_ITEM(IFCMPIGE,     ifcmpige,     OFF16)  \
	BOND_OPCODE_ITEM(IFCMPIGEW,    ifcmpigew,    OFF32)  \
	                                                     \
	BOND_OPCODE_ITEM(IFCMPUILT,    ifcmpuilt,    OFF16)  \
	BOND_OPCODE_ITEM(IFCMPUILTW,   ifcmpuiltw,   OFF32)  \
	BOND_OPCODE_ITEM(IFCMPUILE,    ifcmpuile,    OFF16)  \
	BOND_OPCODE_ITEM(IFCMPUILEW,   ifcmpuilew,   OFF32)  \
	BOND_OPCODE_ITEM(IFCMPUIGT,    ifcmpuigt,    OFF16)  \
	BOND_OPCODE_ITEM(IFCMPUIGTW,   ifcmpuigtw,   OFF32)  \
	BOND_OPCODE_ITEM(IFCMPUIGE,    ifcmpuige,    OFF16)  \
	BOND_OPCODE_ITEM(IFCMPUIGEW,   ifcmpuigew,   OFF32)  \
	                                                     \
	BOND_OPCODE_ITEM(GOTO,         goto,         OFF16)  \
	BOND_OPCODE_ITEM(GOTOW,        gotow,        OFF32)  \
	                                                     \
	BOND_OPCODE_ITEM(INVOKE,       invoke,       HASH)   \
	BOND_OPCODE_ITEM(INVOKENATIVE, invokenative, HASH)   \
	                                                     \
	BOND_OPCODE_ITEM(RETURN,       return,       NONE)   \


enum OpCode
{
#define BOND_OPCODE_ITEM(opCode, mnemonic, arg) OPCODE_ ## opCode,
	BOND_OPCODE_LIST
#undef BOND_OPCODE_ITEM
	OPCODE_MAX
};


enum OpCodeParam
{
	OC_PARAM_NONE,
	OC_PARAM_CHAR,
	OC_PARAM_UCHAR,
	OC_PARAM_SHORT,
	OC_PARAM_USHORT,
	OC_PARAM_INT,
	OC_PARAM_VAL32,
	OC_PARAM_VAL64,
	OC_PARAM_OFF16,
	OC_PARAM_OFF32,
	OC_PARAM_HASH,
};


const char *GetOpCodeMnemonic(OpCode opCode);
OpCodeParam GetOpCodeParamType(OpCode opCode);

}

#endif