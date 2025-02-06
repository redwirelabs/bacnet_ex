#include <bacnet/bacdef.h>
#include <bacnet/rp.h>
#include <bacnet/wp.h>

/**
 * @brief Attempts to set required, optional and proprietary command properties.
 *
 * @note All params are sentinel terminated list of integers.
 * @param required - BACnet required properties for a Command object.
 * @param optional - BACnet optional properties for a Command object.
 * @param proprietary - BACnet proprietary properties for a Command object.
 */
void command_property_lists(
  const int **required,
  const int **optional,
  const int **proprietary
) {
  static const int required_list[] = {
    PROP_OBJECT_IDENTIFIER,
    PROP_OBJECT_NAME,
    PROP_OBJECT_TYPE,
    PROP_PRESENT_VALUE,
    PROP_IN_PROCESS,
    PROP_ALL_WRITES_SUCCESSFUL,
    PROP_ACTION,
    -1
  };

  static const int optional_list[] = { -1 };
  static const int proprietary_list[] = { -1 };

  if (required) *required = required_list;
  if (optional) *optional = optional_list;
  if (proprietary) *proprietary = proprietary_list;
}

/**
 * @brief Initializes a Command object.
 */
void command_init(void)
{
  return;
}

/**
 * @brief Returns the count of Command objects for the currently selected
 *        Device.
 */
unsigned command_count(void)
{
  return -1;
}

/**
 * @brief Get the Commands's instace number from its index.
 *
 * @param index - Index of Command object from within the Devices Object's list.
 *
 * @return BACnet Object instance number.
 */
uint32_t command_index_to_instance(unsigned index)
{
  return -1;
}

/**
 * @brief Checks if the Object instance is a valid Command Object.
 *
 * @param instance - Object instance number.
 */
bool command_valid_instance(uint32_t instance)
{
  return false;
}

/**
 * @brief Retrieve the name of a Command Object.
 *
 * @param[in] instance - Object instance number.
 * @param[out] name - The Objects's name.
 */
bool command_object_name(uint32_t instance, BACNET_CHARACTER_STRING *name)
{
  return false;
}

/**
 * @brief BACnet read-property handler for a Command Object.
 *
 * @param[out] data - Holds request and reply data.
 *
 * @return Byte count of the APDU or BACNET_STATUS_ERROR.
 */
int command_read_property(BACNET_READ_PROPERTY_DATA *data)
{
  return -1;
}

/**
 * @brief BACnet write-property handler for a Command Object.
 *
 * @param[out] data - Holds request and reply data.
 *
 * @return true if no errors occur.
 */
bool command_write_property(BACNET_WRITE_PROPERTY_DATA *data)
{
  return false;
}
