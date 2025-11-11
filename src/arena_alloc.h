#ifndef ARENA_ALLOC_H
#define ARENA_ALLOC_H

#include <stddef.h>
#include "yajl/yajl_parse.h"
#include "yajl/yajl_tree.h"

typedef struct Arena Arena;

Arena *arena_create(void);

void arena_destroy(Arena *arena);

void *arena_malloc_impl(Arena *arena, size_t sz);
void *arena_realloc_impl(Arena *arena, void *ptr, size_t sz);
void  arena_free_impl(Arena *arena, void *ptr);

void arena_init_yajl_alloc_funcs(Arena *arena, yajl_alloc_funcs *funcs);

#endif 
