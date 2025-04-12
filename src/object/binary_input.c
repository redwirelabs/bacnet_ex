#include <stdlib.h>
#include <bacnet/basic/object/device.h>
#include <bacnet/basic/object/routed_object.h>

#include "object/binary_input.h"

static const int required_properties[] = {
  PROP_OBJECT_IDENTIFIER,
  PROP_OBJECT_NAME,
  PROP_OBJECT_TYPE,
  PROP_PRESENT_VALUE,
  PROP_STATUS_FLAGS,
  PROP_EVENT_STATE,
  PROP_OUT_OF_SERVICE,
  PROP_POLARITY,
  -1
};

static const int optional_properties[] = {
  PROP_RELIABILITY,
  PROP_DESCRIPTION,
  PROP_ACTIVE_TEXT,
  PROP_INACTIVE_TEXT,
  -1
};

static const int proprietary_properties[] = { -1 };

/**
 * @brief Handles any setup required to create binary-input objects.
 */
void binary_input_init(void) {
  DEVICE_OBJECT_DATA* device = Get_Routed_Device_Object(-1);

  if (device->objects == NULL)
    device->objects = Keylist_Create();
}

/**
 * @brief Create a character-string object.
 *
 * @param instance - Object instance number.
 *
 * @return BACNET_MAX_INSTANCE on error and a valid instance number on success.
 */
uint32_t binary_input_create(
  DEVICE_OBJECT_DATA* device,
  uint32_t instance,
  char* name,
  char* description,
  bool value,
  BACNET_POLARITY polarity,
  char* active_text,
  char* inactive_text
) {
  if (instance >= BACNET_MAX_INSTANCE)
    return BACNET_MAX_INSTANCE;

  if (strlen(name) <= 0)
    return BACNET_MAX_INSTANCE;

  BINARY_INPUT_OBJECT* object = Keylist_Data(device->objects, instance);

  if (object != NULL)
    return instance;

  object = calloc(1, sizeof(BINARY_INPUT_OBJECT));
  if (!object)
    return BACNET_MAX_INSTANCE;

  object->type = OBJECT_BINARY_INPUT;
  object->polarity = polarity;
  object->present_value = value;

  memset(object->name, 0, sizeof(object->name));
  memset(object->description, 0, sizeof(object->description));
  memset(object->active_text, 0, sizeof(object->active_text));
  memset(object->inactive_text, 0, sizeof(object->inactive_text));

  memcpy(object->name, name, strlen(name));
  memcpy(object->description, description, strlen(description));
  memcpy(object->active_text, active_text, strlen(active_text));
  memcpy(object->inactive_text, inactive_text, strlen(inactive_text));

  if (Keylist_Data_Add(device->objects, instance, object) < 0) {
    free(object);
    return BACNET_MAX_INSTANCE;
  }

  return instance;
}

/**
 * @brief Returns the count of binary-input value objects for the currently
 *        selected Device.
 */
unsigned binary_input_count(void) {
  DEVICE_OBJECT_DATA* device = Get_Routed_Device_Object(-1);

  unsigned count =
    Routed_Object_Count_By_Type(device->objects, OBJECT_BINARY_INPUT);

  return count;
}

/**
 * @brief Get the binary-input's instace number from its index.
 *
 * @param index - Index of the object from within the Devices Object's list.
 *
 * @return BACnet Object instance number.
 */
uint32_t binary_input_index_to_instance(unsigned index) {
  DEVICE_OBJECT_DATA* device = Get_Routed_Device_Object(-1);

  KEY key = UINT32_MAX;
  Routed_Object_Index_Key(device->objects, OBJECT_BINARY_INPUT, index, &key);

  return key;
}

/**
 * @brief Checks if the Object instance is a valid binary-input Object.
 *
 * @param instance - Object instance number.
 */
bool binary_input_valid_instance(uint32_t instance) {
  DEVICE_OBJECT_DATA*  device = Get_Routed_Device_Object(-1);
  BINARY_INPUT_OBJECT* object = Keylist_Data(device->objects, instance);

  if (!object) return false;
  if (object->type != OBJECT_BINARY_INPUT) return false;

  return true;
}

/**
 * @brief Retrieve the name of a binary-input Object.
 *
 * @param[in] instance - Object instance number.
 * @param[out] name - The Objects's name.
 */
bool binary_input_name(uint32_t instance, BACNET_CHARACTER_STRING* name) {
  DEVICE_OBJECT_DATA*  device = Get_Routed_Device_Object(-1);
  BINARY_INPUT_OBJECT* object = Keylist_Data(device->objects, instance);

  if (object == NULL) return false;
  if (strlen(object->name) <= 0) return false;

  return characterstring_init_ansi(name, object->name);
}

/**
 * @brief Retrieve the description of a binary-input Object.
 *
 * @param[in] object - A binary-input Object.
 * @param[out] description - The Objects's description.
 */
bool binary_input_description(
  BINARY_INPUT_OBJECT* object,
  BACNET_CHARACTER_STRING* description
) {
  if (strlen(object->description) <= 0) return false;

  return characterstring_init_ansi(description, object->description);
}

/**
 * @brief Retrieve the active-text value of a binary-input Object.
 *
 * @param[in] object - A binary-input Object.
 * @param[out] active_text - The Objects's active-text value.
 */
bool binary_input_active_text(
  BINARY_INPUT_OBJECT* object,
  BACNET_CHARACTER_STRING* active_text
) {
  if (strlen(object->active_text) <= 0) return false;

  return characterstring_init_ansi(active_text, object->active_text);
}

