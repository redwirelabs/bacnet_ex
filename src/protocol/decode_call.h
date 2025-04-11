#ifndef BACNET_DECODE_CALL_H
#define BACNET_DECODE_CALL_H

#include <bacnet/bacstr.h>

typedef enum {
  CALL_CREATE_GATEWAY,
  CALL_CREATE_ROUTED_DEVICE,
  CALL_CREATE_ROUTED_ANALOG_INPUT,
  CALL_SET_ROUTED_ANALOG_INPUT_VALUE,
  CALL_CREATE_ROUTED_MULTISTATE_INPUT,
  CALL_SET_ROUTED_MULTISTATE_INPUT_VALUE,
  CALL_CREATE_ROUTED_COMMAND,
  CALL_SET_ROUTED_COMMAND_STATUS,
  CALL_CREATE_CHARACTERSTRING_VALUE,
  CALL_UNKNOWN = 255,
} __attribute__((packed)) bacnet_call_type_t;

typedef enum {
  COMMAND_SUCCEEDED,
  COMMAND_FAILED,
} bacnet_command_status_t;

typedef struct {
  uint32_t                bacnet_id;
  BACNET_CHARACTER_STRING name;
  char                    description[MAXATOMLEN];
  char                    model[MAXATOMLEN];
  char                    firmware_version[MAXATOMLEN];
} create_routed_device_t;

typedef struct {
  uint32_t                 device_bacnet_id;
  uint32_t                 object_bacnet_id;
  char                     name[MAXATOMLEN];
  BACNET_ENGINEERING_UNITS unit;
} create_routed_analog_input_t;

typedef struct {
  uint32_t device_bacnet_id;
  uint32_t object_bacnet_id;
  double   value;
} set_routed_analog_input_value_t;

typedef struct {
  uint32_t device_bacnet_id;
  uint32_t object_bacnet_id;
  char     name[MAXATOMLEN];
  char*    states;
  size_t   states_length;
} create_routed_multistate_input_t;

typedef struct {
  uint32_t device_bacnet_id;
  uint32_t object_bacnet_id;
  uint8_t  value;
} set_routed_multistate_input_value_t;

typedef struct {
  uint32_t device_bacnet_id;
  uint32_t object_bacnet_id;
  char     name[MAXATOMLEN];
  char     description[MAXATOMLEN];
  uint32_t value;
  bool     in_progress;
} create_routed_command_t;

typedef struct {
  uint32_t device_bacnet_id;
  uint32_t object_bacnet_id;

  bacnet_command_status_t status;
} set_routed_command_status_t;

typedef struct {
  uint32_t device_bacnet_id;
  uint32_t object_bacnet_id;
  char     name[MAXATOMLEN];
  char     description[MAXATOMLEN];
  char     value[MAXATOMLEN];
} create_characterstring_value_t;

int bacnet_call_malloc(bacnet_call_type_t type, void** call);

int decode_bacnet_call_type(char* buffer, int* index, bacnet_call_type_t* type);

int decode_bacnet_call(
      char* buffer,
      int* index,
      bacnet_call_type_t type,
      void* call);

#endif /* BACNET_DECODE_CALL_H */
