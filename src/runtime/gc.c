#include "gc.h"

#include <pthread.h>
#include <unistd.h>

/**
 * Mark and sweep garbage collector written overnight.
 */

void vm_heap_free(VM *vm, void *pentry);

void gc_mark(Value *val)
{
    if (val == NULL || val->marked)
        return;

    // Mark the object
    val->marked = true;

    printf("GC: marking value at %p\n", (void *)val);

    // Managed objects hold references to other objects
    if (value_is_gc_managed(val)) {
        if (val->s_children != NULL) {
            for (int i = 0; i < val->s_children_count; i++) {
                if (val->s_children[i] != NULL) {
                    printf("GC: marking child %d at %p\n", i, (void *)val->s_children[i]);
                    gc_mark(val->s_children[i]);
                }
            }
        }
    }
}

void gc_mark_all(VM *vm)
{
    // Iterate through the heap allocation list and mark all objects
    printf("GC: marking all objects in the heap\n");

    HeapObject *entry = vm->heap;
    while (entry) {
        if (entry->obj != NULL) {
            printf("GC: marking object at %p\n", (void *)entry->obj);
            gc_mark(entry->obj);
        }
        entry = entry->next;
    }
}

void gc_sweep(VM *vm)
{
    printf("GC: performing a sweep\n");
    HeapObject *entry = vm->heap;

    while (entry) {
        if (entry->obj != NULL) {
            printf("GC: examining entry at %p, marked: %d\n", (void *)entry,
                   entry->obj->marked);
        }

        if (entry->obj == NULL || !entry->obj->marked) {
            // This entry was not reached, so free it
            HeapObject *unreached = entry;
            entry = unreached->next;

            vm_heap_free(vm, unreached);
        } else {
            // This entry was reached, so unmark it for the next GC cycle
            printf("GC: unmarking object at %p for next cycle\n", (void *)entry->obj);
            entry->obj->marked = false;
            entry = entry->next;
        }
    }
}

void gc_collect(VM *vm)
{
    printf("\n\nBefore GC:\n\n");
    HeapObject *entry = vm->heap;
    while (entry != NULL) {
        printf("entry at %p, is free: %d\n", (void *)entry, entry->free);
        entry = entry->next;
    }

    gc_mark_all(vm);

    // Sweep once and then once more to ensure all unreachable objects are freed
    gc_sweep(vm);
    gc_sweep(vm);
}