/**
 * @brief Retrieve the inactive-text value of a binary-input Object.
 *
 * @param[in] object - A binary-input Object.
 * @param[out] inactive_text - The Objects's inactive-text value.
 */
bool binary_input_inactive_text(
  BINARY_INPUT_OBJECT* object,
  BACNET_CHARACTER_STRING* inactive_text
) {
  if (strlen(object->inactive_text) <= 0) return false;

  return characterstring_init_ansi(inactive_text, object->inactive_text);
}

/**
 * @brief BACnet read-property handler for binary-input Object.
 *
 * @param[out] data - Holds request and reply data.
 *
 * @return Byte count of the APDU or BACNET_STATUS_ERROR.
 */
int binary_input_read_property(BACNET_READ_PROPERTY_DATA* data)
{
  bool is_data_invalid =
       data == NULL
    || data->application_data == NULL
    || data->application_data_len == 0;

  if (is_data_invalid) return 0;

  uint32_t instance = data->object_instance;

  DEVICE_OBJECT_DATA*  device = Get_Routed_Device_Object(-1);
  BINARY_INPUT_OBJECT* object = Keylist_Data(device->objects, instance);

  if (!object) {
    data->error_class = ERROR_CLASS_OBJECT;
    data->error_code  = ERROR_CODE_UNKNOWN_OBJECT;
    return BACNET_STATUS_ERROR;
  }

  int apdu_len  = 0;
  uint8_t* apdu = data->application_data;

  switch (data->object_property) {
    case PROP_OBJECT_IDENTIFIER:
      apdu_len =
        encode_application_object_id(
          &apdu[0],
          OBJECT_BINARY_INPUT,
          data->object_instance
        );
      break;

    case PROP_OBJECT_NAME:
      BACNET_CHARACTER_STRING name;
      binary_input_name(data->object_instance, &name);
      apdu_len = encode_application_character_string(&apdu[0], &name);
      break;

    case PROP_DESCRIPTION:
      BACNET_CHARACTER_STRING description;
      binary_input_description(object, &description);
      apdu_len = encode_application_character_string(&apdu[0], &description);
      break;

    case PROP_OBJECT_TYPE:
      apdu_len =
        encode_application_enumerated(&apdu[0], OBJECT_BINARY_INPUT);
      break;

    case PROP_PRESENT_VALUE:
      apdu_len = encode_application_enumerated(&apdu[0], object->present_value);
      break;

    case PROP_POLARITY:
      apdu_len = encode_application_enumerated(&apdu[0], object->polarity);
      break;

    case PROP_ACTIVE_TEXT:
      BACNET_CHARACTER_STRING active_text;
      binary_input_active_text(object, &active_text);
      apdu_len = encode_application_character_string(&apdu[0], &active_text);
      break;

    case PROP_INACTIVE_TEXT:
      BACNET_CHARACTER_STRING inactive_text;
      binary_input_inactive_text(object, &inactive_text);
      apdu_len = encode_application_character_string(&apdu[0], &inactive_text);
      break;

    case PROP_STATUS_FLAGS:
      BACNET_BIT_STRING status;
      bitstring_init(&status);

      bitstring_set_bit(&status, STATUS_FLAG_IN_ALARM, false);
      bitstring_set_bit(&status, STATUS_FLAG_FAULT, false);
      bitstring_set_bit(&status, STATUS_FLAG_OVERRIDDEN, false);
      bitstring_set_bit(&status, STATUS_FLAG_OUT_OF_SERVICE, false);

      apdu_len = encode_application_bitstring(&apdu[0], &status);
      break;

    case PROP_EVENT_STATE:
      apdu_len = encode_application_enumerated(&apdu[0], EVENT_STATE_NORMAL);
      break;

    case PROP_OUT_OF_SERVICE:
      apdu_len = encode_application_boolean(&apdu[0], false);
      break;

    case PROP_RELIABILITY:
      apdu_len =
        encode_application_enumerated(&apdu[0], RELIABILITY_NO_FAULT_DETECTED);
      break;

    default:
      data->error_class = ERROR_CLASS_PROPERTY;
      data->error_code  = ERROR_CODE_UNKNOWN_PROPERTY;
      apdu_len          = BACNET_STATUS_ERROR;
      break;
  }

  if (apdu_len < 0) return apdu_len;

  if (data->array_index != BACNET_ARRAY_ALL) {
    data->error_class = ERROR_CLASS_PROPERTY;
    data->error_code  = ERROR_CODE_PROPERTY_IS_NOT_AN_ARRAY;
    return BACNET_STATUS_ERROR;
  }

  return apdu_len;
}

/**
 * @brief Attempts to set required, optional and proprietary properties.
 *
 * @note All params are sentinel terminated list of integers.
 * @param required - BACnet required properties for a binary-input object.
 * @param optional - BACnet optional properties for a binary-input object.
 * @param proprietary - BACnet proprietary properties for a binary-input object.
 */
void binary_input_property_lists(
  const int** required,
  const int** optional,
  const int** proprietary
) {
  if (required) *required = required_properties;
  if (optional) *optional = optional_properties;
  if (proprietary) *proprietary = proprietary_properties;
}

/**
 * @brief Update the present value.
 *
 * @param object - The Object to update.
 * @param value - The new binary value to set.
 */
bool binary_input_set_present_value(BINARY_INPUT_OBJECT* object, bool value)
{
  object->present_value = value;

  return true;
}
