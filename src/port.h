#ifndef PORT_H
#define PORT_H

#include <ei.h>

// Flag to test the stack using UDP sockets.
// ivan2ed - Any suggestion for a better description?
#define USE_UDP_SOCKETS 1

typedef void (*handle_request_t)(char* buffer, int* index, ei_x_buff* reply);

int port_start(handle_request_t handle_request);
int port_wait_until_done();
int port_send(ei_x_buff* message);
int port_read(ei_x_buff* message);

#endif /* PORT_H */
