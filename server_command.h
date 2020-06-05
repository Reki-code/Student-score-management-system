#ifndef _SERVER_COMMAND_H_
#define _SERVER_COMMAND_H_
#include "server.h"

void setup_command_map(map *map);
void (*get_command(server_t *client, char *cmd))(server_t *, chatting_t *);

void do_nothing(server_t *server, chatting_t *chat);
void exit_command(server_t *server, chatting_t *chat);
void help_command(server_t *server, chatting_t *chat);
void list_command(server_t *server, chatting_t *chat);
void save_command(server_t *server, chatting_t *chat);
void find_command(server_t *server, chatting_t *chat);

#endif
