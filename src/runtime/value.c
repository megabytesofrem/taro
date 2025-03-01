#include "value.h"

#include "../util/logger.h"

Value *value_create(enum RuntimeValueType type) {
    Value *val = (Value *)malloc(sizeof(Value));
    if (val == NULL) {
        log_error("failed to allocate memory for object\n");
        return NULL;
    }

    val->type   = type;
    val->marked = false;

    return val;
}

bool value_has_child_nodes(Value *val) {
    return val->type == TY_GROWARRAY || val->type == TY_STRUCTURE || val->s_children > 0;
}
