#include "lexer.h"
#include "runtime/gc.h"
#include "runtime/vm.h"

#include <stdio.h>
#include <strings.h> // For bzero

int main()
{
    // Lexer l = lexer_init("if x >= 1234 then\n0.1234\n else def end");

    // VM testing ground
    VM vm;
    vm_init(&vm);

    Block *block1 = (Block *)vm_alloc_block(&vm, VM_PAGE_SIZE);
    Block *block2 = (Block *)vm_alloc_block(&vm, VM_PAGE_SIZE);

    Object **arr = (Object **)malloc(2 * sizeof(Object *));
    arr[0] = object_create_int(1234);
    arr[1] = object_create_int(4321);

    block1->obj = object_create_array(arr, 2, 2, true);

    if (block1 != NULL)
        printf("block1: %p\n", (void *)block1);

    if (block2 != NULL)
        printf("block2: %p\n", (void *)block2);

    block2->obj = object_create_int(1234);

    // Trigger GC cleanup
    // gc_collect(&vm);

    vm_free_block(&vm, block1);
    vm_free_block(&vm, block2);

    vm_cleanup(&vm);

    return 0;
}