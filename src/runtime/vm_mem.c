#include "vm_mem.h"

#include "../util/logger.h"

void stack_push(VMMem *mem, Value *value) {
    // Check for stack overflow
    if (mem->sp >= VM_STACK_MAX_SIZE) {
        log_error("VM: stack overflow\n");
        return;
    }

    // Return a pointer to the stack frame
    mem->stack_top[mem->sp++] = *value;
}

Value *stack_pop(VMMem *mem) {
    if (mem->sp == 0) {
        log_error("VM: stack underflow\n");
        return NULL;
    }

    mem->stack_top = &mem->stack[mem->sp - 1];
    mem->sp--;
    return mem->stack_top;
}

void stack_dump(VMMem *mem) {
    log_info("VM: stack dump\n");
    for (int i = 0; i < mem->sp; i++) {
        if (mem->stack[i].type == TY_INT) {
            log_info("  %d: %d\n", i, mem->stack[i].data.int_value);
        } else if (mem->stack[i].type == TY_FLOAT) {
            log_info("  %d: %f\n", i, mem->stack[i].data.float_value);
        } else if (mem->stack[i].type == TY_STRING) {
            log_info("  %d: %s\n", i, mem->stack[i].data.string_value);
        }
    }
}

Value *heap_alloc(VMMem *mem, size_t size) {
    HeapObj *entry = mem->heap;

    // Try to find a free space
    while (entry != NULL) {
        if (entry->free) {
            entry->free = false;
            return (void *)entry;
        }

        entry = entry->next;
    }

    // Allocate a new entry, if no free block is found
    entry = (HeapObj *)malloc(sizeof(HeapObj) + size);
    if (entry == NULL) {
        log_error("VM: failed to allocate memory for block\n");
        return NULL;
    }

    // Add the entry to the list of allocated blocks
    entry->next  = mem->heap;
    mem->heap    = entry;
    entry->value = value_create(TY_UNKNOWN);
    entry->free  = false;

    mem->gc_counter++;
    return entry->value;
}

void heap_free(VMMem *mem, void *pblock) {
    if (pblock == NULL) {
        log_error("VM: failed to free, cannot free NULL block\n");
        return;
    }

#ifdef GC_DEBUG
    log_info("VM: freeing entry at %p\n", pblock);
#endif

    HeapObj *entry = (HeapObj *)pblock;

    // Prevent double-free, only free if we haven't already
    if (!entry->free) {
        if (entry->value) {
            if (entry->value->s_children) {
                free(entry->value->s_children);
            }
            free(entry->value);
        }

        entry->free = true;
        // vm->gc_counter--;
    }
}
