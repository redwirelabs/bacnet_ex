#ifndef BACNET_BASIC_ROUTED_OBJECT_MULTISTATE_INPUT_H
#define BACNET_BASIC_ROUTED_OBJECT_MULTISTATE_INPUT_H

#include <stdbool.h>
#include <stdint.h>
#include "bacnet/bacenum.h"
#include "bacnet/rp.h"

typedef struct routed_multistate_input_object {
  BACNET_OBJECT_TYPE Type;
  bool               Out_Of_Service;
  bool               Changed;
  bool               Write_Enabled;
  uint8_t            Present_Value;
  uint8_t            Reliability;
  char               *State_Text;
  char               Object_Name[MAX_OBJ_NAME_LEN];
  char               Description[MAX_OBJ_DESC_LEN];
#if OS_LAYER_LINUX
} __attribute__((packed)) ROUTED_ANALOG_INPUT_OBJECT;
#else
} ROUTED_MULTISTATE_INPUT_OBJECT;
#endif

BACNET_STACK_EXPORT
void Routed_Multistate_Input_Property_Lists(
    const int **required,
    const int **optional,
    const int **proprietary);

BACNET_STACK_EXPORT
bool Routed_Multistate_Input_Valid_Instance(uint32_t object_instance);

BACNET_STACK_EXPORT
unsigned Routed_Multistate_Input_Count(void);

BACNET_STACK_EXPORT
uint32_t Routed_Multistate_Input_Index_To_Instance(unsigned index);

BACNET_STACK_EXPORT
unsigned Routed_Multistate_Input_Instance_To_Index(uint32_t instance);

BACNET_STACK_EXPORT
bool Routed_Multistate_Input_Object_Name(
  uint32_t object_instance,
  BACNET_CHARACTER_STRING * object_name);

BACNET_STACK_EXPORT
bool Routed_Multistate_Input_Name_Set(uint32_t object_instance, char *name);

BACNET_STACK_EXPORT
int Routed_Multistate_Input_Read_Property(BACNET_READ_PROPERTY_DATA *data);

BACNET_STACK_EXPORT
bool Routed_Multistate_Input_State_Text_List_Set(
  uint32_t object_instance,
  char *state_text_list,
  size_t length);

BACNET_STACK_EXPORT
bool Routed_Multistate_Input_Present_Value_Set(
  uint32_t object_instance,
  uint32_t value);

BACNET_STACK_EXPORT
unsigned Routed_Multistate_Input_Event_State(uint32_t object_instance);

BACNET_STACK_EXPORT
bool Routed_Multistate_Input_Event_State_Set(
  uint32_t object_instance,
  unsigned state);

BACNET_STACK_EXPORT
bool Routed_Multistate_Input_Change_Of_Value(uint32_t instance);

BACNET_STACK_EXPORT
void Routed_Multistate_Input_Change_Of_Value_Clear(uint32_t instance);

BACNET_STACK_EXPORT
bool Routed_Multistate_Input_Encode_Value_List(
  uint32_t object_instance,
  BACNET_PROPERTY_VALUE *value_list);

BACNET_STACK_EXPORT
void Routed_Multistate_Input_Intrinsic_Reporting(uint32_t object_instance);

BACNET_STACK_EXPORT
uint32_t Routed_Multistate_Input_Create(uint32_t object_instance);

BACNET_STACK_EXPORT
bool Routed_Multistate_Input_Delete(uint32_t object_instance);

BACNET_STACK_EXPORT
void Routed_Multistate_Input_Init(void);

#endif /* BACNET_BASIC_ROUTED_OBJECT_MULTISTATE_INPUT_H */
