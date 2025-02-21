#ifndef TARO_RUNTIME_FRAME_H
#define TARO_RUNTIME_FRAME_H

#include "value.h"

/**
 * Stack frame
 */
typedef struct Frame
{
    int pc;
    Value locals[16];
    int locals_count;

    // Parent frame
    struct Frame *parent;
} Frame;

#endif
