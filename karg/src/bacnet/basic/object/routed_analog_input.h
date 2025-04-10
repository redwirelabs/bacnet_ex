#ifndef BACNET_BASIC_ROUTED_OBJECT_ANALOG_INPUT_H
#define BACNET_BASIC_ROUTED_OBJECT_ANALOG_INPUT_H

#include <stdbool.h>
#include <stdint.h>
#include "bacnet/rp.h"

#define MAX_OBJ_NAME_LEN 32
#define MAX_OBJ_DESC_LEN 64

typedef struct routed_analog_input_object {
  BACNET_OBJECT_TYPE Type;
  unsigned           Event_State:3;
  float              Present_Value;
  uint32_t           Reliability;
  bool               Out_Of_Service;
  uint8_t            Units;
  float              Prior_Value;
  float              COV_Increment;
  bool               Changed;
  char               Object_Name[MAX_OBJ_NAME_LEN];
  char               Description[MAX_OBJ_DESC_LEN];
#if OS_LAYER_LINUX
} __attribute__((packed)) ROUTED_ANALOG_INPUT_OBJECT;
#else
} ROUTED_ANALOG_INPUT_OBJECT;
#endif

BACNET_STACK_EXPORT
void Routed_Analog_Input_Property_Lists(
    const int **required,
    const int **optional,
    const int **proprietary);

BACNET_STACK_EXPORT
bool Routed_Analog_Input_Valid_Instance(uint32_t object_instance);

BACNET_STACK_EXPORT
unsigned Routed_Analog_Input_Count(void);

BACNET_STACK_EXPORT
uint32_t Routed_Analog_Input_Index_To_Instance(unsigned index);

BACNET_STACK_EXPORT
unsigned Routed_Analog_Input_Instance_To_Index(uint32_t instance);

BACNET_STACK_EXPORT
bool Routed_Analog_Input_Object_Name(
  uint32_t object_instance,
  BACNET_CHARACTER_STRING * object_name);

BACNET_STACK_EXPORT
bool Routed_Analog_Input_Name_Set(uint32_t object_instance, char *name);

BACNET_STACK_EXPORT
bool Routed_Analog_Input_Units_Set(uint32_t instance, uint16_t units);

BACNET_STACK_EXPORT
int Routed_Analog_Input_Read_Property(BACNET_READ_PROPERTY_DATA *data);

BACNET_STACK_EXPORT
void Routed_Analog_Input_Present_Value_Set(
  uint32_t object_instance,
  float value);

BACNET_STACK_EXPORT
void Analog_Input_Out_Of_Service_Set(uint32_t object_instance, bool flags);

BACNET_STACK_EXPORT
unsigned Routed_Analog_Input_Event_State(uint32_t object_instance);

BACNET_STACK_EXPORT
bool Routed_Analog_Input_Event_State_Set(
  uint32_t object_instance,
  unsigned state);

BACNET_STACK_EXPORT
bool Routed_Analog_Input_Change_Of_Value(uint32_t instance);

BACNET_STACK_EXPORT
void Routed_Analog_Input_Change_Of_Value_Clear(uint32_t instance);

BACNET_STACK_EXPORT
bool Routed_Analog_Input_Encode_Value_List(
  uint32_t object_instance,
  BACNET_PROPERTY_VALUE *value_list);

BACNET_STACK_EXPORT
void Routed_Analog_Input_Intrinsic_Reporting(uint32_t object_instance);

BACNET_STACK_EXPORT
uint32_t Routed_Analog_Input_Create(uint32_t object_instance);

BACNET_STACK_EXPORT
bool Routed_Analog_Input_Delete(uint32_t object_instance);

BACNET_STACK_EXPORT
void Routed_Analog_Input_Init(void);

#endif /* BACNET_BASIC_ROUTED_OBJECT_ANALOG_INPUT_H */
