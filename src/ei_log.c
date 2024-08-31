#include <stdarg.h>
#include <stdbool.h>

#include "ei_client.h"
#include "ei_log.h"

#define MAX_LOG_LEN 1024

static const char *level_to_str(log_level_t level);

void ei_log(log_level_t level, const char *format, ...)
{
  va_list vargs;
  va_start(vargs, format);

  char log_buffer[MAX_LOG_LEN];
  int log_len = vsnprintf(log_buffer, sizeof(log_buffer), format, vargs);

  ei_x_buff out;
  ei_x_new(&out);

  ei_x_buff args;
  ei_x_new(&args);
  ei_x_encode_list_header(&args, 2);
  ei_x_encode_atom(&args, level_to_str(level));
  ei_x_encode_binary(&args, log_buffer, log_len);
  ei_x_encode_empty_list(&args);

  if (!ei_client_call("Elixir.Bacnet", "ei_log", &args, &out)) {
    char new_format[MAX_LOG_LEN];
    snprintf(new_format, sizeof(new_format), "%s\n", format);
    vprintf(new_format, vargs);
  }

  ei_x_free(&out);
  ei_x_free(&args);
  va_end(vargs);
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
