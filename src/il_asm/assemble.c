#include "assemble.h"
#include "../runtime/value.h"
#include "../runtime/vm.h"
#include "../util/logger.h"

/**
 * Assembly and disassembly of values and instructions.
 *
 * The format for assembling value types into bytecode is as follows:
 * - Byte 1: Type of the value (an integer)
 *
 * For integers and floats:
 * - Byte 2-5: The integer or float value
 * - For strings:
 * - Byte 2-n: The string value
 *
 * For arrays:
 * - Byte 2: The count of children
 * - Byte 3: The capacity of the array
 * - Byte 4: A flag indicating if the array is growable
 * - Byte 5: The type of the children in the array
 * - Byte 6-n: The children of the array
 *
 * For structures:
 * Currently not implemented, but it is the same principle as arrays.
 *
 * The format for disassembling instructions into bytecode is as follows:
 * - Byte 1: The opcode of the instruction
 * - Byte 2: The count of operands
 * - Byte 3-n: A pair of two bytes where the first byte is the type of the operand
 * - and the second byte is the data of the operand, n times
 */

void asm_value(Value *val, uint8_t *buf, size_t size)
{
    // First byte encodes the data type

    // For an array it would look like:
    // [type][count][capacity][growable][child_type][child1][child2]...

    buf[0] = val->type;

    switch (val->type) {
    case TY_INT:
        memcpy(buf + 1, &val->data.int_value, sizeof(int));
        break;
    case TY_FLOAT:
        memcpy(buf + 1, &val->data.float_value, sizeof(float));
        break;
    case TY_STRING:
        memcpy(buf + 1, val->data.string_value, size - 1);
        break;
    case TY_FIXEDARRAY:
    case TY_GROWARRAY:
        // Next two bytes encode the count and capacity of the array
        buf[1] = val->s_children_count;
        buf[2] = val->s_children_capacity;
        buf[3] = (val->type == TY_FIXEDARRAY ? 0 : 1); // Growable array flag

        // Next byte encodes the overall type of the array
        buf[4] = val->s_children[0]->type;

        for (int i = 0; i < val->s_children_count; i++) {
            buf[5 + i] = val->s_children[i]->data.int_value;
        }
    case TY_STRUCTURE:
        log_error("VM: assembling structures not implemented\n");
        break;
    default:
        break;
    }
}

void asm_instruction(VMInstruction *ins, uint8_t *buf, size_t size)
{
    // First byte encodes the opcode

    // For an instruction with operands it would look like:
    // [opcode][operand1_type][operand1_data]...

    buf[0]            = ins->opcode;
    int operand_count = ins->operands_count;

    for (int i = 0; i < operand_count; i++) {
        buf[1 + i] = ins->operands[i].type;

        switch (ins->operands[i].type) {
        case TY_INT:
            memcpy(buf + 2 + i, &ins->operands[i].int_value, sizeof(int));
            break;
        case TY_FLOAT:
            memcpy(buf + 2 + i, &ins->operands[i].float_value, sizeof(float));
            break;
        case TY_STRING:
            memcpy(buf + 2 + i, ins->operands[i].string_value, size - 2);
            break;
        case TY_FIXEDARRAY:
        case TY_GROWARRAY:
            log_error("VM: assembling arrays not implemented\n");
            break;
        case TY_STRUCTURE:
            log_error("VM: assembling structures not implemented\n");
            break;
        default:
            break;
        }
    }
}

void disasm_value(uint8_t *inbuf, Value *buf, size_t size)
{
    // First byte encodes the data type

    // For an array it would look like:
    // [type][count][capacity][growable][child_type][child1][child2]...

    buf->type = inbuf[0];

    switch (buf->type) {
    case TY_INT:
        memcpy(&buf->data.int_value, inbuf + 1, sizeof(int));
        break;
    case TY_FLOAT:
        memcpy(&buf->data.float_value, inbuf + 1, sizeof(float));
        break;
    case TY_STRING:
        memcpy(buf->data.string_value, inbuf + 1, size - 1);
        break;
    case TY_FIXEDARRAY:
    case TY_GROWARRAY:
        buf->s_children_count    = inbuf[1];
        buf->s_children_capacity = inbuf[2];

        if (inbuf[3] == 1) {
            buf->type = TY_GROWARRAY;
        }

        buf->s_children = malloc(buf->s_children_capacity * sizeof(Value *));

        for (int i = 0; i < buf->s_children_count; i++) {
            buf->s_children[i]       = malloc(sizeof(Value));
            buf->s_children[i]->type = inbuf[4];
            memcpy(&buf->s_children[i]->data.int_value, inbuf + 5 + i, sizeof(int));
        }
    case TY_STRUCTURE:
        log_error("VM: disassembling structures not implemented\n");
        break;
    default:
        break;
    }
}

void disasm_instruction(uint8_t *in_buf, VMInstruction *ins, size_t size)
{
    // First byte encodes the opcode

    // For an instruction with operands it would look like:
    // [opcode][operand1_type][operand1_data]...

    ins->opcode = in_buf[0];
    int operand_count = ins->operands_count;

    for (int i = 0; i < operand_count; i++) {
        ins->operands[i].type = in_buf[1 + i];

        switch (ins->operands[i].type) {
        case TY_INT:
            memcpy(&ins->operands[i].int_value, in_buf + 2 + i, sizeof(int));
            break;
        case TY_FLOAT:
            memcpy(&ins->operands[i].float_value, in_buf + 2 + i, sizeof(float));
            break;
        case TY_STRING:
            memcpy(ins->operands[i].string_value, in_buf + 2 + i, size - 2);
            break;
        case TY_FIXEDARRAY:
        case TY_GROWARRAY:
            log_error("VM: disassembling arrays not implemented\n");
            break;
        case TY_STRUCTURE:
            log_error("VM: disassembling structures not implemented\n");
            break;
        default:
            break;
        }
    }
}
