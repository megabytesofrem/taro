#ifndef TARO_IL_ASSEMBLE_H
#define TARO_IL_ASSEMBLE_H

#include "../runtime/value.h"
#include "../runtime/vm.h"
#include <stdint.h>

void asm_value(Value *val, uint8_t *buf, size_t size);
void asm_instruction(VMInstruction *ins, uint8_t *buf, size_t size);
void disasm_value(uint8_t *in_buf, Value *buf, size_t size);
void disasm_instruction(uint8_t *in_buf, VMInstruction *ins, size_t size);

#endif