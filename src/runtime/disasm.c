/**
 * disasm.c -- Assembly and disassembly of instructions
 *
 * Authors:
 * - Charlotte (megabytesofrem)
 */

#include "disasm.h"
#include "../util/logger.h"

/*
uint32_t encode_value(Value *val)
{
    uint32_t bits = 0;
    bits          = (val->type << 8);

    switch (val->type) {
    case TY_INT:
        bits |= (val->data.int_value & VALUE_MASK);
        break;
    case TY_FLOAT:
        uint32_t float_bits;
        memcpy(&float_bits, &val->data.float_value, sizeof(float));
        bits |= (float_bits & VALUE_MASK);
        break;
    case TY_STRING:
        bits |= (strlen(val->data.string_value) & VALUE_MASK);

        // Encode the string as a sequence of bytes
        for (int i = 0; i < strlen(val->data.string_value); i++) {
            bits |= (val->data.string_value[i] << (i * 8));
        }
        break;
    case TY_FIXEDARRAY:
        // Type of the array and length
        bits |= (val->data.array_value.count & VALUE_MASK);
        bits |= (val->data.array_value.capacity & (VALUE_MASK << 8));

        // TODO: Need to work around this limitation of encoding, maybe
        // we can encode part of the instruction as one byte and the rest as n bytes?
        if (val->data.array_value.count > 255) {
            log_error(__FILE__ ": array count exceeds maximum value\n");
        }

        for (int i = 0; i < val->data.array_value.count; i++) {
            bits |= (encode_value(val->data.array_value.values[i]) << (i * 8));
        }
        break;
    case TY_GROWARRAY:
        // Type of the array and length
        bits |= (val->data.array_value.count & VALUE_MASK);
        bits |= (val->data.array_value.capacity & (VALUE_MASK << 8));

        // TODO: Need to work around this limitation of encoding, maybe
        // we can encode part of the instruction as one byte and the rest as n bytes?
        if (val->data.array_value.count > 255) {
            log_error(__FILE__ ": array count exceeds maximum value\n");
        }

        for (int i = 0; i < val->data.array_value.count; i++) {
            bits |= (encode_value(val->data.array_value.values[i]) << (i * 8));
        }
        break;
    default:
        break;
    }

    return bits;
}

void decode_value(uint32_t encoded, Value *val)
{
    val->type = (encoded & TYPE_MASK) >> 8;
    switch (val->type) {
    case TY_INT:
        val->data.int_value = encoded & VALUE_MASK;
        break;
    case TY_FLOAT:
        uint32_t float_bits = encoded & VALUE_MASK;
        memcpy(&val->data.float_value, &float_bits, sizeof(float));
        break;
    case TY_STRING:
        // TODO: Need to work around this limitation of encoding, maybe
        // we can encode part of the instruction as one byte and the rest as n bytes?
        val->data.string_value = malloc((encoded & VALUE_MASK) + 1);
        for (int i = 0; i < (encoded & VALUE_MASK); i++) {
            val->data.string_value[i] = (encoded >> (i * 8)) & 0xFF;
        }
        val->data.string_value[(encoded & VALUE_MASK)] = '\0';
        break;
    default:
        break;
    }
}
*/

uint32_t encode_operand(VMOperand *operand)
{
    uint32_t bits = 0;
    bits          = (operand->type << 24);

    switch (operand->type) {
    case TY_INT:
        bits |= (operand->int_value & 0xFFFFFF);
        break;
    case TY_FLOAT:
        uint32_t float_bits;
        memcpy(&float_bits, &operand->float_value, sizeof(float));
        bits |= (float_bits & 0xFFFFFF);
        break;
    case TY_STRING:
        bits |= (strlen(operand->string_value) & 0xFFFFFF);

        // Encode the string as a sequence of bytes
        for (int i = 0; i < strlen(operand->string_value); i++) {
            bits |= (operand->string_value[i] << (i * 8));
        }
        break;
    default:
        // NOTE: Arrays and structs are not supported as operands to keep the instruction
        // set simple
        log_error(__FILE__ ": unsupported operand type %d\n", operand->type);
        break;
    }

    return bits;
}

void decode_operand(uint32_t encoded, VMOperand *operand)
{
    operand->type = (encoded >> 24) & 0xFF;
    switch (operand->type) {
    case TY_INT:
        operand->int_value = encoded & 0xFFFFFF;
        break;
    case TY_FLOAT:
        uint32_t float_bits = encoded & 0xFFFFFF;
        memcpy(&operand->float_value, &float_bits, sizeof(float));
        break;
    case TY_STRING:
        operand->string_value = malloc((encoded & 0xFFFFFF) + 1);
        for (int i = 0; i < (encoded & 0xFFFFFF); i++) {
            operand->string_value[i] = (encoded >> (i * 8)) & 0xFF;
        }

        operand->string_value[(encoded & 0xFFFFFF)] = '\0';
        break;
    default:
        // NOTE: Arrays and structs are not supported as operands to keep the instruction
        // set simple

        log_error(__FILE__ ": unsupported operand type %d\n", operand->type);
        break;
    }
}

uint32_t assemble_instruction(VMInstruction *ins)
{
    uint32_t bits = 0;
    bits          = (ins->opcode << 24);
    bits |= (ins->operands_count << 16);

    for (int i = 0; i < ins->operands_count; i++) {
        bits |= encode_operand(&ins->operands[i]) << (i * 8);
    }

    return bits;
}

void disassemble_instruction(uint32_t encoded, VMInstruction *ins)
{
    ins->opcode         = (encoded >> 24) & 0xFF;
    ins->operands_count = (encoded >> 16) & 0xFF;

    log_info(__FILE__ ": disassembly: opcode: %d, operands: %d\n", ins->opcode,
             ins->operands_count);

    for (int i = 0; i < ins->operands_count; i++) {
        log_info(__FILE__ ": decoding operand %d\n", i);
        decode_operand((encoded >> (i * 8)) & 0xFF, &ins->operands[i]);
    }
}

void print_disassembly(uint32_t encoded, VMInstruction *ins)
{
    disassemble_instruction(encoded, ins);
    char opcode[18][6] = {"NOP",  "PUSHI", "PUSHF", "POP",    "STORES", "LOADS",
                          "ADDI", "SUBI",  "MUL",   "DIV",    "J",      "J_REL",
                          "J_EQ", "CALL",  "RET",   "STOREL", "LOADL",  "ALLOCA"};

    log_info(__FILE__ ": disassembly: %s\n", opcode[ins->opcode]);

    for (int i = 0; i < ins->operands_count; i++) {
        switch (ins->operands[i].type) {
        case TY_INT:
            log_info("\t%d ", ins->operands[i].int_value);
            break;
        case TY_FLOAT:
            log_info("\t%f ", ins->operands[i].float_value);
            break;
        case TY_STRING:
            log_info("\t%s ", ins->operands[i].string_value);
            break;
        default:
            log_error(__FILE__ ": unsupported operand type %d\n", ins->operands[i].type);
            break;
        }
    }
}
