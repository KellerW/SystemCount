#include "arena_alloc.h"

#include <stdlib.h>
#include <string.h>

#define ARENA_BLOCK_SIZE  (64U * 1024U)

typedef struct ArenaBlock 
{
    struct ArenaBlock *next;
    size_t size;
    size_t used;
    unsigned char data[];  /* flexible array */
} ArenaBlock;

typedef struct ArenaAllocHeader 
{
    size_t size;
} ArenaAllocHeader;

struct Arena 
{
    ArenaBlock *head;
};

/* Cria novo bloco com tamanho mínimo "min_size" */
static ArenaBlock *arena_new_block(size_t min_size)
{
    size_t block_size = ARENA_BLOCK_SIZE;
    if (min_size + sizeof(ArenaAllocHeader) > block_size) 
    {
        block_size = min_size + sizeof(ArenaAllocHeader);
    }

    ArenaBlock *blk = (ArenaBlock *)malloc(sizeof(ArenaBlock) + block_size);
    if (blk == NULL) 
    {
        return NULL;
    }

    blk->next = NULL;
    blk->size = block_size;
    blk->used = 0U;
    return blk;
}

void *arena_malloc_impl(Arena *arena, size_t sz)
{
    if ((arena == NULL) || (sz == 0U)) 
    {
        return NULL;
    }

    if (arena->head == NULL) 
    {
        arena->head = arena_new_block(sz);
        if (arena->head == NULL) 
        {
            return NULL;
        }
    }

    ArenaBlock *blk = arena->head;
    size_t need = sz + sizeof(ArenaAllocHeader);

    if (blk->used + need > blk->size) 
    {
        ArenaBlock *newblk = arena_new_block(sz);
        if (newblk == NULL) 
        {
            return NULL;
        }
        newblk->next = arena->head;
        arena->head  = newblk;
        blk = newblk;
    }

    unsigned char *ptr = &blk->data[blk->used];
    ArenaAllocHeader *hdr = (ArenaAllocHeader *)ptr;
    hdr->size = sz;

    blk->used += need;

    return (void *)(hdr + 1);
}

void *arena_realloc_impl(Arena *arena, void *ptr, size_t sz)
{
    if (ptr == NULL) 
    {
        return arena_malloc_impl(arena, sz);
    }
    if (sz == 0U) 
    {
        return NULL;
    }

    ArenaAllocHeader *hdr = ((ArenaAllocHeader *)ptr) - 1;
    size_t old_size = hdr->size;

    if (sz <= old_size) 
    {
        hdr->size = sz;
        return ptr;
    }

    void *newptr = arena_malloc_impl(arena, sz);
    if (newptr == NULL) 
    {
        return NULL;
    }
    memcpy(newptr, ptr, old_size);
    return newptr;
}

void arena_free_impl(Arena *arena, void *ptr)
{
    (void)arena;
    (void)ptr;
}

Arena *arena_create(void)
{
    Arena *arena = (Arena *)malloc(sizeof(Arena));
    if (arena == NULL) 
    {
        return NULL;
    }
    arena->head = NULL;
    return arena;
}

void arena_destroy(Arena *arena)
{
    if (arena == NULL) 
    {
        return;
    }

    ArenaBlock *blk = arena->head;
    while (blk != NULL) 
    {
        ArenaBlock *next = blk->next;
        free(blk);
        blk = next;
    }
    arena->head = NULL;
    free(arena);
}

static void *arena_yajl_malloc(void *ctx, size_t sz)
{
    return arena_malloc_impl((Arena *)ctx, sz);
}

static void *arena_yajl_realloc(void *ctx, void *ptr, size_t sz)
{
    return arena_realloc_impl((Arena *)ctx, ptr, sz);
}

static void arena_yajl_free(void *ctx, void *ptr)
{
    arena_free_impl((Arena *)ctx, ptr);
}

void arena_init_yajl_alloc_funcs(Arena *arena, yajl_alloc_funcs *funcs)
{
    if ((arena == NULL) || (funcs == NULL)) 
    {
        return;
    }

    funcs->malloc  = arena_yajl_malloc;
    funcs->realloc = arena_yajl_realloc;
    funcs->free    = arena_yajl_free;
    funcs->ctx     = (void *)arena;
}
