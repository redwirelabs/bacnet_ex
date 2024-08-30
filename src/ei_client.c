#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "ei_client.h"

#define OTP_COMPAT_VER 25
#define HEARTBEAT_PROCESS "Elixir.Bacnet.Heartbeat"
#define CNODE_NAME "bacnetd"

struct ei_client {
  bool ready;
  pthread_mutex_t lock;
  struct ei_cnode_s cnode;
  uint32_t creation;
  int port;
  int fd;
  char nodename[MAXNODELEN + 1];
  char cookie[MAXATOMLEN + 1];
};

static struct ei_client client = { 0 };
static void ei_free();

bool ei_client_config(const char *nodename, const char *cookie)
{
  if (client.ready) {
    return true;
  }

  if (atexit(ei_free) != 0) {
    return false;
  }

  strncpy(client.nodename, nodename, sizeof(client.nodename));
  strncpy(client.cookie, cookie, sizeof(client.cookie));

  ei_init();
  ei_set_compat_rel(OTP_COMPAT_VER);

  pthread_mutex_init(&client.lock, NULL);
  client.creation = 0;

  if (ei_connect_init(&client.cnode, CNODE_NAME, cookie, client.creation) < 0) {
    return false;
  }

  client.creation++;

  client.fd = ei_connect(&client.cnode, client.nodename);
  if (client.fd < 0) {
    return false;
  }

  client.ready = true;

  return true;
}

bool ei_client_send(char *process_name, ei_x_buff *msg)
{
  if (!client.ready) {
    return false;
  }

  pthread_mutex_lock(&client.lock);
  int ret = ei_reg_send(&client.cnode, client.fd, process_name, msg->buff,
                        msg->index);
  pthread_mutex_unlock(&client.lock);

  return ret == 0 ? true : false;
}

static void ei_free()
{
  if (client.fd != NULL) {
    ei_close_connection(client.fd);
  }
}
