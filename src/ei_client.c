#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "ei_client.h"

#define OTP_COMPAT_VER 24
#define CNODE_NAME "bacnetd"

struct ei_client {
  bool              ready;
  pthread_mutex_t   lock;
  struct ei_cnode_s cnode;
  uint32_t          creation;
  int               port;
  int               fd;
  char              nodename[MAXNODELEN + 1];
  char              cookie[MAXATOMLEN + 1];
};

static struct ei_client client = { 0 };
static void ei_free();

bool ei_client_config(const char *nodename, const char *cookie)
{
  if (client.ready)
    return true;

  if (atexit(ei_free) != 0)
    return false;

  strncpy(client.nodename, nodename, sizeof(client.nodename));
  strncpy(client.cookie, cookie, sizeof(client.cookie));

  ei_init();
  ei_set_compat_rel(OTP_COMPAT_VER);

  pthread_mutex_init(&client.lock, NULL);
  client.creation = 0;

  if (ei_connect_init(&client.cnode, CNODE_NAME, cookie, client.creation) < 0)
    return false;

  client.creation++;

  client.fd = ei_connect(&client.cnode, client.nodename);
  if (client.fd < 0)
    return false;

  erlang_pid *pid = ei_self(&client.cnode);
  if (ei_global_register(client.fd, CNODE_NAME, pid) == -1)
    return false;

  client.ready = true;

  return true;
}

bool ei_client_send(char *process_name, ei_x_buff *message)
{
  if (!client.ready)
    return false;

  pthread_mutex_lock(&client.lock);
  int ret = ei_reg_send(
    &client.cnode,
    client.fd,
    process_name,
    message->buff,
    message->index
  );
  pthread_mutex_unlock(&client.lock);

  return ret == 0 ? true : false;
}

bool ei_client_send_to(erlang_pid *pid, ei_x_buff *message)
{
  if (!client.ready)
    return false;

  pthread_mutex_lock(&client.lock);
  int ret = ei_send(client.fd, pid, message->buff, message->index);
  pthread_mutex_unlock(&client.lock);

  return ret == 0 ? true : false;
}

bool ei_client_call(char *module, char *func, ei_x_buff *args, ei_x_buff *result)
{
  pthread_mutex_lock(&client.lock);
  int ret = ei_rpc(
    &client.cnode,
    client.fd,
    module,
    func,
    args->buff,
    args->index,
    result
  );
  pthread_mutex_unlock(&client.lock);

  return ret == -1 ? false : true;
}

bool ei_client_recv(erlang_msg *meta, ei_x_buff *message)
{
  pthread_mutex_lock(&client.lock);
  int ret = ei_xreceive_msg(client.fd, meta, message);
  pthread_mutex_unlock(&client.lock);

  return ret == ERL_ERROR ? false : true;
}

static void ei_free()
{
  if (client.fd > 0)
    ei_close_connection(client.fd);
}
