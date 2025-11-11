#include "log.h"
#include <stdarg.h>
#include <time.h>
#include <stdint.h>

static log_level_t g_log_level = LOG_LEVEL_INFO;
static FILE *g_log_stream = NULL;

void log_set_level(log_level_t level)
{
    g_log_level = level;
}

void log_set_stream(FILE *stream)
{
    g_log_stream = stream;
}

static const char *level_to_str(log_level_t level)
{
    switch (level) 
    {
        case LOG_LEVEL_TRACE: return "TRACE";
        case LOG_LEVEL_DEBUG: return "DEBUG";
        case LOG_LEVEL_INFO:  return "INFO";
        case LOG_LEVEL_WARN:  return "WARN";
        case LOG_LEVEL_ERROR: return "ERROR";
        default:              return "UNK";
    }
}

void log_log(log_level_t level,
             const char *file,
             int32_t line,
             const char *func,
             const char *fmt, ...)
{
    if (level < g_log_level || level == LOG_LEVEL_NONE) 
    {
        return;
    }

    if (g_log_stream == NULL) 
    {
        g_log_stream = stderr;
    }

    /* timestamp */
    time_t t = time(NULL);
    struct tm tm_info;
    localtime_r(&t, &tm_info);

    char timebuf[20];
    strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S", &tm_info);

    fprintf(g_log_stream, "%s [%s] %s:%d (%s): ",
            timebuf, level_to_str(level), file, line, func);

    va_list args;
    va_start(args, fmt);
    vfprintf(g_log_stream, fmt, args);
    va_end(args);

    fputc('\n', g_log_stream);
    fflush(g_log_stream);
}
