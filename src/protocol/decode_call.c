#include <ei.h>
#include <stdlib.h>

#include "protocol/decode_call.h"
#include "protocol/enum.h"

const enum_tuple_t BACNET_CALL_ATOMS[] = {
  {"create_gateway",                    0},
  {"create_routed_device",              1},
  {"create_routed_analog_input",        2},
  {"set_routed_analog_input_value",     3},
  {"create_routed_multistate_input",    4},
  {"set_routed_multistate_input_value", 5},
};

const size_t BACNET_CALL_SIZE_LOOKUP[] = {
  sizeof(create_routed_device_t),
  sizeof(create_routed_device_t),
  sizeof(create_routed_analog_input_t),
  sizeof(set_routed_analog_input_value_t),
  sizeof(create_routed_multistate_input_t),
  sizeof(set_routed_multistate_input_value_t),
};

static int decode_call_type(char* buffer, int* index, uint8_t* type);
static int decode_call_data(char* buffer, int* index, uint8_t type, void* data);

/**
 * @brief Allocates memory for a BACnet call of a specified type.
 *
 * Allocates a block of memory based on the given call type and initializes
 * the memory to zero. The allocated memory's address is stored in the
 * provided pointer and must be freed by the user.
 *
 * @param type The type of BACnet call for which memory is being allocated.
 * @param call A pointer to a pointer where the address of the allocated
 *             memory will be stored.
 *
 * @return Returns 0 on successful allocation, or -1 if memory allocation
 *         fails.
 */
int bacnet_call_malloc(bacnet_call_type_t type, void** call)
{
  size_t size  = BACNET_CALL_SIZE_LOOKUP[type];
  void* buffer = malloc(size);

  if (buffer == NULL)
    return -1;

  *call = memset(buffer, 0, size);

  return 0;
}

/**
 * @brief Decodes the BACnet call type from a buffer.
 *
 * Extracts the BACnet call type from the provided term buffer.
 *
 * @param buffer A pointer to the buffer containing the encoded call data.
 * @param index  A pointer to the current index in the buffer.
 * @param type   A pointer to a variable where the decoded BACnet call type
 *               will be stored.
 *
 * @return Returns 0 if decoding is successful, or -1 if the message is
 *         malformed or an error occurs.
 */
int decode_bacnet_call_type(char* buffer, int* index, bacnet_call_type_t* type)
{
  int size = 0;

  bool is_bad_message =
       ei_decode_tuple_header(buffer, index, &size)
    || (size < 2)
    || decode_call_type(buffer, index, type);

  if (is_bad_message)
    return -1;

  return 0;
}

/**
 * @brief Decodes the data for a BACnet call.
 *
 * Extracts and interprets the data associated with a specific BACnet call
 * type from the provided buffer.
 *
 * @param buffer A pointer to the buffer containing the encoded call data.
 * @param index  A pointer to the current index in the buffer.
 * @param type   The BACnet call type that indicates how to interpret the data.
 * @param data   A pointer to the memory where the decoded data will be stored.
 *               This memory should be allocated prior to the decoding process.
 *
 * @return Returns 0 if decoding is successful, or -1 if an error occurs
 *         during the decoding process.
 */
int decode_bacnet_call(
  char* buffer,
  int* index,
  bacnet_call_type_t type,
  void* data
) {
  return decode_call_data(buffer, index, type, data);
}

static int decode_call_type(char* buffer, int* index, uint8_t* type)
{
  char atom[MAXATOMLEN] = { 0 };

  if (ei_decode_atom(buffer, index, atom) == -1) {
    return -1;
  }

  int enum_value = find_enum_value(BACNET_CALL_ATOMS, atom);
  if (enum_value == -1)
    return -1;

  *type = enum_value;

  return 0;
}

static int decode_create_routed_device(
  char* buffer,
  int* index,
  create_routed_device_t* data
) {
  long size             = 0;
  int  type             = 0;
  char name[MAXATOMLEN] = { 0 };

  bool is_invalid =
       ei_decode_ulong(buffer, index, (unsigned long*)&data->bacnet_id)
    || ei_get_type(buffer, index, &type, (int*)&size)
    || (size >= sizeof(name))
    || ei_decode_binary(buffer, index, name, (long*)&size)
    || characterstring_init_ansi(&data->name, name) == false
    || ei_get_type(buffer, index, &type, (int*)&size)
    || (size >= sizeof(data->description))
    || ei_decode_binary(buffer, index, data->description, &size)
    || ei_get_type(buffer, index, &type, (int*)&size)
    || (size >= sizeof(data->model))
    || ei_decode_binary(buffer, index, data->model, (long*)&size)
    || ei_get_type(buffer, index, &type, (int*)&size)
    || (size >= sizeof(data->firmware_version))
    || ei_decode_binary(buffer, index, data->firmware_version, &size);

  return is_invalid ? -1 : 0;
}

