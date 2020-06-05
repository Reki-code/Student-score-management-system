#ifndef _CLIENT_COMMAND_H_
#define _CLIENT_COMMAND_H_
#include "client.h"
#define MAX 512

void setup_command_map(map *map);
void (*get_command(client_t *client, char *cmd))(client_t *);

void do_nothing(client_t *client);
void exit_command(client_t *client);
void help_command(client_t *client);
void list_command(client_t *client);
void save_command(client_t *client);
void find_command(client_t *client);

#endif
