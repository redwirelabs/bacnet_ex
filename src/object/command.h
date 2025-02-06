#ifndef BACNET_OBJECT_COMMAND_H
#define BACNET_OBJECT_COMMAND_H

void command_init(void);
unsigned command_count(void);
uint32_t command_index_to_instance(unsigned index);
bool command_valid_instance(uint32_t instance);
bool command_object_name(uint32_t instance, BACNET_CHARACTER_STRING* name);
bool command_name_set(uint32_t instance, char* name);
int command_read_property(BACNET_READ_PROPERTY_DATA* data);
bool command_write_property(BACNET_WRITE_PROPERTY_DATA* data);

void command_property_lists(
  const int** required,
  const int** optional,
  const int** proprietary);

#endif /* BACNET_OBJECT_COMMAND_H */
