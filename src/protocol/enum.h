#ifndef BACNET_ENUM_H
#define BACNET_ENUM_H

typedef struct {
  const char* atom;
  int         value;
} enum_tuple_t;

int find_enum_value(const enum_tuple_t* enum_tuples, const char* atom);

#endif /* BACNET_ENUM_H */
