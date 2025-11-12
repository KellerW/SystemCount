#include "error.h"

const char *error_to_string(error_t err)
{
    switch (err) 
    {
    case ERR_OK:          return "OK";
    case ERR_INVALID_ARG: return "Invalid argument";
    case ERR_NO_MEM:      return "Out of memory";
    case ERR_IO:          return "I/O error";
    case ERR_TIMEOUT:     return "Timeout";
    case ERR_NOT_FOUND:   return "Not found";
    case ERR_INTERNAL:    return "Internal error";
    default:              return "Unknown error";
    }
}