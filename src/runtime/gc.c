#include "gc.h"

/** Primitive GC algorithm */

void gc_mark(Object *obj)
{
    if (obj == NULL || obj->marked)
        return;

    // Mark the object
    obj->marked = true;

    // Managed objects hold references to other objects
    if (object_is_gc_managed(obj)) {
        log_info("marking object at %p\n", (void *)obj);
        for (int i = 0; i < obj->s_children_count; i++) {
            if (obj->s_children[i] != NULL) {
                gc_mark(obj->s_children[i]);
            } else {
                log_info("skipping NULL child at index %d\n", i);
            }
        }
    }
}

void gc_mark_all(VM *vm)
{
    Block *block = vm->heap_alloc_list;
    while (block) {
        gc_mark(block->obj);
        block = block->next;
    }
}

void gc_sweep(VM *vm)
{
    Block **block = &vm->heap_alloc_list;
    while (*block) {
        Object *obj = (*block)->obj;

        if (obj->marked) {
            obj->marked = false; // Unmark for the next GC cycle
            block = &(*block)->next;
        } else {
            Block *unmarked = *block;

            // Remove the block from the allocation list
            *block = unmarked->next;

            // Free the object and add the block to the free list
            free(unmarked->obj);
            unmarked->obj = NULL;

            unmarked->next = vm->heap_free_list;
            vm->heap_free_list = unmarked;
        }
    }
}

void gc_collect(VM *vm)
{
    printf("Before GC:\n");
    Block *block = vm->heap_alloc_list;
    while (block) {
        printf("block at %p, object at %p\n", (void *)block, (void *)block->obj);
        block = block->next;
    }

    gc_mark_all(vm);
    gc_sweep(vm);

    printf("After GC:\n");
    block = vm->heap_alloc_list;
    while (block) {
        printf("block at %p, object at %p\n", (void *)block, (void *)block->obj);
        block = block->next;
    }
}