#include "lexer.h"
#include "runtime/gc.h"
#include "runtime/vm.h"

#include <stdio.h>
#include <strings.h> // For bzero
#include <unistd.h>

HeapObject *create_object(VM *vm)
{
    HeapObject *obj = (HeapObject *)vm_heap_alloc(vm, 0);
    Value **val = (Value **)malloc(3 * sizeof(Value *));
    val[0] = value_create_int(1);
    val[1] = value_create_int(2);
    val[2] = value_create_int(3);

    obj->obj = value_create_array(val, 3, 3, true);
    return obj;
}

int main()
{
    // Lexer l = lexer_init("if x >= 1234 then\n0.1234\n else def end");

    // VM testing ground
    VM vm;
    vm_init(&vm);

    for (int i = 0; i < 5; i++) {
        HeapObject *obj = create_object(&vm);
        printf("created object at %p\n", (void *)obj);
    }

    sleep(2);
    vm_cleanup(&vm);

    return 0;
}