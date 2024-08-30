#ifndef EI_LOG_H
#define EI_LOG_H

#define LOG_ERR(format, ...) ei_log(ERROR, format __VA_ARGS__)
#define LOG_WRN(format, ...) ei_log(WARNING, format __VA_ARGS__)
#define LOG_INF(format, ...) ei_log(INFO, format __VA_ARGS__)
#define LOG_DBG(format, ...) ei_log(DEBUG, format __VA_ARGS__)

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
