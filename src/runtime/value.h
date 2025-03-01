#ifndef TARO_RUNTIME_VALUE_H
#define TARO_RUNTIME_VALUE_H

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

/* Macros to define values */

#define FIXED_ARRAY false   /* The array is a fixed size */
#define GROWABLE_ARRAY true /* The array is growable */

#define new_int(_val) ((Value){.type = TY_INT, .data.int_value = _val})
#define new_float(_val) ((Value){.type = TY_FLOAT, .data.float_value = _val})
#define new_string(_val) ((Value){.type = TY_STRING, .data.string_value = _val})

#define new_array(_val, _count, _capacity, _growable)                                    \
    ((Value){.type                = _growable ? TY_GROWARRAY : TY_FIXEDARRAY,            \
             .s_children          = _val,                                                \
             .s_children_count    = _count,                                              \
             .s_children_capacity = _capacity})

#define as_int(_val) _val.int_value
#define as_float(_val) _val.float_value
#define as_string(_val) _val.string_value

/**
 * Value type enumeration
 */
enum RuntimeValueType {
    TY_UNKNOWN,
    TY_INT,
    TY_FLOAT,
    TY_STRING,
    TY_GROWARRAY,  // Growable array
    TY_FIXEDARRAY, // Fixed size array
    TY_STRUCTURE,
};

typedef struct RuntimeValue {
    bool marked;
    enum RuntimeValueType type;

    union {
        char *string_value;
        int int_value;
        float float_value;
    } data;

    // Representing arrays/structures
    struct RuntimeValue **s_children;
    int s_children_count;    // current number of children
    int s_children_capacity; // maximum number of children
} Value;

Value *value_create(enum RuntimeValueType type);

/**
 * Return whether the value has child nodes. This is used to determine if we
 * need to traverse the children of a value during garbage collection.
 */
bool value_has_child_nodes(Value *val);

#endif
