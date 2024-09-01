#include <unistd.h>

#include "arg.h"
#include "bacnet/datalink/dlenv.h"
#include "ei_client.h"
#include "ei_log.h"

#define STRINGIFY(x) #x
#define TOSTR(x) STRINGIFY(x)
#define ERL_TUPLE TOSTR(ERL_SMALL_TUPLE_EXT) TOSTR(ERL_SMALL_TUPLE_EXT)

static arg_t args;
static void handle_call(char *buffer, int *index, ei_x_buff *reply);

int main(int argc, char **argv)
{
  dlenv_init();
  args_parse(&args, argc, argv);

  if (!ei_client_config(args.nodename, args.cookie)) {
    LOG_ERROR("bacnetd: unable to establish connection");
    return -1;
  }

  LOG_DEBUG("bacnetd: process started");

  while (1) {
    erlang_msg meta;
    ei_x_buff message;
    ei_x_new(&message);

    if (!ei_client_recv(&meta, &message))
      goto cleanup;

    int index = 0;
    int version = 0;
    ei_term term = { 0 };
    char message_type[MAXATOMLEN] = { 0 };

    bool is_bad_message =
         ei_decode_version(message.buff, &index, &version)
      || ei_decode_tuple_header(message.buff, &index, &term.size)
      || (term.size < 2)
      || ei_decode_atom(message.buff, &index, message_type);

    if (is_bad_message)
      goto cleanup;

    if (strcmp(message_type, "$gen_call") == 0) {
      erlang_pid from_pid;
      erlang_ref from_ref;

      // request {:"$gen_call", {PID, [:alias | REF]}, TUPLE}}
      bool is_bad_message =
           ei_decode_tuple_header(message.buff, &index, &term.size)
        || (term.size != 2)
        || ei_decode_pid(message.buff, &index, &from_pid)
        || ei_decode_list_header(message.buff, &index, &term.size)
        || (term.size != 1)
        || ei_decode_atom(message.buff, &index, term.value.atom_name)
        || strcmp(term.value.atom_name, "alias")
        || ei_decode_ref(message.buff, &index, &from_ref)
        || ei_get_type(message.buff, &index, (int *)&term.ei_type, &term.size)
        || memchr(ERL_TUPLE, term.ei_type, sizeof(ERL_TUPLE)) == NULL;

      if (is_bad_message) {
        LOG_ERROR("Failed decoding message");
        goto cleanup;
      }

      // reply {[:alias | REF], REPLY}
      ei_x_buff reply;
      ei_x_new_with_version(&reply);
      ei_x_encode_tuple_header(&reply, 2);
      ei_x_encode_list_header(&reply, 1);
      ei_x_encode_atom(&reply, "alias");
      ei_x_encode_ref(&reply, &from_ref);

      handle_call(message.buff, &index, &reply);

      if (!ei_client_send_to(&from_pid, &reply))
        LOG_ERROR("Unable to send reply");

      ei_x_free(&reply);
    }
    else {
      LOG_WARNING("bacnetd: unknown message type %s", message_type);
    }

  cleanup:
    ei_x_free(&message);
  }

  return 0;
}

static void add_device(char *buffer, int *index, ei_x_buff *reply)
{
  ei_x_encode_tuple_header(reply, 2);
  ei_x_encode_atom(reply, "error");
  ei_x_encode_atom(reply, "unimplemented");
}

static void handle_call(char *buffer, int *index, ei_x_buff *reply)
{
  int size = 0;
  char call_type[MAXATOMLEN] = { 0 };

  bool is_bad_message =
       ei_decode_tuple_header(buffer, index, &size)
    || ei_decode_atom(buffer, index, call_type);

  if (is_bad_message) {
    ei_x_encode_tuple_header(reply, 2);
    ei_x_encode_atom(reply, "error");
    ei_x_encode_atom(reply, "bad_request");
    return;
  }

  if (strcmp(call_type, "add_device") == 0) {
    add_device(buffer, index, reply);
  }
  else {
    ei_x_encode_tuple_header(reply, 2);
    ei_x_encode_atom(reply, "error");
    ei_x_encode_atom(reply, "unimplemented");
  }
}
