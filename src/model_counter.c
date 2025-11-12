#include "model_counter.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>  
#include <glib.h>

typedef struct 
{
    const char *ptr;
    size_t      len;
} ModelKey;

typedef struct 
{
    ModelKey      key;
    unsigned long count;
} ModelEntry;

struct ModelCounter 
{
    GHashTable *table;
};

/* -------- Funções de hash / equal para GHashTable -------- */

static guint model_key_hash(gconstpointer v)
{
    const ModelKey *k = v;
    const unsigned char *p = (const unsigned char *)k->ptr;
    size_t i;
    guint h = 5381U;  /* djb2 básico */

    for (i = 0U; i < k->len; ++i) 
    {
        h = (h * 33U) + (guint)p[i];
    }
    return h;
}

static gboolean model_key_equal(gconstpointer a, gconstpointer b)
{
    const ModelKey *ka = a;
    const ModelKey *kb = b;

    if (ka->len != kb->len) 
    {
        return FALSE;
    }
    if (ka->ptr == kb->ptr) 
    {
        return TRUE;
    }
    return (memcmp(ka->ptr, kb->ptr, ka->len) == 0);
}

static void model_key_destroy(gpointer key)
{
    ModelKey *k = (ModelKey *)key;

    ModelEntry *entry =
        (ModelEntry *)((char *)k - offsetof(ModelEntry, key));

    g_free((gpointer)entry->key.ptr);
    g_free(entry);
}

/* -------- API Public -------- */

ModelCounter * model_counter_create(void)
{
    ModelCounter *mc = g_new0(ModelCounter, 1);
    if (mc == NULL) 
    {
        return NULL;
    }

    mc->table = g_hash_table_new_full(
        model_key_hash,
        model_key_equal,
        model_key_destroy, 
        NULL               
    );

    if (mc->table == NULL) 
    {
        g_free(mc);
        return NULL;
    }

    return mc;
}

void model_counter_add(ModelCounter *mc, const char *str, size_t len)
{
    if ((mc == NULL) || (str == NULL) || (len == 0U)) 
    {
        return;
    }

    ModelKey lookup;
    lookup.ptr = str;
    lookup.len = len;

    ModelEntry *entry = g_hash_table_lookup(mc->table, &lookup);

    if (entry == NULL) 
    {
        entry = g_new0(ModelEntry, 1);
        if (entry == NULL) 
        {
            return;
        }

        char *copy = g_strndup(str, len);
        if (copy == NULL) 
        {
            g_free(entry);
            return;
        }

        entry->key.ptr = copy;
        entry->key.len = len;
        entry->count   = 1U;
        
        g_hash_table_insert(mc->table, &entry->key, entry);
    } 
    else 
    {
        entry->count++;
    }
}

static void print_entry(gpointer key, gpointer value, gpointer user_data)
{
    (void)key;
    (void)user_data;

    ModelEntry *entry = (ModelEntry *)value;

    printf("%10lu  %.*s\n",
           entry->count,
           (int)entry->key.len,
           entry->key.ptr);
}

void model_counter_print(const ModelCounter *mc)
{
    if ((mc == NULL) || (mc->table == NULL)) 
    {
        return;
    }

    g_hash_table_foreach(mc->table, print_entry, NULL);
}

void model_counter_destroy(ModelCounter *mc)
{
    if (mc == NULL) 
    {
        return;
    }

    if (mc->table != NULL) 
    {
        g_hash_table_destroy(mc->table);
        mc->table = NULL;
    }

    g_free(mc);
}
