#include "../include/arena.h"
#include <stdlib.h>

static inline size_t align_up(size_t size, size_t alignment) {
    return (size + (alignment - 1)) & ~(alignment - 1);
}

bool arena_init(Arena *arena, size_t capacity) {
    if (!arena) return false;
    
    arena->buffer = malloc(capacity);
    if (!arena->buffer) {
        arena->capacity = 0;
        arena->offset = 0;
        return false;
    }

    arena->capacity = capacity;
    arena->offset = 0;
    
    if (pthread_mutex_init(&arena->lock, NULL) != 0) {
        free(arena->buffer);
        return false;
    }

    return true;
}

void *arena_alloc(Arena *arena, size_t size) {
    if (!arena || !arena->buffer) return NULL;

    size_t aligned_size = align_up(size, DEFAULT_ALIGNMENT);

    pthread_mutex_lock(&arena->lock);
    if (arena->offset + aligned_size > arena->capacity) {
        pthread_mutex_unlock(&arena->lock);
        return NULL;
    }

    void *ptr = &arena->buffer[arena->offset];
    arena->offset += aligned_size;
    pthread_mutex_unlock(&arena->lock);

    return ptr;
}

void arena_reset(Arena *arena) {
    if (arena) {
        pthread_mutex_lock(&arena->lock);
        arena->offset = 0;
        pthread_mutex_unlock(&arena->lock);
    }
}

void arena_free(Arena *arena) {
    if (arena && arena->buffer) {
        pthread_mutex_destroy(&arena->lock);
        free(arena->buffer);
        arena->buffer = NULL;
        arena->capacity = 0;
        arena->offset = 0;
    }
}
