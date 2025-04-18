#include <stdlib.h>
#include <bacnet/basic/object/device.h>
#include <bacnet/basic/object/routed_object.h>

#include "object/characterstring_value.h"

static const int required_properties[] = {
  PROP_OBJECT_IDENTIFIER,
  PROP_OBJECT_NAME,
  PROP_OBJECT_TYPE,
  PROP_PRESENT_VALUE,
  PROP_STATUS_FLAGS,
  -1
};

static const int optional_properties[] = {
  PROP_EVENT_STATE,
  PROP_OUT_OF_SERVICE,
  PROP_DESCRIPTION,
  -1
};

static const int proprietary_properties[] = { -1 };

/**
 * @brief Handles any setup required to create character-string objects.
 */
void characterstring_value_init(void) {
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
uint32_t characterstring_value_create(
  DEVICE_OBJECT_DATA* device,
  uint32_t instance,
  char* name,
  char* description,
  char* value
) {
  if (instance >= BACNET_MAX_INSTANCE)
    return BACNET_MAX_INSTANCE;

  if (strlen(name) <= 0)
    return BACNET_MAX_INSTANCE;

  CHARACTERSTRING_VALUE_OBJECT* object =
    Keylist_Data(device->objects, instance);

  if (object != NULL)
    return instance;

  object = calloc(1, sizeof(CHARACTERSTRING_VALUE_OBJECT));
  if (!object)
    return BACNET_MAX_INSTANCE;

  object->type = OBJECT_CHARACTERSTRING_VALUE;

  memset(object->name, 0, sizeof(object->name));
  memset(object->description, 0, sizeof(object->description));
  memset(object->present_value, 0, sizeof(object->present_value));

  memcpy(object->name, name, strlen(name));
  memcpy(object->description, description, strlen(description));
  memcpy(object->present_value, value, strlen(value));

  if (Keylist_Data_Add(device->objects, instance, object) < 0) {
    free(object);
    return BACNET_MAX_INSTANCE;
  }

  return instance;
}

/**
 * @brief Returns the count of character-string value objects for the currently
 *        selected Device.
 */
unsigned characterstring_value_count(void) {
  DEVICE_OBJECT_DATA* device = Get_Routed_Device_Object(-1);

  unsigned count =
    Routed_Object_Count_By_Type(
      device->objects,
      OBJECT_CHARACTERSTRING_VALUE
    );

  return count;
}

/**
 * @brief Get the character-string's instace number from its index.
 *
 * @param index - Index of the object from within the Devices Object's list.
 *
 * @return BACnet Object instance number.
 */
uint32_t characterstring_value_index_to_instance(unsigned index) {
  DEVICE_OBJECT_DATA* device = Get_Routed_Device_Object(-1);

  KEY key = UINT32_MAX;
  Routed_Object_Index_Key(
    device->objects,
    OBJECT_CHARACTERSTRING_VALUE,
    index,
    &key
  );

  return key;
}

/**
 * @brief Checks if the Object instance is a valid character-string value
 *        Object.
 *
 * @param instance - Object instance number.
 */
bool characterstring_value_valid_instance(uint32_t instance) {
  DEVICE_OBJECT_DATA* device = Get_Routed_Device_Object(-1);

  CHARACTERSTRING_VALUE_OBJECT* object =
    Keylist_Data(device->objects, instance);

  if (!object) return false;
  if (object->type != OBJECT_CHARACTERSTRING_VALUE) return false;

  return true;
}

/**
 * @brief Retrieve the name of a character-string Object.
 *
 * @param[in] instance - Object instance number.
 * @param[out] name - The Objects's name.
 */
bool
characterstring_value_name(uint32_t instance, BACNET_CHARACTER_STRING* name) {
  DEVICE_OBJECT_DATA* device = Get_Routed_Device_Object(-1);

  CHARACTERSTRING_VALUE_OBJECT* object =
    Keylist_Data(device->objects, instance);

  if (object == NULL) return false;
  if (strlen(object->name) <= 0) return false;

  return characterstring_init_ansi(name, object->name);
}

/**
 * @brief Retrieve the description of a character-string Object.
 *
 * @param[in] object - A character-string Object.
 * @param[out] description - The Objects's description.
 */
bool characterstring_value_description(
  CHARACTERSTRING_VALUE_OBJECT* object,
  BACNET_CHARACTER_STRING* description
) {
  if (strlen(object->description) <= 0) return false;

  return characterstring_init_ansi(description, object->description);
}

/**
 * @brief Retrieve the present-value of a character-string Object.
 *
 * @param[in] object - A character-string Object.
 * @param[out] value - The Objects's present-value.
 */
bool characterstring_value_present_value(
  CHARACTERSTRING_VALUE_OBJECT* object,
  BACNET_CHARACTER_STRING* value
) {
  if (strlen(object->present_value) <= 0) return false;

  return characterstring_init_ansi(value, object->present_value);
}

/**
 * @brief BACnet read-property handler for character-string Object.
 *
 * @param[out] data - Holds request and reply data.
 *
 * @return Byte count of the APDU or BACNET_STATUS_ERROR.
 */
int characterstring_value_read_property(BACNET_READ_PROPERTY_DATA* data)
{
  bool is_data_invalid =
       data == NULL
    || data->application_data == NULL
    || data->application_data_len == 0;

  if (is_data_invalid) return 0;

  DEVICE_OBJECT_DATA* device = Get_Routed_Device_Object(-1);

  CHARACTERSTRING_VALUE_OBJECT* object =
    Keylist_Data(device->objects, data->object_instance);

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
          OBJECT_CHARACTERSTRING_VALUE,
          data->object_instance
        );
      break;

    case PROP_OBJECT_NAME:
    {
      BACNET_CHARACTER_STRING name;
      characterstring_value_name(data->object_instance, &name);
      apdu_len = encode_application_character_string(&apdu[0], &name);
      break;
    }

    case PROP_DESCRIPTION:
    {
      BACNET_CHARACTER_STRING description;
      characterstring_value_description(object, &description);
      apdu_len = encode_application_character_string(&apdu[0], &description);
      break;
    }

    case PROP_OBJECT_TYPE:
      apdu_len =
        encode_application_enumerated(&apdu[0], OBJECT_CHARACTERSTRING_VALUE);
      break;

    case PROP_PRESENT_VALUE:
    {
      BACNET_CHARACTER_STRING present_value;
      characterstring_value_present_value(object, &present_value);
      apdu_len = encode_application_character_string(&apdu[0], &present_value);
      break;
    }

    case PROP_STATUS_FLAGS:
    {
      BACNET_BIT_STRING status;
      bitstring_init(&status);

      bitstring_set_bit(&status, STATUS_FLAG_IN_ALARM, false);
      bitstring_set_bit(&status, STATUS_FLAG_FAULT, false);
      bitstring_set_bit(&status, STATUS_FLAG_OVERRIDDEN, false);
      bitstring_set_bit(&status, STATUS_FLAG_OUT_OF_SERVICE, false);

      apdu_len = encode_application_bitstring(&apdu[0], &status);
      break;
    }

    case PROP_EVENT_STATE:
      apdu_len = encode_application_enumerated(&apdu[0], EVENT_STATE_NORMAL);
      break;

    case PROP_OUT_OF_SERVICE:
      apdu_len = encode_application_boolean(&apdu[0], false);
      break;

    default:
      data->error_class = ERROR_CLASS_PROPERTY;
      data->error_code  = ERROR_CODE_UNKNOWN_PROPERTY;
      apdu_len          = BACNET_STATUS_ERROR;
      break;
  }

  if (apdu_len < 0) return apdu_len;

  bool requesting_array_index = data->array_index != BACNET_ARRAY_ALL;
  if (requesting_array_index && data->object_property != PROP_STATE_TEXT) {
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
 * @param required - BACnet required properties for a character-string object.
 * @param optional - BACnet optional properties for a character-string object.
 * @param proprietary - BACnet proprietary properties for a character-string
 *                      object.
 */
void characterstring_value_property_lists(
  const int** required,
  const int** optional,
  const int** proprietary
) {
  if (required) *required = required_properties;
  if (optional) *optional = optional_properties;
  if (proprietary) *proprietary = proprietary_properties;
}
