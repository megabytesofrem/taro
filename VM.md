VM Architecture
---------------

Stack Frame
-----------
`ip`: pointer to next instruction to execute when returning
`base`: pointer to base of the current stack frame
`locals`, `locals_count`: local variables
`return_addr`: return address to jump to after function call
`params`, `params_count`: function parameters
`saved_regs`, `saved_regs_count`: saved registers


VM
---
Stack: holds stack frames and is a fixed size (`VM_STACK_MAX_SIZE`)
Heap: resizable, non-contiguous 

Opcodes
-------
`OP_PUSH`: push to stack
`OP_POP`: pop from stack
`OP_ADD`: add
`OP_SUB`: sub
`OP_MUL`: mul
`OP_DIV`: div
`OP_JUMP`: jump unconditional
`OP_JUMP_IF`: jump if conditional
`OP_ALLOC`: allocate on the heap
`OP_PUSH_FRAME`: push a stack frame
`OP_LOCAL_SET`: set a local value
`OP_LOCAL_GET`: retrieve a local value
`OP_CALL`: call function
