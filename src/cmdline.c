#include "cmdline.h"
#include <getopt.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

static void print_usage(const char *progname)
{
  if (progname == NULL)
  {
    progname = "program";
  }

  fprintf(stderr,
          "Uso: %s -i <input> [-v] [-h]\n"
          "\n"
          "  -i <input>   input file\n"
          "  -v           Verbose Mode\n"
          "  -h           Help\n",
          progname);
}

int parse_command_line(int32_t argc, char *argv[], struct CommandLine *cl)
{
  int32_t opt;

  if (cl == NULL)
  {
    return -1;
  }

  cl->input_file = NULL;
  cl->verbose = 0;
  cl->show_help = 0;
  cl->first_arg_index = 0;

  while ((opt = getopt(argc, argv, "i:o:vh")) != -1)
  {
    switch (opt)
    {
    case 'i':
      cl->input_file = optarg;
      break;

    case 'v':
      cl->verbose = 1;
      break;

    case 'h':
      cl->show_help = 1;
      break;

    default:
      print_usage(argv[0]);
      return -1;
    }
  }
  
  cl->first_arg_index = optind;

  if (cl->show_help)
  {
    return 0;
  }

  if (cl->input_file == NULL)
  {
    fprintf(stderr, "Error: Missing parameter -i <input>\n\n");
    print_usage(argv[0]);
    return -1;
  }

  return 0;
}
