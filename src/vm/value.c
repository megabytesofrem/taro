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

bool value_has_gc_roots(Value *val)
{
    return val->type == TY_GROWARRAY || val->type == TY_STRUCTURE || val->s_children > 0;
}