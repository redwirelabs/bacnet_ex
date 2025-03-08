#ifndef BACNET_EVENT_H
#define BACNET_EVENT_H

#include <ei.h>

int cast_command(
  uint32_t device_instance,
  uint32_t object_instance,
  uint32_t value);

#endif /* BACNET_EVENT_H */
