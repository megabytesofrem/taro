VM Architecture
---------------
```c
typedef struct VM
{
    size_t ip;          // instruction pointer
    int flag;           // set after comparison

    VMInstruction *code;
    int code_size;

    // Memory
    // Stack and a heap
    VMMem mem;

    Hashtable *string_tbl;

    // GC implementation omitted
} VM;
```

Instructions
------------
```
Stack Manipulation:

pushi 123:                  Push an int to the stack
pushf 3.14:                 Push a float to the stack
popi:                       Pop an int value from the stack
popf:                       Pop a float value from the stack
pops:                       Pop a string value from the stack
stores msg, "hello":        Store a string to the string table
loads msg:                  Load a string from the string table onto the stack

Comparison:
cmp:                        Pop two items off the stack, compare them, set flag
j @label/j +<0x01>:         Jump to a label/jump to a absolute/relative address
jeq  ..           :         Jump if equal (flag=1)
jne  ..           :         Jump if not equal (flag=0)

Math:
addi/subi/mul/div:          Integer addition/subtraction/multiplication/division
addf/subf:                  Floating point addition/subtraction

Functions:
call $foo:                  Call a function defined, pushes stack frame - function must end with `ret`
ret:                        Return from a function

```

Example IL (subject to change)
----------
```x86asm
func add:
    pushi $0
    pushi $1
    addi
    ret
end

# Main entrypoint
.main entrypoint:
    # call a function 'add'
    call $add(1, 2)
    popi
    ret
end
```