#ifndef BACNET_DECODE_CALL_H
#define BACNET_DECODE_CALL_H

#include <bacnet/bacstr.h>

typedef enum {
  CALL_CREATE_GATEWAY = 0,
  CALL_UNKNOWN = 255,
} __attribute__((packed)) bacnet_call_type_t;

typedef struct {
  uint32_t                bacnet_id;
  BACNET_CHARACTER_STRING name;
  char                    description[MAXATOMLEN];
  char                    model[MAXATOMLEN];
  char                    firmware_version[MAXATOMLEN];
} create_routed_device_t;

int bacnet_call_malloc(bacnet_call_type_t type, void** call);

int decode_bacnet_call_type(char* buffer, int* index, bacnet_call_type_t* type);

int decode_bacnet_call(
      char* buffer,
      int* index,
      bacnet_call_type_t type,
      void* call);

#endif /* BACNET_DECODE_CALL_H */
