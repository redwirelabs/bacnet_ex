#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "log.h"
#include "port.h"

#define STRINGIFY(x) #x
#define TOSTR(x) STRINGIFY(x)
#define ERL_TUPLE TOSTR(ERL_SMALL_TUPLE_EXT) TOSTR(ERL_SMALL_TUPLE_EXT)

typedef struct {
  erlang_ref from_ref;
  erlang_pid from_pid;
  ei_x_buff  request;
} gen_call_t;

static pthread_t        read_thread_id;
static pthread_mutex_t  write_lock;
static handle_request_t handle_request;

static int read_exact(uint8_t* buffer, size_t length);
static int read_u32(uint32_t* value);
static void* read_loop(void* arg);
static int decode_gen_call(char* buffer, int* index, gen_call_t* command);

/**
 * @brief Starts the port operations and initializes resources.
 *
 * Creates a new thread for handling incoming requests using the specified
 * callback function.
 *
 * @param handle_request_cb A callback function to handle incoming requests.
 *
 * @return Returns 0 on success, or -1 if thread creation fails.
 */
int port_start(handle_request_t handle_request_cb)
{
  if (pthread_create(&read_thread_id, NULL, &read_loop, NULL) != 0) {
    LOG_DEBUG("bacnetd: failed to create port read thread");
    return -1;
  }

  pthread_mutex_init(&write_lock, NULL);
  handle_request = handle_request_cb;

  return 0;
}

/**
 * @brief Waits for the port operations to complete.
 *
 * Blocks the calling thread until the port gracefully shutsdown.
 *
 * @return Returns 0 on success, or -1 if stopping the thread fails.
 */
int port_wait_until_done()
{
  if (pthread_join(read_thread_id, NULL) != 0) {
    LOG_DEBUG("bacnetd: failed to join thread");
    return -1;
  }

  return 0;
}

/**
 * @brief Sends a message through the port.
 *
 * Writes the specified message to stdout.
 *
 * @param message A pointer to an `ei_x_buff` structure containing the message
 *                to be sent.
 *
 * @return Returns 0 on success, or -1 if an error occurs during the write
 *         operation.
 */
int port_send(ei_x_buff* message)
{
  uint32_t total_bytes = htonl(message->index);
  size_t sent_bytes = 0;

  pthread_mutex_lock(&write_lock);
  int rt = write(STDOUT_FILENO, &total_bytes, sizeof(total_bytes));
  if (rt != 4) {
    goto error;
  }

  while (sent_bytes < message->index) {
    size_t sent = write(
      STDOUT_FILENO,
      message->buff + sent_bytes,
      message->index - sent_bytes
    );

    if (sent < 0) {
      goto error;
    }

    sent_bytes += sent;
  }

  pthread_mutex_unlock(&write_lock);
  return 0;

error:
  pthread_mutex_unlock(&write_lock);
  return -1;
}

/**
 * @brief Reads a message from the port.
 *
 * Reads the total byte count of an incoming message and allocates sufficient
 * memory to store the message, expanding as needed.
 *
 * @param message A pointer to an `ei_x_buff` structure where the
 *                incoming message will be stored.
 *
 * @return Returns 0 on success, or a non-zero error code if an
 *         error occurs during reading or memory allocation.
 */
int port_read(ei_x_buff* message)
{
  int result_code = 0;
  uint32_t total_bytes = 0;

  result_code = read_u32(&total_bytes);
  if (result_code != 0)
    return result_code;

  message->index = total_bytes;
  if (message->index > message->buffsz) {
    uint8_t* expanded_buffer = realloc(message->buff, message->index);
    if (expanded_buffer == NULL)
      return ENOMEM;

    message->buff = expanded_buffer;
    message->buffsz = message->index;
  }

  result_code = read_exact(message->buff, message->index);
  if (result_code != 0)
    return result_code;

  return 0;
}

static void* read_loop(void* arg)
{
  LOG_DEBUG("bacnetd: starting read loop");

  while (true) {
    ei_x_buff message;
    ei_x_new(&message);

    int return_code = port_read(&message);
    if (return_code == EBADF)
      break;
    else if (return_code != 0)
      goto cleanup;

    int index = 0;
    int version = 0;
    ei_term term = { 0 };
    char message_type[MAXATOMLEN] = { 0 };

    gen_call_t call_command = { 0 };
    if (decode_gen_call(message.buff, &index, &call_command) == -1)
      goto cleanup;

    // reply {:"$gen_reply", {PID, [:alias | REF]}, RESULT}
    ei_x_buff reply;
    ei_x_new_with_version(&reply);
    ei_x_encode_tuple_header(&reply, 3);
    ei_x_encode_atom(&reply, "$gen_reply");

    ei_x_encode_tuple_header(&reply, 2);
    ei_x_encode_pid(&reply, &call_command.from_pid);
    ei_x_encode_list_header(&reply, 1);
    ei_x_encode_atom(&reply, "alias");
    ei_x_encode_ref(&reply, &call_command.from_ref);

    ei_x_buff* request = &call_command.request;
    handle_request(request->buff, &request->index, &reply);

    if (port_send(&reply) == -1)
      LOG_ERROR("bacnetd: unable to send reply");

    ei_x_free(&reply);

  cleanup:
    ei_x_free(&message);
  }

  pthread_exit(NULL);
}

static int decode_gen_call(char* buffer, int* index, gen_call_t* command)
{
  int  size             = 0;
  int  term_type        = 0;
  char atom[MAXATOMLEN] = { 0 };

  int  version                  = 0;
  char message_type[MAXATOMLEN] = { 0 };

  bool is_bad_message =
       ei_decode_version(buffer, index, &version)
    || ei_decode_tuple_header(buffer, index, &size)
    || (size < 2)
    || ei_decode_atom(buffer, index, message_type)
    || strcmp(message_type, "$gen_call")
    || ei_decode_tuple_header(buffer, index, &size)
    || (size != 2)
    || ei_decode_pid(buffer, index, &command->from_pid)
    || ei_decode_list_header(buffer, index, &size)
    || (size != 1)
    || ei_decode_atom(buffer, index, atom)
    || strcmp(atom, "alias")
    || ei_decode_ref(buffer, index, &command->from_ref)
    || ei_get_type(buffer, index, &term_type, &size)
    || memchr(ERL_TUPLE, term_type, sizeof(ERL_TUPLE)) == NULL;

  if (is_bad_message)
    return -1;

  command->request.buff = buffer;
  command->request.index = *index;

  return 0;
}

static int read_exact(uint8_t* buffer, size_t length)
{
  size_t read_bytes = 0;

  while (read_bytes < length) {
    size_t sent = read(STDIN_FILENO, buffer + read_bytes, length - read_bytes);
    switch(sent) {
      case 0: return EBADF;
      case -1: return errno;
    }

    read_bytes += sent;
  }

  return 0;
}

static int read_u32(uint32_t* value)
{
  uint8_t buffer[4] = { 0 };

  int result_code = read_exact(buffer, sizeof(buffer));
  if (result_code != 0)
    return result_code;

  *value = ntohl(*(uint32_t*)buffer);

  return 0;
}
