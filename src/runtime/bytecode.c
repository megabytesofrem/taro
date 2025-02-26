/**
 * Bytecode executor. Part of the runtime.
 *
 * Authors:
 * - Charlotte (megabytesofrem)
 */

#include "bytecode.h"
#include "../util/logger.h"
#include "vm.h"

uint32_t encode_operand(VMOperand operand) {
    uint32_t bits = 0;
    bits |= operand.type;

    switch (operand.type) {
    case TY_INT:
        bits |= operand.int_value << 8;
        break;
    case TY_FLOAT:
        // TODO: encode this to be smaller
        float float_bits;
        memcpy(&float_bits, &operand.float_value, sizeof(float));
        bits |= (uint32_t)float_bits << 8;
        break;
    default:
        log_error(__FILE__ ": cannot encode an operand of this type\n");
    }

    return bits;
}

void decode_operand(uint32_t encoded, VMOperand *operand) {
    operand->type = (encoded & 0b11);

    switch (operand->type) {
    case TY_INT:
        operand->int_value = (encoded >> 8);
        break;
    case TY_FLOAT:
        // TODO: encode this to be smaller
        float float_bits = (encoded >> 8);
        memcpy(&operand->float_value, &float_bits, sizeof(float));
        break;
    default:
        log_error(__FILE__ ": cannot decode an operand of this type\n");
    }
}
