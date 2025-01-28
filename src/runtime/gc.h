#ifndef TARO_GC_IMPL
#define TARO_GC_IMPL

#define GC_DEBUG 1

#include "value.h"
#include "vm.h"

void gc_create_thread(VM *vm);

void gc_mark(Value *val);
void gc_mark_all(VM *vm);
void gc_perform_sweep(VM *vm);
void gc_collect(VM *vm);

#endif