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

#ifdef GC_DEBUG
    printf("GC: marking value at %p\n", (void *)val);
#endif

    // Managed objects hold references to other objects
    if (value_has_gc_roots(val)) {
        if (val->s_children != NULL) {
            for (int i = 0; i < val->s_children_count; i++) {
                if (val->s_children[i] != NULL) {
#ifdef GC_DEBUG
                    printf("GC: marking child %d at %p\n", i, (void *)val->s_children[i]);
#endif
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
    while (entry != NULL) {
        if (entry->value != NULL) {
#ifdef GC_DEBUG
            printf("GC: marking object at %p\n", (void *)entry->value);
#endif
            gc_mark(entry->value);
        }
        entry = entry->next;
    }
}

void gc_sweep(VM *vm)
{
    printf("GC: performing a sweep\n");

    // HeapObject *prev = NULL;
    HeapObject *entry = vm->heap;

    while (entry != NULL) {
        if (entry->value != NULL) {
#ifdef GC_DEBUG
            printf("GC: examining entry at %p, marked: %d\n", (void *)entry,
                   entry->value->marked);
#endif
        }

        if (entry->value == NULL || !entry->value->marked) {
            // Unreached entry so let's free it
            HeapObject *unreached = entry;
            entry = entry->next;

            if (unreached != NULL) {
                vm_heap_free(vm, unreached);
            }
        } else {
#ifdef GC_DEBUG
            // This entry was reached, so unmark it for the next GC cycle
            printf("GC: unmarking object at %p for next cycle\n", (void *)entry->value);
#endif
            entry->value->marked = false;
            entry = entry->next;
        }
    }
}

void gc_collect(VM *vm)
{
    gc_mark_all(vm);

    // Sweep once and then once more to ensure all unreachable objects are freed
    gc_sweep(vm);
    gc_sweep(vm);
    vm->gc_counter = 0;
}