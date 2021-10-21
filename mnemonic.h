#include <stdint.h>

const struct {
    uint32_t word;
    char mnemo[5];
} mnemo[] = {
    /* List of the mnemonics associated with the corresponding opcode */
    { 0x0f000000, "LDW" },
    { 0x0e800000, "LWS" },
    { 0x0e000000, "LWU" },
    { 0x0d800000, "LHS" },
    { 0x0d000000, "LHU" },
    { 0x0c800000, "LBS" },
    { 0x0c000000, "LBU" },
    { 0x0b000000, "STD" },
    { 0x0a000000, "STW" },
    { 0x09000000, "STH" },
    { 0x08000000, "STB" },
    { 0x0a800000, "STW" },
    { 0x09800000, "STH" },
    { 0x08800000, "STB" },
    { 0x0f800000, "LLD" },
    { 0x0b800000, "SCD" },

    { 0x00600000, "ADD" },
    { 0x00e00000, "SUB" },
    { 0x04600000, "AND" },
    { 0x04e00000, "ORR" },
    { 0x05600000, "EOR" },
    { 0x01600000, "ASL" },
    { 0x02600000, "LSR" },
    { 0x01e00000, "ASR" },
    { 0x03600000, "ROL" },
    { 0x02e00000, "ROR" },
    { 0x05e00000, "MUL" },
    { 0x03800000, "ADC" },
    { 0x03a00000, "SBC" },
    { 0x03e00000, "RLC" },
    { 0x03c00000, "RRC" },

    { 0x00600000, "ADDD" }, /* unused in disassembly */
    { 0x00400000, "ADDW" },
    { 0x00200000, "ADDH" },
    { 0x00000000, "ADDB" },

    { 0x00e00000, "SUBD" }, /* unused in disassembly */
    { 0x00c00000, "SUBW" },
    { 0x00a00000, "SUBH" },
    { 0x00800000, "SUBB" },

    { 0x01600000, "ASLD" }, /* unused in disassembly */
    { 0x01400000, "ASLW" },
    { 0x01200000, "ASLH" },
    { 0x01000000, "ASLB" },

    { 0x01e00000, "ASRD" }, /* unused in disassembly */
    { 0x01c00000, "ASRW" },
    { 0x01a00000, "ASRH" },
    { 0x01800000, "ASRB" },

    { 0x02600000, "LSRD" }, /* unused in disassembly */
    { 0x02400000, "LSRW" },
    { 0x02200000, "LSRH" },
    { 0x02000000, "LSRB" },

    { 0x03600000, "ROLD" }, /* unused in disassembly */
    { 0x03400000, "ROLW" },
    { 0x03200000, "ROLH" },
    { 0x03000000, "ROLB" },

    { 0x02e00000, "RORD" }, /* unused in disassembly */
    { 0x02c00000, "RORW" },
    { 0x02a00000, "RORH" },
    { 0x02800000, "RORB" },

    { 0x05c00000, "SUMW" },
    { 0x05a00000, "SUMH" },
    { 0x05800000, "SUMB" },

    { 0x06000000, "BAL" },
    { 0x56000000, "BEQ" },
    { 0x46000000, "BNE" },
    { 0xf6000000, "BMI" },
    { 0xe6000000, "BPL" },
    { 0xd6000000, "BVS" },
    { 0xc6000000, "BVC" },
    { 0x36000000, "BCS" },
    { 0x26000000, "BCC" },
    { 0xb6000000, "BHI" },
    { 0xa6000000, "BLQ" },
    { 0x76000000, "BGT" },
    { 0x66000000, "BLE" },
    { 0x96000000, "BGE" },
    { 0x86000000, "BLT" },
    { 0x36000000, "BHQ" }, /* unused in disassembly */
    { 0x26000000, "BLO" }, /* unused in disassembly */

    { 0x06200000, "BLK" },

    { 0x07400000, "TCR" },
    { 0x07600000, "TRC" },
    { 0x07800000, "TSR" },
    { 0x07a00000, "TRS" },
    { 0x07c00000, "RTI" },
    { 0x07e00000, "SYS" },

};

const struct {
    uint32_t word;
    char prefix[4];
} prefix[] = {
    /* List of all conditionnal prefixes */
    { 0x00000000, "AL." }, /*unused in disassembly */
    { 0x50000000, "EQ." },
    { 0x40000000, "NE." },
    { 0xf0000000, "MI." },
    { 0xe0000000, "PL." },
    { 0xd0000000, "VS." },
    { 0xc0000000, "VC." },
    { 0x30000000, "CS." },
    { 0x20000000, "CC." },
    { 0xb0000000, "HI." },
    { 0xa0000000, "LQ." },
    { 0x70000000, "GT." },
    { 0x60000000, "LE." },
    { 0x90000000, "GE." },
    { 0x80000000, "LT." },
    { 0x30000000, "HQ." }, /* unused in disassembly */
    { 0x20000000, "LO." }, /* unused in disassembly */

};

