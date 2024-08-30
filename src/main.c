#include <unistd.h>

#include "arg.h"
#include "bacnet/datalink/dlenv.h"
#include "ei_client.h"
#include "ei_heartbeat.h"
#include "ei_log.h"

static arg_t args;

int main(int argc, char **argv)
{
  dlenv_init();

  args_parse(&args, argc, argv);

  if (!ei_client_config(args.nodename, args.cookie)) {
    LOG_ERR("Failed to start ei client");
    return -1;
  }

  if (!ei_heartbeat_start()) {
    LOG_ERR("Heartbeat failed to start");
    return -1;
  }

  while (1) {
    pause();
  }

  return 0;
}
