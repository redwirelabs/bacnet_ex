#include <stdbool.h>
#include <stdlib.h>

#include "bacnet/basic/services.h"
#include "bacnet/basic/sys/keylist.h"
#include "bacnet/basic/object/device.h"
#include "bacnet/basic/object/routed_analog_input.h"
#include "bacnet/basic/object/routed_object.h"

static const int PROPERTIES_REQUIRED[] = {
  PROP_OBJECT_IDENTIFIER,
  PROP_OBJECT_NAME,
  PROP_OBJECT_TYPE,
  PROP_PRESENT_VALUE,
  PROP_STATUS_FLAGS,
  PROP_EVENT_STATE,
  PROP_OUT_OF_SERVICE,
  PROP_UNITS,
  -1
};

static const int PROPERTIES_OPTIONAL[] = {
  PROP_DESCRIPTION,
  PROP_RELIABILITY,
  PROP_COV_INCREMENT,
  -1
};

static const int PROPERTIES_PROPRIETARY[] = {
  -1
};

void Routed_Analog_Input_Init(void)
{
  return;
}

unsigned Routed_Analog_Input_Count(void)
{
  DEVICE_OBJECT_DATA *device = Get_Routed_Device_Object(-1);

  return Routed_Object_Count_By_Type(device->objects, OBJECT_ANALOG_INPUT);
}

uint32_t Routed_Analog_Input_Index_To_Instance(unsigned index)
{
  DEVICE_OBJECT_DATA *device = Get_Routed_Device_Object(-1);

  KEY key = UINT32_MAX;
  Routed_Object_Index_Key(
    device->objects,
    OBJECT_ANALOG_INPUT,
    index,
    &key
  );

  return key;
}

unsigned Routed_Analog_Input_Instance_To_Index(uint32_t instance)
{
  DEVICE_OBJECT_DATA *device = Get_Routed_Device_Object(-1);

  return Keylist_Index(device->objects, instance);
}

bool Routed_Analog_Input_Valid_Instance(uint32_t instance_number)
{
  DEVICE_OBJECT_DATA *device = Get_Routed_Device_Object(-1);

  if (!Keylist_Data(device->objects, instance_number))
    return false;

  return true;
}

bool
Routed_Analog_Input_Object_Name(
  uint32_t object_instance,
  BACNET_CHARACTER_STRING *out)
{
  DEVICE_OBJECT_DATA *device = Get_Routed_Device_Object(-1);

  ROUTED_ANALOG_INPUT_OBJECT *object =
    Keylist_Data(device->objects, object_instance);

  if (object == NULL)
    return false;

  if (strlen(object->Object_Name) <= 0) {
    static char default_name[MAX_OBJ_NAME_LEN] = { 0 };
    snprintf(
      default_name,
      sizeof(default_name),
      "ANALOG INPUT %lu",
      (unsigned long)object_instance
    );

    return characterstring_init_ansi(out, default_name);
  }

  return characterstring_init_ansi(out, object->Object_Name);
}

int Routed_Analog_Input_Read_Property(BACNET_READ_PROPERTY_DATA *data)
{
  bool invalid_data =
       data == NULL
    || data->application_data == NULL
    || data->application_data_len == 0;

  if (invalid_data)
    return 0;

  DEVICE_OBJECT_DATA *device = Get_Routed_Device_Object(-1);

  ROUTED_ANALOG_INPUT_OBJECT *object =
    Keylist_Data(device->objects, data->object_instance);

  if (!object)
    return BACNET_STATUS_ERROR;

  int apdu_len = 0;
  uint8_t *apdu = data->application_data;

  switch ((int)data->object_property) {
    case PROP_OBJECT_IDENTIFIER:
      apdu_len = encode_application_object_id(
        &apdu[0],
        OBJECT_ANALOG_INPUT,
        data->object_instance
      );
      break;

    case PROP_OBJECT_NAME: {
      BACNET_CHARACTER_STRING name;
      Routed_Analog_Input_Object_Name(data->object_instance, &name);
      apdu_len = encode_application_character_string(&apdu[0], &name);
      break;
    }

    case PROP_OBJECT_TYPE:
      apdu_len =
        encode_application_enumerated(&apdu[0], OBJECT_ANALOG_INPUT);
      break;

    case PROP_PRESENT_VALUE:
      apdu_len = encode_application_real(&apdu[0], object->Present_Value);
      break;

    case PROP_STATUS_FLAGS: {
      BACNET_BIT_STRING status;
      bitstring_init(&status);

      bitstring_set_bit(
        &status,
        STATUS_FLAG_IN_ALARM,
        object->Event_State != EVENT_STATE_NORMAL
      );

      bitstring_set_bit(
        &status,
        STATUS_FLAG_FAULT,
        object->Reliability != RELIABILITY_NO_FAULT_DETECTED
      );

      bitstring_set_bit(&status, STATUS_FLAG_OVERRIDDEN, false);

      bitstring_set_bit(
        &status,
        STATUS_FLAG_OUT_OF_SERVICE,
        object->Out_Of_Service
      );

      apdu_len = encode_application_bitstring(&apdu[0], &status);
      break;
    }

    case PROP_EVENT_STATE:
      apdu_len = encode_application_enumerated(&apdu[0], object->Event_State);
      break;

    case PROP_RELIABILITY:
      apdu_len = encode_application_enumerated(&apdu[0], object->Reliability);
      break;

    case PROP_OUT_OF_SERVICE:
      apdu_len = encode_application_boolean(&apdu[0], object->Out_Of_Service);
      break;

    case PROP_UNITS:
      apdu_len = encode_application_enumerated(&apdu[0], object->Units);
      break;

    case PROP_DESCRIPTION: {
      BACNET_CHARACTER_STRING description;
      characterstring_init_ansi(&description, object->Description);

      apdu_len = encode_application_character_string(&apdu[0], &description);
      break;
    }

    case PROP_COV_INCREMENT:
      apdu_len = encode_application_real(&apdu[0], object->COV_Increment);
      break;

    default:
      data->error_class = ERROR_CLASS_PROPERTY;
      data->error_code = ERROR_CODE_UNKNOWN_PROPERTY;
      apdu_len = BACNET_STATUS_ERROR;
      break;
  }

  bool is_not_valid_array =
       apdu_len >= 0
    && data->object_property != PROP_EVENT_TIME_STAMPS
    && data->array_index != BACNET_ARRAY_ALL;

  if (is_not_valid_array) {
    data->error_class = ERROR_CLASS_PROPERTY;
    data->error_code = ERROR_CODE_PROPERTY_IS_NOT_AN_ARRAY;
    apdu_len = BACNET_STATUS_ERROR;
  }

  return apdu_len;
}

