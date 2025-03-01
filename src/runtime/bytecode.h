#ifndef TARO_BYTECODE_H
#define TARO_BYTECODE_H

#include "../util/common.h"
#include "vm.h"
#include <stdint.h>

struct packed_t BytecodeHdr {
    char magic[4]; // TARO
    int version;   // Binary version

    int code_size;
};

/**
 * Executable bytecode format.
 */
struct packed_t Bytecode {
    struct packed_t BytecodeHdr header;

    uint8_t *code;
};

int read_bytecode_stream(uint8_t *stream, size_t len, VMInstruction *out_insts,
                         size_t *out_count);

int read_bytecode_file(const char *filename, struct Bytecode *bc);

#endif
