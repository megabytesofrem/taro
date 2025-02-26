VM Architecture
---------------
The virtual machine is a simple stack-machine architecture with both a stack and heap for larger values. The VM is also known as the kernel (not to be confused with operating system kernels).

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
    
    // Statically allocated string table
    char** string_tbl;

    // GC implementation omitted for simplicity
} VM;
```

Instructions
------------
Stack Manipulation:
```
pushi 123:                  Push an int to the stack
pushf 3.14:                 Push a float to the stack
popi:                       Pop an int value from the stack
popf:                       Pop a float value from the stack
pops:                       Pop a string value from the stack
stores <index>,<string>:    Store a string into the string table
loads <index>:              Load a string from the string table onto the stack
```

Comparison:
```
cmp:                        Pop two items off the stack, compare them, set flag
j <label|addr>:             Jump to a label/jump to a absolute/relative address
jeq <label|addr>:           Jump if equal (flag=1)
jne <label|addr>:           Jump if not equal (flag=0)
```

Math:
```
addi/subi/mul/div:          Integer addition/subtraction/multiplication/division
addf/subf:                  Floating point addition/subtraction
```

Functions:
```
call:                       Call a function defined, see below for specification
ret:                        Return from a function

```

Kernel Builtins
------------------
The kernel provides a set of built-in instructions and functions.

- Locals: 

Local variables can be declared at the top of a function using the `local` instruction.
They can be accessed using their name e.g `$local1` and modified using `set`. 

Locals can *only* be used in functions because they store variables within stack frames.

```lua
local $local1: int = 1
set $local1 123
```

- If: 

If `$local1` equals `$local2`, then jump to `.success`. Otherwise, the `ip` will increment past the if statement
Valid operators are: `eq, ne, lt, gt`.

```lua
local $local1: int = 1
local $local2: int = 2
if $local1 eq $local2 .success
j .fail
```

- Functions:

Functions are defined using the `func` keyword. They cannot take any parameters, and must return a value upon exit using the `ret` instruction. They can only be called using `call`.

```lua
func do_nothing()
   local $local1: int = 0
   
   ret
end

call $do_nothing()
```

Bytecode format
---------------
The bytecode format is defined in `src/runtime/bytecode.h`
