#include <stdlib.h>
#include <bacnet/bacdef.h>
#include <bacnet/rp.h>
#include <bacnet/wp.h>
#include <bacnet/basic/object/device.h>
#include <bacnet/basic/object/routed_object.h>
#include <bacnet/basic/sys/keylist.h>

#include "object/command.h"
#include "protocol/event.h"

static int validate_request(int apdu_len, uint32_t index, uint32_t property);
static int action_list_encode(uint32_t instance, uint32_t index, uint8_t* apdu);

static const int required_properties[] = {
  PROP_OBJECT_IDENTIFIER,
  PROP_OBJECT_NAME,
  PROP_OBJECT_TYPE,
  PROP_PRESENT_VALUE,
  PROP_IN_PROCESS,
  PROP_ALL_WRITES_SUCCESSFUL,
  PROP_ACTION,
  -1
};

static const int optional_properties[] = {
  PROP_DESCRIPTION,
  -1
};

static const int proprietary_properties[] = { -1 };

/**
 * @brief Attempts to set required, optional and proprietary command properties.
 *
 * @note All params are sentinel terminated list of integers.
 * @param required - BACnet required properties for a Command object.
 * @param optional - BACnet optional properties for a Command object.
 * @param proprietary - BACnet proprietary properties for a Command object.
 */
void command_property_lists(
  const int** required,
  const int** optional,
  const int** proprietary
) {
  if (required) *required = required_properties;
  if (optional) *optional = optional_properties;
  if (proprietary) *proprietary = proprietary_properties;
}

/**
 * @brief Handles any setup required to create Command object.
 */
void command_init(void)
{
  DEVICE_OBJECT_DATA* device = Get_Routed_Device_Object(-1);

  if (device->objects == NULL)
    device->objects = Keylist_Create();
}

/**
 * @brief Initializes a Command object.
 *
 * @param instance - Object instance number.
 *
 * @return BACNET_MAX_INSTANCE on error and a valid instance number on success.
 */
uint32_t command_create(
  DEVICE_OBJECT_DATA* device,
  uint32_t instance,
  char* name,
  char* description,
  uint32_t value,
  bool in_progress
) {
  if (instance >= BACNET_MAX_INSTANCE)
    return BACNET_MAX_INSTANCE;

  if (strlen(name) <= 0)
    return BACNET_MAX_INSTANCE;

  COMMAND_OBJECT* object =
    Keylist_Data(device->objects, instance);

  if (object != NULL)
    return instance;

  object = calloc(1, sizeof(COMMAND_OBJECT));
  if (!object)
    return BACNET_MAX_INSTANCE;

  object->type          = OBJECT_COMMAND;
  object->present_value = value;
  object->in_progress   = in_progress;
  object->successful    = true;

  memset(object->name, 0, sizeof(object->name));
  memset(object->description, 0, sizeof(object->description));

  memcpy(object->name, name, strlen(name));
  memcpy(object->description, description, strlen(description));

  if (Keylist_Data_Add(device->objects, instance, object) < 0) {
    free(object);
    return BACNET_MAX_INSTANCE;
  }

  return instance;
}

/**
 * @brief Returns the count of Command objects for the currently selected
 *        Device.
 */
unsigned command_count(void)
{
  DEVICE_OBJECT_DATA* device = Get_Routed_Device_Object(-1);

  return Routed_Object_Count_By_Type(device->objects, OBJECT_COMMAND);
}

/**
 * @brief Get the Commands's instace number from its index.
 *
 * @param index - Index of Command object from within the Devices Object's list.
 *
 * @return BACnet Object instance number.
 */
uint32_t command_index_to_instance(unsigned index)
{
  DEVICE_OBJECT_DATA* device = Get_Routed_Device_Object(-1);

  KEY key = UINT32_MAX;
  Routed_Object_Index_Key(device->objects, OBJECT_COMMAND, index, &key);

  return key;
}

/**
 * @brief Checks if the Object instance is a valid Command Object.
 *
 * @param instance - Object instance number.
 */
bool command_valid_instance(uint32_t instance)
{
  DEVICE_OBJECT_DATA* device  = Get_Routed_Device_Object(-1);
  COMMAND_OBJECT*     object = Keylist_Data(device->objects, instance);

  if (!object) return false;
  if (object->type != OBJECT_COMMAND) return false;

  return true;
}

