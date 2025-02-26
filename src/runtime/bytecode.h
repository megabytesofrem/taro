#ifndef TARO_BYTECODE_H
#define TARO_BYTECODE_H

#include "vm.h"
#include <stdint.h>

/* Typedef this so I don't have to type all this */
#undef _packed
#define _packed __attribute__((__packed__))

struct _packed BcHdr {
    char magic[4]; // TARO
    int version;   // Binary version
};

/**
 * Executable bytecode format.
 */
struct _packed Bytecode {
    struct _packed BcHdr header;

    uint32_t **code;
};

uint32_t encode_operand(VMOperand operand);
void decode_operand(uint32_t encoded, VMOperand *operand);

uint32_t encode_vm_ins(VMInstruction ins);
void decode_vm_ins(uint32_t encoded, VMInstruction *ins);

#endif
