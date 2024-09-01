#include <unistd.h>

#include "arg.h"
#include "bacnet/datalink/dlenv.h"
#include "ei_client.h"
#include "ei_log.h"

#define STRINGIFY(x) #x
#define TOSTR(x) STRINGIFY(x)
#define ERL_TUPLE TOSTR(ERL_SMALL_TUPLE_EXT) TOSTR(ERL_SMALL_TUPLE_EXT)

static arg_t args;
static void handle_call(char *buf, int *index, ei_x_buff *reply);

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
    ei_term t = { 0 };
    char msg_type[MAXATOMLEN] = { 0 };

    char *buf = msg.buff;
    bool bad_msg = (false
          || ei_decode_version(buf, &index, &version)
          || ei_decode_tuple_header(buf, &index, &t.size)
          || (t.size < 2)
          || ei_decode_atom(buf, &index, msg_type));

    if (bad_msg) {
      goto cleanup;
    }

    if (strcmp(msg_type, "$gen_call") == 0) {
      erlang_pid from_pid;
      erlang_ref from_ref;

      // request {:"$gen_call", {PID, [:alias | REF]}, TUPLE}}
      bool bad_msg = (false
            || ei_decode_tuple_header(buf, &index, &t.size)
            || (t.size != 2)
            || ei_decode_pid(buf, &index, &from_pid)
            || ei_decode_list_header(buf, &index, &t.size)
            || (t.size != 1)
            || ei_decode_atom(buf, &index, t.value.atom_name)
            || strcmp(t.value.atom_name, "alias")
            || ei_decode_ref(buf, &index, &from_ref)
            || ei_get_type(buf, &index, (int *)&t.ei_type, &t.size)
            || memchr(ERL_TUPLE, t.ei_type, sizeof(ERL_TUPLE)) == NULL);

      if (bad_msg) {
        LOG_ERR("Failed decoding message");
        goto cleanup;
      }

      // reply {[:alias | REF], REPLY}
      ei_x_buff reply;
      ei_x_new_with_version(&reply);
      ei_x_encode_tuple_header(&reply, 2);
      ei_x_encode_list_header(&reply, 1);
      ei_x_encode_atom(&reply, "alias");
      ei_x_encode_ref(&reply, &from_ref);

      handle_call(buf, &index, &reply);

      if (!ei_client_send_to(&from_pid, &reply)) {
        LOG_ERR("Unable to send reply");
      }

      ei_x_free(&reply);
    } else {
      LOG_WRN("bacnetd: unknown message type %s", msg_type);
    }

  cleanup:
    ei_x_free(&msg);
  }

  return 0;
}

static void add_device(char *buf, int *index, ei_x_buff *reply)
{
  ei_x_encode_tuple_header(reply, 2);
  ei_x_encode_atom(reply, "error");
  ei_x_encode_atom(reply, "unimplemented");
}

static void handle_call(char *buf, int *index, ei_x_buff *reply)
{
  int size = 0;
  char call_type[MAXATOMLEN] = { 0 };

  bool bad_msg = (false
        || ei_decode_tuple_header(buf, index, &size)
        || ei_decode_atom(buf, index, call_type));

  if (bad_msg) {
    ei_x_encode_tuple_header(reply, 2);
    ei_x_encode_atom(reply, "error");
    ei_x_encode_atom(reply, "bad_request");
    return;
  }

  if (strcmp(call_type, "add_device") == 0) {
    add_device(buf, index, reply);
  }
  else {
    ei_x_encode_tuple_header(reply, 2);
    ei_x_encode_atom(reply, "error");
    ei_x_encode_atom(reply, "unimplemented");
  }
}