/**
 * @brief Retrieve the name of a Command Object.
 *
 * @param[in] object - The Command Object.
 * @param[out] name - The Objects's name.
 */
bool command_name(uint32_t instance, BACNET_CHARACTER_STRING* name)
{
  DEVICE_OBJECT_DATA* device = Get_Routed_Device_Object(-1);

  COMMAND_OBJECT* object =
    Keylist_Data(device->objects, instance);

  if (object == NULL) return false;
  if (strlen(object->name) <= 0) return false;

  return characterstring_init_ansi(name, object->name);
}

/**
 * @brief Set a Command Object's name.
 *
 * @param instance - Object instance number.
 * @param name - The Objects's name.
 */
bool command_name_set(uint32_t instance, char *name)
{
  DEVICE_OBJECT_DATA* device = Get_Routed_Device_Object(-1);
  COMMAND_OBJECT*     object = Keylist_Data(device->objects, instance);

  if (!object || strlen(name) >= MAX_OBJ_NAME_LEN)
    return false;

  memset(object->name, 0, sizeof(object->name));
  strcpy(object->name, name);

  return true;
}

/**
 * @brief Set the present value of a Command Object.
 *
 * @param device - The BACnet device that owns the Command Object.
 * @param instance - Object instance number.
 * @param value - The Objects's present value.
 *
 * @note Setting the value will also set the in-progress flag to true.
 */
bool command_present_value_set(COMMAND_OBJECT* object, uint32_t value)
{
  object->present_value = value;
  object->in_progress   = true;

  return true;
}

/**
 * @brief Update the Command's status.
 *
 * @param object - The Command Object to update.
 * @param successful - True if the command succeeded.
 *
 * @note Resets in_progress to false.
 */
bool command_update_status(COMMAND_OBJECT* object, bool successful)
{
  object->in_progress   = false;
  object->present_value = 0;
  object->successful    = successful;

  return true;
 }

/**
 * @brief BACnet read-property handler for a Command Object.
 *
 * @param[out] data - Holds request and reply data.
 *
 * @return Byte count of the APDU or BACNET_STATUS_ERROR.
 */
int command_read_property(BACNET_READ_PROPERTY_DATA* data)
{
  bool is_data_invalid =
       data == NULL
    || data->application_data == NULL
    || data->application_data_len == 0;

  if (is_data_invalid) return 0;

  uint32_t instance = data->object_instance;

  DEVICE_OBJECT_DATA* device = Get_Routed_Device_Object(-1);
  COMMAND_OBJECT*     object = Keylist_Data(device->objects, instance);

  int apdu_len  = 0;
  uint8_t* apdu = data->application_data;

  switch (data->object_property) {
    case PROP_OBJECT_IDENTIFIER:
      apdu_len =
        encode_application_object_id(&apdu[0], OBJECT_COMMAND, instance);
      break;

    case PROP_OBJECT_NAME:
      BACNET_CHARACTER_STRING name;
      command_name(instance, &name);
      apdu_len = encode_application_character_string(&apdu[0], &name);
      break;

    case PROP_OBJECT_TYPE:
      apdu_len = encode_application_enumerated(&apdu[0], OBJECT_COMMAND);
      break;

    case PROP_PRESENT_VALUE:
      apdu_len =
        encode_application_unsigned(&apdu[0], object->present_value);
      break;

    case PROP_IN_PROCESS:
      apdu_len = encode_application_boolean(&apdu[0], object->in_progress);
      break;

    case PROP_ALL_WRITES_SUCCESSFUL:
      apdu_len = encode_application_boolean(&apdu[0], object->successful);
      break;

    case PROP_ACTION:
      apdu_len =
        bacnet_array_encode(
          instance,
          data->array_index,
          action_list_encode,
          MAX_COMMAND_ACTIONS,
          apdu,
          data->application_data_len
        );

      if (apdu_len == BACNET_STATUS_ABORT) {
        data->error_class = ERROR_CLASS_COMMUNICATION;
        data->error_code  = ERROR_CODE_ABORT_SEGMENTATION_NOT_SUPPORTED;
      }
      else if (apdu_len == BACNET_STATUS_ERROR) {
        data->error_class = ERROR_CLASS_PROPERTY;
        data->error_code  = ERROR_CODE_INVALID_ARRAY_INDEX;
      }
      break;

    default:
      data->error_class = ERROR_CLASS_PROPERTY;
      data->error_code  = ERROR_CODE_UNKNOWN_PROPERTY;
      apdu_len          = BACNET_STATUS_ERROR;
      break;
  }

  apdu_len =
    validate_request(apdu_len, data->array_index, data->object_property);

  if (apdu_len == BACNET_STATUS_ERROR) {
    data->error_class = ERROR_CLASS_PROPERTY;
    data->error_code  = ERROR_CODE_PROPERTY_IS_NOT_AN_ARRAY;
  }

  return apdu_len;
}

