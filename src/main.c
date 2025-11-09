#include <glib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "cmdline.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
  struct CommandLine cl;

  if (parse_command_line(argc, argv, &cl) != 0)
  {
    return 1;
  }

  if (cl.verbose)
  {
    puts("verbose mode ON");
  }

  if (cl.output_file)
  {
    printf("outfile: %s\n", cl.output_file);
  }

  printf("args não-opção:\n");
  extern int optind; /* declarado em unistd.h normalmente, mas só lembrando que
                        existe */
  for (int i = optind; i < argc; ++i)
  {
    printf("  %s\n", argv[i]);
  }

  return 0;
}
