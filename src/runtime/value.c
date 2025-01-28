#include "value.h"

Value *value_create(enum TrValueType type)
{
    Value *val = (Value *)malloc(sizeof(Value));
    if (val == NULL) {
        log_error("failed to allocate memory for object\n");
        return NULL;
    }

    val->type = type;
    val->marked = false;

    return val;
}

bool value_is_gc_managed(Value *val)
{
    return val->type == TY_GROWARRAY || val->type == TY_STRUCTURE || val->s_children > 0;
}

Value *value_create_int(int value)
{
    Value *val = value_create(TY_INT);
    val->data.int_value = value;

    return val;
}

Value *value_create_float(float value)
{
    Value *val = value_create(TY_FLOAT);
    val->data.float_value = value;

    return val;
}

Value *value_create_string(char *value)
{
    Value *val = value_create(TY_STRING);
    val->data.string_value = strdup(value);

    return val;
}

Value *value_create_array(Value **children, int child_count, int capacity, bool growable)
{
    Value *val = value_create(TY_FIXEDARRAY);

    if (growable) {
        val->type = TY_GROWARRAY;
    }

    val->s_children = children;
    val->s_children_count = child_count;
    val->s_children_capacity = capacity;

    return val;
}