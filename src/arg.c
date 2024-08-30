#include <getopt.h>
#include <string.h>

#include "arg.h"

static const struct option options[] = {
  {"nodename", required_argument, 0, 'n'},
  {"cookie", required_argument, 0, 'c'},
  {0, 0, 0, 0},
};

void args_parse(arg_t *args, int argc, char **argv)
{
  int opt;
  int option_index = 0;

  while ((opt = getopt_long(argc, argv, "n:c:", options, &option_index))
         != -1) {
    switch (opt) {
    case 'n':
      strncpy(args->nodename, optarg, sizeof(args->nodename));
      break;
    case 'c':
      strncpy(args->cookie, optarg, sizeof(args->cookie));
      break;
    default:
      break;
    }
  }
}
