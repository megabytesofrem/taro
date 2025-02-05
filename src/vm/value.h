#ifndef TARO_VALUE
#define TARO_VALUE

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "../util/error.h"

/* Macros to define values */

#define FIXED_ARRAY false   /* The array is a fixed size */
#define GROWABLE_ARRAY true /* The array is growable */

#define INT(_val) ((Value){.type = TY_INT, .data.int_value = _val})
#define FLOAT(_val) ((Value){.type = TY_FLOAT, .data.float_value = _val})
#define STRING(_val) ((Value){.type = TY_STRING, .data.string_value = _val})

#define ARRAY(_val, _count, _capacity, _growable)                                        \
    ((Value){.type = _growable ? TY_GROWARRAY : TY_FIXEDARRAY,                           \
             .s_children = _val,                                                         \
             .s_children_count = _count,                                                 \
             .s_children_capacity = _capacity})

/**
 * Value type enumeration
 */
enum TrValueType
{
    TY_UNKNOWN,
    TY_INT,
    TY_FLOAT,
    TY_STRING,
    TY_GROWARRAY,  // Growable array
    TY_FIXEDARRAY, // Fixed size array
    TY_STRUCTURE,
};

typedef struct Value
{
    bool marked;
    enum TrValueType type;

    union {
        char *string_value;
        int int_value;
        float float_value;
    } data;

    // Representing arrays/structures
    struct Value **s_children;
    int s_children_count;    // current number of children
    int s_children_capacity; // maximum number of children
} Value;

Value *value_create(enum TrValueType type);

/**
 * Return whether the value has children that are GC roots and should be managed
 * themselves.
 *
 * TODO: Rename this to be clearer
 */
bool value_has_gc_roots(Value *val);

#endif