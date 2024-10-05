#include <string.h>

#include "protocol/enum.h"

/**
 * @brief Retrieves the enum value corresponding to a specified string
 *        identifier.
 *
 * The purpose is to map string identifiers to their associated integer values
 * within a collection of predefined pairs. When provided with a string, the
 * goal is to locate its associated integer representation.
 *
 * @param enum_tuples A pointer to an array of structures that pair string
 *                    identifiers with integer values. The array should be
 *                    terminated by a sentinel structure where the `atom` member
 *                    is NULL, indicating the end of valid entries.
 *
 * @param atom A string identifier.
 *
 * @return The integer value associated with the provided string identifier.
 *         If the identifier is not found within the array, -1 is returned.
 *
 * @note The search process is case-sensitive, so the input string must match
 *       the identifiers exactly.
 *
 * @example
 * const enum_tuple_t my_enums[] = {
 *   {"ONE",   1},
 *   {"TWO",   2},
 *   {"THREE", 3},
 *   {NULL,    0},
 * };
 *
 * int result = find_enum_value(my_enums, "TWO"); // Returns 2
 * int not_found = find_enum_value(my_enums, "FOUR"); // Returns -1
 */
int find_enum_value(const enum_tuple_t* enum_tuples, const char* atom)
{
  for (size_t index = 0; enum_tuples[index].atom != NULL; index++) {
    if (strcmp(enum_tuples[index].atom, atom) == 0) {
      return enum_tuples[index].value;
    }
  }

  return -1;
}
