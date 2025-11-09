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
          "Uso: %s -i <input> [-o <output>] [-v] [-h]\n"
          "\n"
          "  -i <input>   ficheiro de entrada (obrigatório)\n"
          "  -o <output>  ficheiro de saída (opcional)\n"
          "  -v           modo verboso\n"
          "  -h           mostra esta ajuda\n",
          progname);
}

int parse_command_line(int argc, char *argv[], struct CommandLine *cl)
{
  int32_t opt;

  if (cl == NULL)
  {
    return -1;
  }

  /* Inicializa a struct */
  cl->input_file = NULL;
  cl->output_file = NULL;
  cl->verbose = 0;
  cl->show_help = 0;
  cl->first_arg_index = 0;

  /* Loop de parsing com getopt.
     String "i:o:vh" significa:
       i:  -> -i precisa de argumento
       o:  -> -o precisa de argumento
       v   -> flag sem argumento
       h   -> flag sem argumento
  */
  while ((opt = getopt(argc, argv, "i:o:vh")) != -1)
  {
    switch (opt)
    {
    case 'i':
      cl->input_file = optarg;
      break;

    case 'o':
      cl->output_file = optarg;
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

  /* guarda onde começam os argumentos não-opção */
  cl->first_arg_index = optind;

  /* Se o user pediu help, não obriga -i */
  if (cl->show_help)
  {
    return 0;
  }

  /* Validação básica: -i é obrigatório */
  if (cl->input_file == NULL)
  {
    fprintf(stderr, "Erro: falta o parâmetro -i <input>\n\n");
    print_usage(argv[0]);
    return -1;
  }

  return 0;
}
