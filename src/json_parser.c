#include "json_parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "yajl/yajl_parse.h"

#include "arena_alloc.h"
#include "model_counter.h"

typedef struct 
{
    int32_t  have_key;
    char last_key[256];
    ModelCounter *counter; 
} ParseCtx;

/* Callbacks YAJL */

static int32_t handle_map_key(void *ctx,
                          const unsigned char *key,
                          size_t len)
{
    ParseCtx *p = (ParseCtx *)ctx;

    if (len >= sizeof(p->last_key)) 
    {
        len = sizeof(p->last_key) - 1U;
    }
    memcpy(p->last_key, key, len);
    p->last_key[len] = '\0';
    p->have_key = 1;

    return YAJL_CB_OK; 
    
}

static int32_t handle_string(void *ctx,
                         const unsigned char *str,
                         size_t len)
{
    ParseCtx *p = (ParseCtx *)ctx;

    if (p->have_key && (strcmp(p->last_key, "model") == 0)) 
    {
       model_counter_add(p->counter, (const char *)str, len);
    }

    p->have_key = 0;
    return YAJL_CB_OK;
}

static int32_t handle_start_map(void *ctx)       
{ 
    (void)ctx; 
    return YAJL_CB_OK; 
}
static int32_t handle_end_map(void *ctx)         
{ 
    (void)ctx; 
    return YAJL_CB_OK; 
}

static int32_t handle_null(void *ctx)           
{ 
    (void)ctx; return YAJL_CB_OK; 
}

static int32_t handle_boolean(void *ctx, int32_t v)  
{ 
    (void)ctx; 
    (void)v; 
    return YAJL_CB_OK; 
}

static int32_t handle_integer(void *ctx, long long val)
{
    (void)ctx;
    (void)val;
    return YAJL_CB_OK;
}
static int32_t handle_double(void *ctx, double val)
{
    (void)ctx;
    (void)val;
    return YAJL_CB_OK;
}
static int32_t handle_number(void *ctx, const char *s, size_t l)
{
    (void)ctx;
    (void)s;
    (void)l;
    return YAJL_CB_OK;
}
static int32_t handle_start_array(void *ctx)     
{ 
    (void)ctx; 
    return YAJL_CB_OK; 
}
static int32_t handle_end_array(void *ctx)      
{ 
    (void)ctx; 
    return YAJL_CB_OK; 
}

static yajl_callbacks g_callbacks = 
{
    handle_null,         /* yajl_null        */
    handle_boolean,      /* yajl_boolean     */
    handle_integer,      /* yajl_integer     */
    handle_double,       /* yajl_double      */
    handle_number,       /* yajl_number      */
    handle_string,       /* yajl_string      */
    handle_start_map,    /* yajl_start_map   */
    handle_map_key,      /* yajl_map_key     */
    handle_end_map,      /* yajl_end_map     */
    handle_start_array,  /* yajl_start_array */
    handle_end_array     /* yajl_end_array   */
};

#define IO_BUF_SIZE  (64U * 1024U)
//#define IO_BUF_SIZE  (1024U * 1024U) 
//#define IO_BUF_SIZE  (256U * 1024U) 
typedef struct 
{
    unsigned char data[IO_BUF_SIZE];
} IOBuffer;

int32_t json_parse_fd(int32_t fd, ModelCounter *counter)
{
    error_t err = ERR_OK;

    IOBuffer  *buf   = NULL;
    Arena     *arena = NULL;
    yajl_handle hand = NULL;

    if ((fd < 0) || (counter == NULL))
    {
        return ERR_INVALID_ARG;
    }

    LOG_DEBUG("Starting JSON parse from fd=%d", fd);

    buf = (IOBuffer *)malloc(sizeof(IOBuffer));
    if (buf == NULL) 
    {
        perror("malloc IOBuffer");
        err = ERR_NO_MEM;
        goto cleanup;
    }

    /* Create arena for YAJL */
    arena = arena_create();
    if (arena == NULL) 
    {
        fprintf(stderr, "Failed to create arena\n");
        err = ERR_NO_MEM;
        goto cleanup;
    }

    yajl_alloc_funcs allocs;
    arena_init_yajl_alloc_funcs(arena, &allocs);

    /* Parser context */
    ParseCtx ctx;
    memset(&ctx, 0, sizeof(ctx));
    ctx.counter = counter;

    hand = yajl_alloc(&g_callbacks, &allocs, (void *)&ctx);
    if (hand == NULL) 
    {
        fprintf(stderr, "Failed to create YAJL handle\n");
        err = ERR_NO_MEM;
        goto cleanup;
    }

    yajl_status stat;
    ssize_t rd;

    for (;;) 
    {
        rd = read(fd, buf->data, IO_BUF_SIZE);
        if (rd < 0) 
        {
            perror("read");
            err = ERR_IO;
            goto cleanup;
        }
        if (rd == 0) {
            /* EOF */
            break;
        }

        stat = yajl_parse(hand, buf->data, (size_t)rd);
        if (stat != yajl_status_ok)
        {
            unsigned char *yerr = yajl_get_error(hand, 1,
                                                 buf->data, (size_t)rd);
            fprintf(stderr, "YAJL error: %s\n", yerr);
            yajl_free_error(hand, yerr);
            err = ERR_IO;
            goto cleanup;
        }
    }

    stat = yajl_complete_parse(hand);
    if (stat != yajl_status_ok) 
    {
        unsigned char *yerr = yajl_get_error(hand, 0, NULL, 0);
        fprintf(stderr, "YAJL finalization error: %s\n", yerr);
        yajl_free_error(hand, yerr);
        err = ERR_IO;
        goto cleanup;
    }

    LOG_DEBUG("Finished JSON parse from fd=%d", fd);

cleanup:
    if (hand != NULL) 
    {
        yajl_free(hand);
    }
    if (arena != NULL) 
    {
        arena_destroy(arena);
    }
    if (buf != NULL) 
    {
        free(buf);
    }

    return err;
}

int32_t json_parse_file(const char *filename, ModelCounter *counter)
{
    if ((filename == NULL) || (counter == NULL)) 
    {
        return -1;
    }

    int32_t fd = open(filename, O_RDONLY);
    if (fd < 0) 
    {
        perror("open");
        return -1;
    }

    int32_t rc = json_parse_fd(fd, counter);
    close(fd);
    return rc;
}
