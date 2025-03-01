/**
 * Bytecode representation. Part of the runtime.
 *
 * Authors:
 * - Charlotte (megabytesofrem)
 */

#include "bytecode.h"
#include "../util/common.h"
#include "../util/logger.h"
#include "value.h"
#include "vm.h"

// static int serialize_operand(VMOperand operand, uint8_t *buffer);
static int deserialize_operand(uint8_t *buffer, VMOperand *operand);

/* Encode and decode instructions */
static inline int encode_instruction(int op, int operands_count, VMOperand *operands,
                                     uint8_t *output);
static inline int decode_instruction(uint8_t *buffer, VMInstruction *inst);

static int deserialize_operand(uint8_t *input, VMOperand *output) {
    output->type = input[0];

    switch (output->type) {
    case TY_INT:
        memcpy(&output->int_value, input + 2, sizeof(int));
        break;
    case TY_FLOAT:
        memcpy(&output->float_value, input + 2, sizeof(float));
        break;
    default:
        log_error(__FILE__ ": invalid operand type %d\n");
        return -1;
    }

    return 0;
}

static inline int encode_instruction(int op, int operands_count, VMOperand *operands,
                                     uint8_t *output) {
    size_t nbytes  = 0;
    uint8_t header = (op << 4) | operands_count;
    output[0]      = header;

    log_info(__FILE__ ": header encoded as: %02x\n", header);

    size_t offset = 1;
    VMOperand it;
    foreach (it, operands, operands_count) {
        output[offset++] = it.type;
        output[offset++] = 0; // padding for future size of operand

        switch (it.type) {
        case TY_INT:
            memcpy(output + offset, &it.int_value, sizeof(int));
            offset += sizeof(int);
            break;
        case TY_FLOAT:
            memcpy(output + offset, &it.float_value, sizeof(float));
            offset += sizeof(float);
            break;
        default:
            log_error(__FILE__ ": invalid operand type %d\n");
            return -1;
        }
    }

    nbytes = offset;
    return nbytes;
}

static inline int decode_instruction(uint8_t *input, VMInstruction *output) {
    output->opcode         = input[0] >> 4;
    output->operands_count = input[0] & 0x0F;

    // opcode operand_count [rest] opcode operand_count [rest] ...

    size_t offset = 1;
    foreach_n(i, input, output->operands_count) {
        if (deserialize_operand(input + offset, &output->operands[i]) != 0) {
            log_error(__FILE__ ": failed to deserialize operand\n");
            return -1;
        }

        offset += 6;
    }

    log_info("Decoded instruction: %d (number of operands: %d)\n", output->opcode,
             output->operands_count);

    return 0;
}

int read_bytecode_stream(uint8_t *stream, size_t len, VMInstruction *out_insts,
                         size_t *out_count) {

    // Read len instructions from the stream and decode an instruction stream
    size_t offset    = 0;
    size_t curr_inst = 0;

    VMInstruction inst;

    while (offset < len) {
        if (decode_instruction(stream + offset, &inst) != 0) {
            log_error(__FILE__ ": failed to decode instruction\n");
            return -1;
        }

        out_insts[curr_inst++] = inst;
        offset += 5;
    }

    *out_count = curr_inst;
    return 0;
}

int read_bytecode_file(const char *filename, struct Bytecode *bc) {
    FILE *file   = fopen(filename, "rb");
    uint8_t *buf = NULL;
    size_t len   = 0;

    if (!file) {
        log_error("failed to open file: %s\n", filename);
        perror("failed to open file");
        return -1;
    }

    fseek(file, 0, SEEK_END);
    len = ftell(file);
    rewind(file);

    // Allocate a buffer
    buf      = malloc(len + 1);
    buf[len] = '\0';

    // Read the header
    size_t read_size;
    if ((read_size = fread(&bc->header, sizeof(struct BytecodeHdr), 1, file)) != 1) {
        printf("read size: %zu\n", read_size);
        perror("fread");
        fclose(file);
        return EXIT_FAILURE;
    }

    if (strncmp(bc->header.magic, "TARO", 4) != 0) {
        perror("magic number did not match");
        fclose(file);
        return EXIT_FAILURE;
    }

    printf("magic: %s\n", bc->header.magic);
    printf("version: %d\n", bc->header.version);
    printf("code size: %d\n", bc->header.code_size);

    bc->code = (uint8_t *)malloc(bc->header.code_size);
    fread(&bc->code, sizeof(bc->code), bc->header.code_size, file);
    fclose(file);

    return EXIT_SUCCESS;
}
