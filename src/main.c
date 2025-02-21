#include "lexer.h"
#include "runtime/gc.h"
#include "runtime/value.h"
#include "runtime/vm.h"

#include <stdio.h>
#include <strings.h> // For bzero
#include <unistd.h>

HeapObj *create_object(VM *vm)
{
    HeapObj *obj = (HeapObj *)heap_alloc(&vm->mem, 0);
    Value **val  = (Value **)malloc(3 * sizeof(Value *));
    val[0]       = &new_int(1);
    val[1]       = &new_int(2);
    val[2]       = &new_int(3);

    obj->value = &new_array(val, 3, 3, FIXED_ARRAY);
    return obj;
}

int main()
{
    // Lexer l = lexer_init("if x >= 1234 then\n0.1234\n else def end");

    // VM testing ground
    VM vm;
    vm_init(&vm, VM_DEFAULT_GC_THRESHOLD);

    // VMInstruction instructions[1] = {
    //     {.opcode = LOAD_INT, .operands_count = 1},
    // };

    VMInstruction instructions[6] = {
        {.opcode = PUSHI, .operands_count = 1, .operands = {{.int_value = 1}}},
        {.opcode = PUSHI, .operands_count = 1, .operands = {{.int_value = 2}}},
        {.opcode = ADDI, .operands_count = 0},

        {.opcode         = STORES,
         .operands_count = 2,
         .operands       = {{.string_value = "x"}, {.string_value = "test"}}},

        {.opcode = LOADS, .operands_count = 1, .operands = {{.string_value = "x"}}},
    };

    vm_load(&vm, instructions, 6);

    // for (int i = 0; i < 1000; i++) {
    //     HeapObj *obj = create_object(&vm);
    //     // printf("created object at %p\n", (void *)obj);
    // }

    while (true) {
        sleep(1);
        vm_cycle(&vm);
    }

    sleep(2);
    vm_cleanup(&vm);

    return 0;
}
