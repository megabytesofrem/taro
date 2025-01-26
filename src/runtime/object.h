#ifndef TARO_OBJECT
#define TARO_OBJECT

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "../util/error.h"

enum TrObjectKind
{
    OBJ_UNKNOWN,
    OBJ_INT,
    OBJ_FLOAT,
    OBJ_STRING,
    OBJ_GROWARRAY,  // Growable array
    OBJ_FIXEDARRAY, // Fixed size array
    OBJ_STRUCTURE,
};

typedef struct Object
{
    bool marked;

    enum TrObjectKind kind;

    union ObjectData {
        char *string_value;
        int int_value;
        float float_value;
    } data;

    // Representing arrays/structures
    struct Object **s_children;
    int s_children_count;    // current number of children
    int s_children_capacity; // maximum number of children
} Object;

Object *object_create(enum TrObjectKind kind);

/**
 * Return whether the object is managed by the GC. Growable arrays and structures
 * require the GC to manage their memory.
 */
bool object_is_gc_managed(Object *obj);

Object *object_create_int(int value);
Object *object_create_float(float value);
Object *object_create_string(char *value);
Object *object_create_array(Object **children, int child_count, int capacity,
                            bool growable);

#endif