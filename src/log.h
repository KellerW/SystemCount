#ifndef LOG_H
#define LOG_H

#include <stdio.h>

/* Log levels */
typedef enum 
{
    LOG_LEVEL_TRACE = 0,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_NONE
} log_level_t;

void log_set_level(log_level_t level);
void log_set_stream(FILE *stream);

void log_log(log_level_t level,
             const char *file,
             int line,
             const char *func,
             const char *fmt, ...);

/* Convenience macros */
#define LOG_TRACE(fmt, ...) log_log(LOG_LEVEL_TRACE, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#define LOG_DEBUG(fmt, ...) log_log(LOG_LEVEL_DEBUG, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt,  ...) log_log(LOG_LEVEL_INFO,  __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt,  ...) log_log(LOG_LEVEL_WARN,  __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) log_log(LOG_LEVEL_ERROR, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)

#ifdef NDEBUG
# undef LOG_TRACE
# undef LOG_DEBUG
# define LOG_TRACE(...) ((void)0)
# define LOG_DEBUG(...) ((void)0)
#endif

#endif
