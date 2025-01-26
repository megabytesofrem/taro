VM architecture
---------------

Instruction format
------------------
opcode: the opcode
operand1: an object
operand2: another object

Stack Frame
-----------
ip: pointer to next instruction to execute when returning
base: pointer to base of the current stack frame
locals, locals_count: local variables
return_addr: return address to jump to after function call
params, params_count: function parameters
saved_regs, saved_regs_count: saved registers


VM
---
Single page in the VM: 4KB aligned
Stack: holds stack frames and is 16KB
Heap: resizable, 4KB aligned pages

Opcodes
-------
OP_PUSH: push to stack
OP_POP: pop from stack
OP_ADD: add
OP_SUB: sub
OP_MUL: mul
OP_DIV: div
OP_JUMP: jump unconditional
OP_JUMP_IF: jump if conditional
OP_LOCAL: local variable
OP_PUSH_FRAME: push stack frame containing locals
OP_POP_FRAME: pop stack frame
OP_CALL: call function

GC: our GC is a mark and sweep garbage collector, which marks all objects in the heap,
performs a sweep cycle where we unmark the object and free it
