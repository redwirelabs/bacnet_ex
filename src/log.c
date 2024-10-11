#include <stdarg.h>

#include "log.h"
#include "port.h"

#define MAX_LOG_LEN 1024

static const char* level_to_str(log_level_t level);

/**
 * @brief Sends a log message at a specified log level.
 *
 * Prepares a log message based on the provided format and arguments, and
 * dispatches it through the port. If transmission fails, then the an attempt
 * is made to print to stderr.
 *
 * @param level  The log level indicating the severity of the message.
 * @param format The format string for the log message.
 * @param ...    Additional arguments for the format string.
 *
 * @return Returns 0 on success, or -1 if message formatting fails.
 */
int send_log(log_level_t level, const char* format, ...)
{
  va_list args;
  va_start(args, format);

  char buffer[MAX_LOG_LEN];
  int length = vsnprintf(buffer, sizeof(buffer), format, args);
  if (length < 0) {
    va_end(args);
    return -1;
  }

  ei_x_buff message;
  ei_x_new_with_version(&message);
  ei_x_encode_tuple_header(&message, 3);
  ei_x_encode_atom(&message, "log");
  ei_x_encode_atom(&message, level_to_str(level));
  ei_x_encode_string(&message, buffer);

  if (port_send(&message) == -1) {
    fprintf(stderr, "failed to send log %d\n", errno);
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
  }

  ei_x_free(&message);
  va_end(args);

  return 0;
}

static const char* level_to_str(log_level_t level)
{
  switch (level) {
    case EMERGENCY: return "emergency";
    case ALERT:     return "alert";
    case CRITICAL:  return "critical";
    case ERROR:     return "error";
    case WARNING:   return "warning";
    case NOTICE:    return "notice";
    case INFO:      return "info";
    case DEBUG:     return "debug";
  }
}
