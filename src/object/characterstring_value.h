#ifndef BACNET_OBJECT_CHARACTERSTRING_VALUE_H
#define BACNET_OBJECT_CHARACTERSTRING_VALUE_H

#include "object/common.h"

typedef struct {
  BACNET_OBJECT_TYPE type;

  char name[MAX_STRING_LEN];
  char description[MAX_STRING_LEN];
  char present_value[MAX_STRING_LEN];
} CHARACTERSTRING_VALUE_OBJECT;

void characterstring_value_init(void);

uint32_t characterstring_value_create(
  DEVICE_OBJECT_DATA* device,
  uint32_t instance,
  char* name,
  char* description,
  char* value);

unsigned characterstring_value_count(void);
uint32_t characterstring_value_index_to_instance(unsigned index);
bool characterstring_value_valid_instance(uint32_t instance);

bool characterstring_value_name(
  uint32_t instance,
  BACNET_CHARACTER_STRING* name);

int characterstring_value_read_property(BACNET_READ_PROPERTY_DATA* data);

void characterstring_value_property_lists(
  const int** required,
  const int** optional,
  const int** proprietary);

#endif /* BACNET_OBJECT_CHARACTERSTRING_VALUE_H */
