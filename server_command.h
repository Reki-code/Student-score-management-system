#ifndef _SERVER_COMMAND_H_
#define _SERVER_COMMAND_H_
#include "server.h"

void do_nothing(server_t *server, chatting_t *chat);
void exit_command(server_t *server, chatting_t *chat);
void list_command(server_t *server, chatting_t *chat);
void save_command(server_t *server, chatting_t *chat);
void find_command(server_t *server, chatting_t *chat);

#endif