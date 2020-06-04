#ifndef _CLIENT_COMMAND_H_
#define _CLIENT_COMMAND_H_
#include "client.h"
#define MAX 80

void do_nothing(client_t *client);
void exit_command(client_t *client);
void list_command(client_t *client);
void save_command(client_t *client);
void find_command(client_t *client);
void login_command(client_t *client);

#endif
