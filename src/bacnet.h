#ifndef BACNET_H
#define BACNET_H

#include "ei.h"

void handle_bacnet_request(char* buffer, int* index, ei_x_buff* reply);

int bacnet_start_services();
int bacnet_stop_services();
int bacnet_wait_until_done();

#endif /* BACNET_H */
