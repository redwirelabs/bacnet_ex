#include "bacnet/basic/object/routed_object.h"

int Routed_Object_Count_By_Type(OS_Keylist objects, BACNET_OBJECT_TYPE type)
{
  int count = 0;

  int total_count = Keylist_Count(objects);
  for (int i = 0; i < total_count; i++) {
    ROUTED_OBJECT *object = Keylist_Data_Index(objects, i);
    if (object->Type != type)
      continue;

    count += 1;
  }

  return count;
}

void *
Routed_Object_Next_By_Type(
  OS_Keylist objects,
  BACNET_OBJECT_TYPE type,
  int *cursor)
{
  if (cursor == NULL)
    return NULL;

  int total_count = Keylist_Count(objects);
  for (int i = *cursor; i < total_count; i++) {
    ROUTED_OBJECT *object = Keylist_Data_Index(objects, i);
    if (object->Type != type)
      continue;

    *cursor = i;
    return object;
  }

  return NULL;
}

bool
Routed_Object_Index_Key(
  OS_Keylist objects,
  BACNET_OBJECT_TYPE type,
  int index,
  KEY *key)
{
  int total_count = Keylist_Count(objects);
  int type_index = 0;

  for (int i = 0; i < total_count; i++) {
    ROUTED_OBJECT *object = Keylist_Data_Index(objects, i);
    if (object->Type != type)
      continue;

    if (type_index == index)
      return Keylist_Index_Key(objects, i, key);

    type_index += 1;
  }

  return false;
}
