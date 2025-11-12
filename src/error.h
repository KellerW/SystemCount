#ifndef ERROR_H
#define ERROR_H

typedef enum 
{
    ERR_OK = 0,
    ERR_INVALID_ARG,
    ERR_NO_MEM,
    ERR_IO,
    ERR_TIMEOUT,
    ERR_NOT_FOUND,
    ERR_INTERNAL,
} error_t;

/* Optional: convert error to string for logging/debug */
const char *error_to_string(error_t err);

#endif