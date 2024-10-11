#ifndef LOG_H
#define LOG_H

#define LOG_EMERGENCY(format, ...) send_log(EMERGENCY, format, ##__VA_ARGS__)
#define LOG_ALERT(format, ...)     send_log(ALERT, format, ##__VA_ARGS__)
#define LOG_CRITICAL(format, ...)  send_log(CRITICAL, format, ##__VA_ARGS__)
#define LOG_ERROR(format, ...)     send_log(ERROR, format, ##__VA_ARGS__)
#define LOG_WARNING(format, ...)   send_log(WARNING, format, ##__VA_ARGS__)
#define LOG_NOTICE(format, ...)    send_log(NOTICE, format, ##__VA_ARGS__)
#define LOG_INFO(format, ...)      send_log(INFO, format, ##__VA_ARGS__)
#define LOG_DEBUG(format, ...)     send_log(DEBUG, format, ##__VA_ARGS__)

typedef enum {
  EMERGENCY,
  ALERT,
  CRITICAL,
  ERROR,
  WARNING,
  NOTICE,
  INFO,
  DEBUG,
} log_level_t;

int send_log(log_level_t level, const char* format, ...);

#endif /* LOG_H */