/**
 * @brief BACnet write-property handler for a Command Object.
 *
 * @param[out] data - Holds request and reply data.
 *
 * @return true if no errors occur.
 */
bool command_write_property(BACNET_WRITE_PROPERTY_DATA* data)
{
  BACNET_APPLICATION_DATA_VALUE value;

  int len =
    bacapp_decode_application_data(
      data->application_data,
      data->application_data_len,
      &value
    );

  if (len < 0) {
    data->error_class = ERROR_CLASS_PROPERTY;
    data->error_code  = ERROR_CODE_VALUE_OUT_OF_RANGE;
    return false;
  }

  len = validate_request(len, data->array_index, data->object_property);
  if (len == BACNET_STATUS_ERROR) {
    data->error_class = ERROR_CLASS_PROPERTY;
    data->error_code  = ERROR_CODE_PROPERTY_IS_NOT_AN_ARRAY;
    return false;
  }

  uint32_t instance = data->object_instance;

  DEVICE_OBJECT_DATA* device = Get_Routed_Device_Object(-1);
  COMMAND_OBJECT*     object = Keylist_Data(device->objects, instance);

  if (object == NULL) {
    data->error_class = ERROR_CLASS_OBJECT;
    data->error_code  = ERROR_CODE_UNKNOWN_OBJECT;
    return false;
  }

  switch (data->object_property) {
    case PROP_PRESENT_VALUE:
      if (object->in_progress) {
        data->error_class = ERROR_CLASS_OBJECT;
        data->error_code  = ERROR_CODE_BUSY;
        return false;
      }

      bool status =
        write_property_type_valid(
          data,
          &value,
          BACNET_APPLICATION_TAG_UNSIGNED_INT
        );

      if (!status) {
        data->error_class = ERROR_CLASS_PROPERTY;
        data->error_code  = ERROR_CODE_WRITE_ACCESS_DENIED;
        return false;
      }

      if (value.type.Unsigned_Int >= MAX_COMMAND_ACTIONS) {
        data->error_class = ERROR_CLASS_PROPERTY;
        data->error_code  = ERROR_CODE_VALUE_OUT_OF_RANGE;
        return false;
      }

      if (!command_present_value_set(object, value.type.Unsigned_Int))
        return false;

      int sent_ret =
        send_command(
          device->bacObj.Object_Instance_Number,
          instance,
          value.type.Unsigned_Int
        );

      return sent_ret == 0;
    default:
      bool is_valid_prop =
        property_lists_member(
          required_properties,
          optional_properties,
          proprietary_properties,
          data->object_property
        );

      if (is_valid_prop) {
        data->error_class = ERROR_CLASS_PROPERTY;
        data->error_code  = ERROR_CODE_WRITE_ACCESS_DENIED;
      }
      else {
        data->error_class = ERROR_CLASS_PROPERTY;
        data->error_code  = ERROR_CODE_UNKNOWN_PROPERTY;
      }
      break;
  }

  return false;
}

static int validate_request(
  int apdu_len,
  BACNET_ARRAY_INDEX array_index,
  BACNET_PROPERTY_ID object_property
) {
  if (apdu_len < 0) return apdu_len;

  bool requesting_array_index = array_index != BACNET_ARRAY_ALL;
  if (requesting_array_index && object_property != PROP_ACTION) {
    return BACNET_STATUS_ERROR;
  }

  return apdu_len;
}

static int action_list_encode(
  uint32_t instance,
  BACNET_ARRAY_INDEX index,
  uint8_t* apdu
) {
  DEVICE_OBJECT_DATA* device = Get_Routed_Device_Object(-1);
  COMMAND_OBJECT*     object = Keylist_Data(device->objects, instance);

  if (!object || index >= MAX_COMMAND_ACTIONS)
    return BACNET_STATUS_ERROR;

  return bacnet_action_command_encode(apdu, &object->actions[index]);
}