bool Routed_Analog_Input_Units_Set(uint32_t object_instance, uint16_t units)
{
  DEVICE_OBJECT_DATA *device = Get_Routed_Device_Object(-1);

  ROUTED_ANALOG_INPUT_OBJECT *object =
    Keylist_Data(device->objects, object_instance);

  if (object == NULL)
    return false;

  object->Units = units;

  return true;
}

bool Routed_Analog_Input_Name_Set(uint32_t object_instance, char *name)
{
  DEVICE_OBJECT_DATA *device = Get_Routed_Device_Object(-1);

  ROUTED_ANALOG_INPUT_OBJECT *object =
    Keylist_Data(device->objects, object_instance);

  if (!object || strlen(name) >= MAX_OBJ_NAME_LEN)
    return false;

  memset(object->Object_Name, 0, sizeof(object->Object_Name));
  strcpy(object->Object_Name, name);

  return true;
}

void
Routed_Analog_Input_Present_Value_Set(uint32_t object_instance, float value)
{
  DEVICE_OBJECT_DATA *device = Get_Routed_Device_Object(-1);

  ROUTED_ANALOG_INPUT_OBJECT *object =
    Keylist_Data(device->objects, object_instance);

  if (!object)
    return;

  object->Present_Value = value;
}

void
Routed_Analog_Input_Property_Lists(
  const int **required,
  const int **optional,
  const int **proprietary)
{
  if (required)
    *required = PROPERTIES_REQUIRED;

  if (optional)
    *optional = PROPERTIES_OPTIONAL;

  if (proprietary)
    *proprietary = PROPERTIES_PROPRIETARY;
}

bool
Routed_Analog_Input_Encode_Value_List(
  uint32_t object_instance,
  BACNET_PROPERTY_VALUE *value_list)
{
  DEVICE_OBJECT_DATA *device = Get_Routed_Device_Object(-1);

  ROUTED_ANALOG_INPUT_OBJECT *object =
    Keylist_Data(device->objects, object_instance);

  if (!object)
    return false;

  bool in_alarm = object->Event_State != EVENT_STATE_NORMAL;
  bool faulted = object->Reliability != RELIABILITY_NO_FAULT_DETECTED;

  return cov_value_list_encode_real(
    value_list,
    object->Present_Value,
    in_alarm,
    faulted,
    false,
    object->Out_Of_Service
  );
}

bool Routed_Analog_Input_Change_Of_Value(uint32_t instance_number)
{
  DEVICE_OBJECT_DATA *device = Get_Routed_Device_Object(-1);

  ROUTED_ANALOG_INPUT_OBJECT *object =
    Keylist_Data(device->objects, instance_number);

  if (!object)
    return false;

  return object->Changed;
}

void Routed_Analog_Input_Change_Of_Value_Clear(uint32_t instance_number)
{
  DEVICE_OBJECT_DATA *device = Get_Routed_Device_Object(-1);

  ROUTED_ANALOG_INPUT_OBJECT *object =
    Keylist_Data(device->objects, instance_number);

  if (object)
    object->Changed = false;
}

uint32_t Routed_Analog_Input_Create(uint32_t object_instance)
{
  DEVICE_OBJECT_DATA *device = Get_Routed_Device_Object(-1);

  if (object_instance >= BACNET_MAX_INSTANCE)
    return BACNET_MAX_INSTANCE;

  if (device->objects == NULL)
    device->objects = Keylist_Create();

  ROUTED_ANALOG_INPUT_OBJECT *object =
    Keylist_Data(device->objects, object_instance);

  if (object != NULL)
    return object_instance;

  object = calloc(1, sizeof(ROUTED_ANALOG_INPUT_OBJECT));
  if (!object)
    return BACNET_MAX_INSTANCE;

  object->Type = OBJECT_ANALOG_INPUT;
  object->Reliability = RELIABILITY_NO_FAULT_DETECTED;
  object->COV_Increment = 1.0;
  object->Present_Value = 0.0f;
  object->Prior_Value = 0.0;
  object->Units = UNITS_PERCENT;
  object->Out_Of_Service = false;
  object->Changed = false;
  object->Event_State = EVENT_STATE_NORMAL;

  memset(object->Object_Name, 0, sizeof(object->Object_Name));
  memset(object->Description, 0, sizeof(object->Description));

  if (Keylist_Data_Add(device->objects, object_instance, object) < 0) {
    free(object);
    return BACNET_MAX_INSTANCE;
  }

  return object_instance;
}

bool Routed_Analog_Input_Delete(uint32_t object_instance)
{
  DEVICE_OBJECT_DATA *device = Get_Routed_Device_Object(-1);

  ROUTED_ANALOG_INPUT_OBJECT *object =
    Keylist_Data_Delete(device->objects, object_instance);

  if (!object)
    return false;

  free(object);

  return true;
}
