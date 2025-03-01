#ifndef ARENA_ALLOC_H
#define ARENA_ALLOC_H

#if !defined(ARENA_MALLOC) || !defined(ARENA_FREE)
#define ARENA_MALLOC malloc
#define ARENA_FREE free
#endif

#include <stddef.h>
#include <stdlib.h>

// Based on this tutorial:
// https://dev.to/ccgargantua/working-smarter-instead-of-harder-in-c-43o3

typedef struct {
    void *region;
    size_t index, size;
} Arena;

static Arena *arena_create(size_t size) __attribute__((unused));
static void *arena_alloc(Arena *arena, size_t size) __attribute__((unused));
static void arena_clear(Arena *arena) __attribute__((unused));
static void arena_destroy(Arena *arena) __attribute__((unused));

static Arena *arena_create(size_t size) {
    Arena *arena = (Arena *)ARENA_MALLOC(sizeof(Arena));
    if (arena == NULL) {
        return NULL;
    }

    arena->region = ARENA_MALLOC(size);
    if (arena->region == NULL) {
        return NULL;
    }

    arena->index = 0;
    arena->size  = size;
    return arena;
}

static void *arena_alloc(Arena *arena, size_t size) {
    if (arena == NULL || arena->region == NULL) {
        return NULL;
    }

    if ((arena->size - arena->index) < size) {
        // Check if we have enough room for the allocation
        return NULL;
    }

    arena->index += size;

    // Return the region pointer but offset by the index in the arena
    return (void *)(arena->region + (arena->index - size));
}

static void arena_clear(Arena *arena) {
    if (arena == NULL) {
        return;
    }

    // Reset the index to 0, so we can reuse memory without free'ing
    arena->index = 0;
}

static void arena_destroy(Arena *arena) {
    if (arena == NULL) {
        return;
    }

    // Free all allocations in the region before free'ing the arena itself
    if (arena->region != NULL) {
        ARENA_FREE(arena->region);
    }

    ARENA_FREE(arena);
}

#endif
