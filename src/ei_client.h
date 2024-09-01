#ifndef EI_CLIENT_H
#define EI_CLIENT_H

#include <ei.h>

bool ei_client_config(const char *nodename, const char *cookie);
bool ei_client_send(char *process_name, ei_x_buff *message);
bool ei_client_send_to(erlang_pid *pid, ei_x_buff *messageg);
bool ei_client_call(char *module, char *func, ei_x_buff *message, ei_x_buff *out);
bool ei_client_recv(erlang_msg *meta, ei_x_buff *message);

#endif /* EI_CLIENT_H */
