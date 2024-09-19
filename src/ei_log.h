#ifndef EI_LOG_H
#define EI_LOG_H

#define LOG_EMERGENCY(format, ...) ei_log(EMERGENCY, format, ##__VA_ARGS__)
#define LOG_ALERT(format, ...)     ei_log(ALERT, format, ##__VA_ARGS__)
#define LOG_CRITICAL(format, ...)  ei_log(CRITICAL, format, ##__VA_ARGS__)
#define LOG_ERROR(format, ...)     ei_log(ERROR, format, ##__VA_ARGS__)
#define LOG_WARNING(format, ...)   ei_log(WARNING, format, ##__VA_ARGS__)
#define LOG_NOTICE(format, ...)    ei_log(NOTICE, format, ##__VA_ARGS__)
#define LOG_INFO(format, ...)      ei_log(INFO, format, ##__VA_ARGS__)
#define LOG_DEBUG(format, ...)     ei_log(DEBUG, format, ##__VA_ARGS__)

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

void ei_log(log_level_t level, const char *format, ...);

#endif /* EI_LOG_H */
