#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#define YAJL_CB_OK    1
#define YAJL_CB_ERROR 0

#include <stddef.h>
#include <stdint.h>
#include "error.h"
#include "log.h"

#include "model_counter.h"

int32_t json_parse_fd(int32_t fd, ModelCounter *counter);
int32_t json_parse_file(const char *filename, ModelCounter *counter);

#endif