static int decode_bacnet_unit_atom(
  char* buffer,
  int* index,
  BACNET_ENGINEERING_UNITS* unit
) {
  char atom[MAXATOMLEN] = { 0 };

  if (ei_decode_atom(buffer, index, atom) == -1)
    return -1;

  int enum_value = find_enum_value(BACNET_UNIT_ATOMS, atom);
  if (enum_value == -1)
    return -1;

  *unit = (BACNET_ENGINEERING_UNITS)enum_value;

  return 0;
}

static int decode_create_routed_analog_input(
  char* buffer,
  int* index,
  create_routed_analog_input_t* data
) {
  long size = 0;
  int  type = 0;

  bool is_invalid =
       ei_decode_ulong(buffer, index, (unsigned long*)&data->device_bacnet_id)
    || ei_decode_ulong(buffer, index, (unsigned long*)&data->object_bacnet_id)
    || ei_get_type(buffer, index, &type, (int*)&size)
    || (size >= sizeof(data->name))
    || (memset(data->name, 0, sizeof(data->name)) == NULL)
    || ei_decode_binary(buffer, index, data->name, &size)
    || decode_bacnet_unit_atom(buffer, index, &data->unit);

  return is_invalid ? -1 : 0;
}

static int decode_set_routed_analog_input_value(
  char* buffer,
  int* index,
  set_routed_analog_input_value_t* data
) {
  bool is_invalid =
       ei_decode_ulong(buffer, index, (unsigned long*)&data->device_bacnet_id)
    || ei_decode_ulong(buffer, index, (unsigned long*)&data->object_bacnet_id)
    || ei_decode_double(buffer, index, &data->value);

  return is_invalid ? -1 : 0;
}

static int decode_multistate_states(
  char* buffer,
  int* index,
  char** out,
  size_t* out_length
) {
  int  type        = 0;
  long atom_size   = 0;
  int  atoms_count = 0;
  int  cursor      = 0;

  if (ei_decode_list_header(buffer, index, &atoms_count) || atoms_count == 0)
    return -1;

  for (int i = 0; i < atoms_count; i++) {
    if (ei_get_type(buffer, index, &type, (int *)&atom_size))
      return -1;

    *out = realloc(*out, cursor + atom_size + 1);
    if (*out == NULL)
      goto cleanup;

    char* cursor_buffer = *out + cursor;
    memset(cursor_buffer, 0, atom_size + 1);

    if (ei_decode_binary(buffer, index, cursor_buffer, &atom_size))
      goto cleanup;

    cursor += atom_size + 1;
    *out_length = cursor;
    continue;

  cleanup:
    if (*out != NULL)
      free(*out);

    return -1;
  }

  return 0;
}

static int decode_create_routed_multistate_input(
  char* buffer,
  int* index,
  create_routed_multistate_input_t* data
) {
  long size = 0;
  int  type = 0;

  bool is_invalid =
       ei_decode_ulong(buffer, index, (unsigned long*)&data->device_bacnet_id)
    || ei_decode_ulong(buffer, index, (unsigned long*)&data->object_bacnet_id)
    || ei_get_type(buffer, index, &type, (int*)&size)
    || (size >= sizeof(data->name))
    || (memset(data->name, 0, sizeof(data->name)) == NULL)
    || ei_decode_binary(buffer, index, data->name, &size)
    || decode_multistate_states(buffer, index, &data->states, &data->states_length);

  return is_invalid ? -1 : 0;
}

static int decode_set_routed_multistate_input_value(
  char* buffer,
  int* index,
  set_routed_multistate_input_value_t* data
) {
  bool is_invalid =
       ei_decode_ulong(buffer, index, (unsigned long*)&data->device_bacnet_id)
    || ei_decode_ulong(buffer, index, (unsigned long*)&data->object_bacnet_id)
    || ei_decode_ulong(buffer, index, (unsigned long*)&data->value);

  return is_invalid ? -1 : 0;
}

static int decode_call_data(
  char* buffer,
  int* index,
  bacnet_call_type_t type,
  void* data
) {
  switch(type) {
    case CALL_CREATE_GATEWAY:
      return decode_create_routed_device(buffer, index, data);

    case CALL_CREATE_ROUTED_DEVICE:
      return decode_create_routed_device(buffer, index, data);

    case CALL_CREATE_ROUTED_ANALOG_INPUT:
      return decode_create_routed_analog_input(buffer, index, data);

    case CALL_SET_ROUTED_ANALOG_INPUT_VALUE:
      return decode_set_routed_analog_input_value(buffer, index, data);

    case CALL_CREATE_ROUTED_MULTISTATE_INPUT:
      return decode_create_routed_multistate_input(buffer, index, data);

    case CALL_SET_ROUTED_MULTISTATE_INPUT_VALUE:
      return decode_set_routed_multistate_input_value(buffer, index, data);

    default:
      return -1;
  }
}
