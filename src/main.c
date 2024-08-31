#include <unistd.h>

#include "arg.h"
#include "bacnet/datalink/dlenv.h"
#include "ei_client.h"
#include "ei_log.h"

static arg_t args;
static void unimplemented(const char *msg_type);

int main(int argc, char **argv)
{
  dlenv_init();
  args_parse(&args, argc, argv);

  if (!ei_client_config(args.nodename, args.cookie)) {
    LOG_ERR("bacnetd: unable to establish connection");
    return -1;
  }

  LOG_DBG("bacnetd: process started");

  while (1) {
    erlang_msg meta;
    ei_x_buff msg;
    ei_x_new(&msg);

    if (!ei_client_recv(&meta, &msg)) {
      goto cleanup;
    }

    int index = 0;
    int version = 0;
    int arity = 0;
    char msg_type[MAXATOMLEN] = { 0 };

    char *buf = msg.buff;
    bool bad_msg = (false
          || ei_decode_version(buf, &index, &version)
          || ei_decode_tuple_header(buf, &index, &arity)
          || (arity < 2)
          || ei_decode_atom(buf, &index, msg_type));

    if (bad_msg) {
      goto cleanup;
    }

    if (strcmp(msg_type, "$gen_call") == 0) {
      unimplemented("$gen_call");
    }

  cleanup:
    ei_x_free(&msg);
  }

  return 0;
}

static void unimplemented(const char *msg_type)
{
  LOG_WRN("bacnetd: %s unimplemented", msg_type);
}
