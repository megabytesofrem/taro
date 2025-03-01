#include "lexer.h"
#include "runtime/bytecode.h"
#include "runtime/gc.h"
#include "runtime/value.h"
#include "runtime/vm.h"
#include "util/arena.h"
#include "util/common.h"

#include <stdio.h>
#include <strings.h> // For bzero
#include <unistd.h>

int main() {
    // VM testing ground
    VM vm;
    Arena *arena = arena_create(1024);

    vm_init(&vm, VM_DEFAULT_GC_THRESHOLD);

    struct Bytecode bc = {0};
    read_bytecode_file("/home/rem/Documents/Coding/taro/exe.bc", &bc);
    vm_load(arena, &vm, bc.code, bc.header.code_size);

    while (true) {
        sleep(1);
        vm_cycle(arena, &vm);
    }

    sleep(2);
    vm_cleanup(arena, &vm);

    return 0;
}
