#include <signal.h>
#include <stdbool.h>
#include <time.h>

#include "ei_client.h"
#include "ei_log.h"

#define HEARTBEAT_PROCESS "Elixir.Bacnet.Heartbeat"

static bool initialized;
static timer_t timer;
static void heartbeat_cb(int sig, siginfo_t *si, void *uc);

bool ei_heartbeat_start()
{
  if (initialized) {
    LOG_DBG("Heartbeat already initialized");
    return true;
  }

  struct sigaction sa = {
    .sa_flags = SA_SIGINFO,
    .sa_sigaction = heartbeat_cb,
  };

  sigemptyset(&sa.sa_mask);

  if (sigaction(SIGRTMIN, &sa, NULL) < 0) {
    return false;
  }

  struct sigevent event = {
    .sigev_notify = SIGEV_SIGNAL,
    .sigev_signo = SIGRTMIN,
  };

  if (timer_create(CLOCK_REALTIME, &event, &timer)) {
    return false;
  }

  struct itimerspec spec = {
    .it_interval = {.tv_sec = 4},
    .it_value = {.tv_sec = 4},
  };

  if (timer_settime(timer, 0, &spec, NULL)) {
    return false;
  }

  initialized = true;

  return true;
}

static void heartbeat_cb(int sig, siginfo_t *si, void *uc)
{
  ei_x_buff msg;
  ei_x_new_with_version(&msg);
  ei_x_encode_atom(&msg, "heartbeat");

  if (!ei_client_send(HEARTBEAT_PROCESS, &msg)) {
    LOG_ERR("Failed to send heartbeat");
  }

  ei_x_free(&msg);
}
