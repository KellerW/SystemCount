#ifndef CMDLINE_H
#define CMDLINE_H
#include <stdint.h>
struct CommandLine 
{
  const char *input_file;      /* -i */
  int32_t verbose;             /* -v */
  int32_t show_help;           /* -h */
  int32_t first_arg_index;
};

int32_t parse_command_line(int32_t argc, char *argv[], struct CommandLine *cl);

static void print_usage(const char *progname);

#endif