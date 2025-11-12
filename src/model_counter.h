#ifndef MODEL_COUNTER_H
#define MODEL_COUNTER_H

#include <stddef.h>

typedef struct ModelCounter ModelCounter;

ModelCounter *model_counter_create(void);

void model_counter_add(ModelCounter *mc,
                       const char *str,
                       size_t len);

void model_counter_print(const ModelCounter *mc);

void model_counter_destroy(ModelCounter *mc);

#endif
