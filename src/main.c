#include "lexer.h"
#include "vm/gc.h"
#include "vm/vm.h"

#include <stdio.h>
#include <strings.h> // For bzero
#include <unistd.h>

HeapObject *create_object(VM *vm)
{
    HeapObject *obj = (HeapObject *)vm_heap_alloc(vm, 0);
    Value **val = (Value **)malloc(3 * sizeof(Value *));
    val[0] = &INT(1);
    val[1] = &INT(2);
    val[2] = &INT(3);

    obj->value = &ARRAY(val, 3, 3, FIXED_ARRAY);
    return obj;
}

int main()
{
    // Lexer l = lexer_init("if x >= 1234 then\n0.1234\n else def end");

    // VM testing ground
    VM vm;
    vm_init(&vm, VM_DEFAULT_GC_THRESHOLD);

    // VMInstruction instructions[] = {
    //     INST(OP_ALLOC, 1, value_create_int(1)),
    //     INST(OP_ALLOC, 1, value_create_int(2)),
    // };

    // vm_load(&vm, instructions, 3);

    for (int i = 0; i < 5000; i++) {
        HeapObject *obj = create_object(&vm);
        printf("created object at %p\n", (void *)obj);
    }

    while (true) {
        sleep(1);
        vm_cycle(&vm);
    }

    sleep(2);
    vm_cleanup(&vm);

    return 0;
}