#include <ei.h>

#include "log.h"
#include "port.h"

void handle_request(char* buffer, int* index, ei_x_buff* reply);

int main(int argc, char** argv)
{
  if (port_start(handle_request) == -1) {
    LOG_ERROR("bacnetd: failed to start port thread");
    return -1;
  }

  LOG_DEBUG("bacnetd: process started");
  port_wait_until_done();

  return 0;
}

void handle_request(char* buffer, int* index, ei_x_buff* reply)
{
  ei_x_encode_tuple_header(reply, 2);
  ei_x_encode_atom(reply, "error");
  ei_x_encode_atom(reply, "unimplemented");
}
