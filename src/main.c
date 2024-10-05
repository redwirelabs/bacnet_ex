#include "bacnet.h"
#include "log.h"
#include "port.h"

int main(int argc, char** argv)
{
  if (port_start(handle_bacnet_request) == -1) {
    LOG_ERROR("bacnetd: failed to start port thread");
    return -1;
  }

  if (bacnet_start_services() != 0) {
    LOG_ERROR("bacnetd: failed to start bacnet services");
    return -1;
  }

  LOG_DEBUG("bacnetd: process started");
  port_wait_until_done();
  bacnet_stop_services();
  bacnet_wait_until_done();

  return 0;
}
