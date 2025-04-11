#ifndef BACNET_OBJECT_BINARY_INPUT_H
#define BACNET_OBJECT_BINARY_INPUT_H

#include <bacnet/bacaction.h>

#define MAX_OBJ_NAME_LEN 128
#define MAX_OBJ_DESC_LEN 128

typedef struct {
  BACNET_OBJECT_TYPE type;

  char name[MAX_OBJ_NAME_LEN];
  char description[MAX_OBJ_DESC_LEN];
  char active_text[MAX_OBJ_NAME_LEN];
  char inactive_text[MAX_OBJ_NAME_LEN];
  bool present_value;

  BACNET_POLARITY polarity;
} BINARY_INPUT_OBJECT;

void binary_input_init(void);
unsigned binary_input_count(void);
uint32_t binary_input_index_to_instance(unsigned index);
bool binary_input_valid_instance(uint32_t instance);
bool binary_input_name(uint32_t instance, BACNET_CHARACTER_STRING* name);
int binary_input_read_property(BACNET_READ_PROPERTY_DATA* data);
bool binary_input_set_present_value(BINARY_INPUT_OBJECT* object, bool value);

uint32_t binary_input_create(
  DEVICE_OBJECT_DATA* device,
  uint32_t instance,
  char* name,
  char* description,
  bool value,
  BACNET_POLARITY polarity,
  char* active_text,
  char* inactive_text);

void binary_input_property_lists(
  const int** required,
  const int** optional,
  const int** proprietary);

#endif /* BACNET_OBJECT_BINARY_INPUT_H */
