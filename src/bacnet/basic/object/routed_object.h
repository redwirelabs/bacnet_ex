#ifndef BACNET_BASIC_ROUTED_OBJECT_H
#define BACNET_BASIC_ROUTED_OBJECT_H

#include <stdbool.h>

#include "bacnet/bacenum.h"
#include "bacnet/basic/sys/keylist.h"

#define MAX_OBJ_NAME_LEN 32
#define MAX_OBJ_DESC_LEN 64

typedef struct routed_object {
  BACNET_OBJECT_TYPE Type;
#if OS_LAYER_LINUX
} __attribute__((packed)) ROUTED_OBJECT;
#else
} ROUTED_OBJECT;
#endif

int Routed_Object_Count_By_Type(OS_Keylist objects, BACNET_OBJECT_TYPE type);

void *
Routed_Object_Next_By_Type(
  OS_Keylist objects,
  BACNET_OBJECT_TYPE type,
  int *cursor);

bool
Routed_Object_Index_Key(
  OS_Keylist objects,
  BACNET_OBJECT_TYPE type,
  int index,
  KEY *key);

#endif /* BACNET_BASIC_ROUTED_OBJECT_H */
