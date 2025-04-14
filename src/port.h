#ifndef PORT_H
#define PORT_H

#include <ei.h>

typedef void (*handle_request_t)(char* buffer, int* index, ei_x_buff* reply);

int port_start(handle_request_t handle_request);
int port_wait_until_done();
int port_send(ei_x_buff* message);
int port_read(ei_x_buff* message);

#endif /* PORT_H */
