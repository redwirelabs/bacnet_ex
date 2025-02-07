#ifndef BACNET_OBJECT_COMMAND_H
#define BACNET_OBJECT_COMMAND_H

#include <bacnet/bacaction.h>

#ifndef MAX_COMMAND_ACTIONS
#define MAX_COMMAND_ACTIONS 8
#endif

#define MAX_OBJ_NAME_LEN 32
#define MAX_OBJ_DESC_LEN 64

typedef struct {
  BACNET_OBJECT_TYPE type;

  uint32_t present_value;
  bool     in_progress;
  bool     successful;
  char     name[MAX_OBJ_NAME_LEN];
  char     description[MAX_OBJ_DESC_LEN];

  BACNET_ACTION_LIST actions[MAX_COMMAND_ACTIONS]; // TODO: make this dynamic?
} COMMAND_OBJECT;

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
