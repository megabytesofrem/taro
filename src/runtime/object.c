#include "object.h"

Object *object_create(enum TrObjectKind kind)
{
    Object *obj = (Object *)malloc(sizeof(Object));
    if (obj == NULL) {
        log_error("failed to allocate memory for object\n");
        return NULL;
    }

    obj->kind = kind;
    obj->marked = false;

    return obj;
}

bool object_is_gc_managed(Object *obj)
{
    return obj->kind == OBJ_GROWARRAY || obj->kind == OBJ_STRUCTURE ||
           obj->s_children > 0;
}

Object *object_create_int(int value)
{
    Object *obj = object_create(OBJ_INT);
    obj->data.int_value = value;

    return obj;
}

Object *object_create_float(float value)
{
    Object *obj = object_create(OBJ_FLOAT);
    obj->data.float_value = value;

    return obj;
}

Object *object_create_string(char *value)
{
    Object *obj = object_create(OBJ_STRING);
    obj->data.string_value = strdup(value);

    return obj;
}

Object *object_create_array(Object **children, int child_count, int capacity,
                            bool growable)
{
    Object *obj = object_create(OBJ_FIXEDARRAY);

    if (growable) {
        obj->kind = OBJ_GROWARRAY;
    }

    obj->s_children = children;
    obj->s_children_count = child_count;
    obj->s_children_capacity = capacity;

    return obj;
}