/**
 * Common utility macros
 */

#ifndef UTIL_MACROS_H
#define UTIL_MACROS_H

#define packed_t __attribute__((__packed__))
#define UNUSED(x) (void)
#define DEPRECATED(x) __attribute__((deprecated(x)))

#define foreach(iter, array, len)                                                        \
    for (size_t _i = 0; _i < (len) && ((iter) = (array)[_i], 1); _i++)
#define foreach_n(iter, array, len) for (size_t iter = 0; iter < len; iter++)

#endif
