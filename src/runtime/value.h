#ifndef TARO_OBJECT
#define TARO_OBJECT

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "../util/error.h"

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

    union ObjectData {
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
 * Return whether the value is managed by the GC. Growable arrays and structures
 * require the GC to manage their memory.
 */
bool value_is_gc_managed(Value *val);

Value *value_create_int(int value);
Value *value_create_float(float value);
Value *value_create_string(char *value);
Value *value_create_array(Value **children, int child_count, int capacity, bool growable);

#endif