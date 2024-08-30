#ifndef EI_CLIENT_H
#define EI_CLIENT_H

#include <ei.h>

typedef void(msg_handler_cb_t)(erlang_msg *msg, ei_x_buff *data);

bool ei_client_config(const char *nodename, const char *cookie);
bool ei_client_send(char *process_name, ei_x_buff *msg);

#endif /* EI_CLIENT_H */