const char * reg_name[] = { 
    "R0",
    "R1",
    "R2",
    "R3",
    "R4",
    "R5",
    "R6",
    "R7",
    "R8",
    "R9",
    "R10",
    "R11",
    "R12",
    "R13",
    "R14",
    "PC",
    "R16",
    "R17",
    "R18",
    "R19",
    "R20",
    "R21",
    "R22",
    "R23",
    "R24",
    "R25",
    "R26",
    "R27",
    "R28",
    "R29",
    "R30",
    "LR"
};

enum {
    R0,
    R1,
    R2,
    R3,
    R4,
    R5,
    R6,
    R7,
    R8,
    R9,
    R10,
    R11,
    R12,
    R13,
    R14,
    PC,
    R16,
    R17,
    R18,
    R19,
    R20,
    R21,
    R22,
    R23,
    R24,
    R25,
    R26,
    R27,
    R28,
    R29,
    R30,
    LR
};
    

/* All values below are masks */
#define CONDITION_MASK     0xf0000000 /* used to select conditionnal prefixes */
#define SUFFIX_BIT         0x00100000 /* used to select the suffix -S bit */
#define MEMORY_BIT         0x08000000 /* used to determine if the instruction is
                                         a load/store */ 
#define MEMORY_MASK        0x0f800000 /* used to select the bits that correspond
                                         to a memory instruction, without the 
                                         addressing mode */
#define ALU_MASK           0x0fe00000 /* used to select the bits that correspond
                                         to a register/ALU instruction */
#define DEST_MASK          0x0000001f /* used to select the destination register*/
#define REGX_MASK          0x000003e0 /* used to select the first register
                                         operand in case of a register operation.
                                         Select the base register in case of a
                                         load/store */
#define REGY_MASK          0x00007c00 /* used to select the second register
                                         operand in case of a register operation.
                                         Select the index register in case of a
                                         load/store */
#define ALU_MODE_MASK      0x00080000 /* used to select the addressing mode of an
                                         ALU operation (immediate or second
                                         register) */
#define ALU_IMM_MASK       0x0007fc00 /* determine the immediate value for an
                                         ALU operation */
#define ALU_SHIFT_MASK     0x00078000 /* determine the shift amount for an
                                         ALU operation */
#define MEMORY_MODE_MASK   0x00600000 /* used to select the addressing mode of a
                                         load/store (direct, indexed, base index
                                         shifted, auto-indexing mode) */
#define MEMORY_SHIFT_MASK  0x000f8000 /* used to know the shift amount in case
                                         of a load/store with the base index
                                         shifted addressing mode */
#define MEMORY_DIRECT_MASK 0x000fffe0 /* used to determine the address in case
                                         of a load/store with direct addressing
                                         mode */
#define MEMORY_AUTO_MASK   0x000c0000 /* used to know if the index is
                                         pre(post)inc(dec)remented */
#define MEMORY_I_OFF_MASK  0x000ffc00 /* used to select the offset if the
                                         addressing mode is the indexed mode */
#define BRANCH_MASK        0xffe00000 /* selects the bits for conditionnal
                                         branches */
#define BRANCH_OFF_MASK    0x0007ffe0 /* selects the offset bits of a branch
                                         instruction*/
/* Macro-instructions definition */
/* The macro-instruction below is used to determine if the instruction is a
 * conditionnal branch or not. This macro is useful because I don't want to see
 * cc.BAL instead of Bcc in our dissasembled code. For example I prefer to BEQ
 * and not EQ.BAL */
#define is_branch_instruction(a) !(((a) ^ 0x06000000) & ALU_MASK)
/* Selects the bits and shift them to know the condionnal prefix between 0 and
 * 15 */
#define extract_condition(a) ((a) & CONDITION_MASK)
/* Determine if the instruction is a load/store or not */
#define is_memory_instruction(a) ((a) & MEMORY_BIT)
/* Returns the destination register */
#define extract_rd(a) ((a) & DEST_MASK)
/* Returns Rx */
#define extract_rx(a) (((a) & REGX_MASK) >> 5)
/* Returns Ry */
#define extract_ry(a) (((a) & REGY_MASK) >> 10)
/* returns true if there is the suffix bit */
#define is_suffix(a) ((a) & SUFFIX_BIT)
/* returns true if the the addressing mode is immediate */
#define is_immediate(a) !((a) & ALU_MODE_MASK)
/* returns the immediate value */
#define extract_imm(a) (((a) & ALU_IMM_MASK) >> 10)
