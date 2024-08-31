#ifndef EI_LOG_H
#define EI_LOG_H

#define LOG_EME(format, ...) ei_log(EMERGENCY, format, ##__VA_ARGS__)
#define LOG_ALE(format, ...) ei_log(ALERT, format, ##__VA_ARGS__)
#define LOG_CRT(format, ...) ei_log(CRITICAL, format, ##__VA_ARGS__)
#define LOG_ERR(format, ...) ei_log(ERROR, format, ##__VA_ARGS__)
#define LOG_WRN(format, ...) ei_log(WARNING, format, ##__VA_ARGS__)
#define LOG_NTC(format, ...) ei_log(NOTICE, format, ##__VA_ARGS__)
#define LOG_INF(format, ...) ei_log(INFO, format, ##__VA_ARGS__)
#define LOG_DBG(format, ...) ei_log(DEBUG, format, ##__VA_ARGS__)

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
