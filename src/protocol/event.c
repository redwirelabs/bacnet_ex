#include "protocol/event.h"
#include "port.h"

int send_command(
  uint32_t device_instance,
  uint32_t object_instance,
  uint32_t value
) {
  ei_x_buff reply;
  ei_x_new_with_version(&reply);
  ei_x_encode_tuple_header(&reply, 2);
  ei_x_encode_atom(&reply, "$event");

  // {:command, device_id, object_id, value}
  ei_x_encode_tuple_header(&reply, 4);
  ei_x_encode_atom(&reply, "command");
  ei_x_encode_ulong(&reply, device_instance);
  ei_x_encode_ulong(&reply, object_instance);
  ei_x_encode_ulong(&reply, value);

  return port_send(&reply);
}
