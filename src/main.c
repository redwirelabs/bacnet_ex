#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#include "bacnet.h"
#include "log.h"
#include "port.h"

#ifdef USE_UDP_SOCKETS
int sockfd;
struct sockaddr_in server_addr;
#endif

int main(int argc, char** argv)
{
#ifdef USE_UDP_SOCKETS
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) {
      perror("Socket creation failed");
      return -1;
  }

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(8080);
  server_addr.sin_addr.s_addr = INADDR_ANY;
#endif

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

#ifdef USE_UDP_SOCKETS
  close(sockfd);
#endif

  return 0;
}
