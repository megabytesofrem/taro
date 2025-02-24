#ifndef TARO_DISASM_H
#define TARO_DISASM_H

#include "value.h"
#include "vm.h"
#include <stdint.h>

#define OPCODE_MASK 0xFF000000
#define PARAMS_MASK 0x00FF0000
#define TYPE_MASK 0x0000FF00
#define VALUE_MASK 0x000000FF

// uint32_t encode_value(Value *val);
// void decode_value(uint32_t encoded, Value *val);

uint32_t encode_operand(VMOperand *operand);
void decode_operand(uint32_t encoded, VMOperand *operand);

uint32_t assemble_instruction(VMInstruction *ins);
void disassemble_instruction(uint32_t encoded, VMInstruction *ins);
void print_disassembly(uint32_t encoded, VMInstruction *ins);

#endif
