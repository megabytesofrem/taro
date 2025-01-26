#ifndef TARO_GC_IMPL
#define TARO_GC_IMPL

#include "object.h"
#include "vm.h"

// FIXME: Broken GC implementation
void gc_mark(Object *obj);
void gc_mark_all(VM *vm);
void gc_perform_sweep(VM *vm);
void gc_collect(VM *vm);

#endif