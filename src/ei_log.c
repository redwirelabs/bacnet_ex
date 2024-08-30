#include <stdarg.h>
#include <stdbool.h>

#include "ei_client.h"
#include "ei_log.h"

#define MAX_LOG_LEN 1024

static const char *level_to_str(log_level_t level);

void ei_log(log_level_t level, const char *format, ...)
{
  va_list args;
  va_start(args, format);

  char log_buffer[MAX_LOG_LEN];
  int log_len = snprintf(log_buffer, sizeof(log_buffer), format, args);

  ei_x_buff msg;
  ei_x_new_with_version(&msg);
  ei_x_encode_tuple_header(&msg, 2);
  ei_x_encode_atom(&msg, level_to_str(level));
  ei_x_encode_binary(&msg, log_buffer, log_len);

  if (!ei_client_send("Elixir.Bacnet.Logger", &msg)) {
    char new_format[MAX_LOG_LEN];
    snprintf(new_format, sizeof(new_format), "%s\n", format);
    vprintf(new_format, args);
  }

  ei_x_free(&msg);
  va_end(args);
}

static const char *level_to_str(log_level_t level)
{
  switch (level) {
  case EMERGENCY:
    return "emergency";
  case ALERT:
    return "alert";
  case CRITICAL:
    return "critical";
  case ERROR:
    return "error";
  case WARNING:
    return "warning";
  case NOTICE:
    return "notice";
  case INFO:
    return "info";
  case DEBUG:
    return "debug";
  }
}
