#ifndef CMDLINE_H
#define CMDLINE_H

struct CommandLine {
  const char *input_file;  /* -i */
  const char *output_file; /* -o (opcional) */
  int verbose;             /* -v */
  int show_help;           /* -h */
  int first_arg_index;
};

/*
 * Parseia argc/argv e preenche a struct CommandLine.
 * Retorna:
 *   0  -> sucesso
 *  -1  -> erro de uso (mensagem já impressa)
 */
int parse_command_line(int argc, char *argv[], struct CommandLine *cl);

/* Função para imprimir o help/uso */
static void print_usage(const char *progname);

#endif