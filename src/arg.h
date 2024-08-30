#ifndef ARG_H
#define ARG_H

#include <ei.h>

typedef struct {
  char nodename[MAXNODELEN + 1];
  char cookie[MAXATOMLEN + 1];
} arg_t;

void args_parse(arg_t *args, int argc, char **argv);

#endif /* ARG_H */
