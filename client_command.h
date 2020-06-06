#ifndef _CLIENT_COMMAND_H_
#define _CLIENT_COMMAND_H_
#include "client.h"

void setup_command_map(map *map);
void (*get_command(client_t *client, char *cmd))(client_t *, char *line,
                                                 char *cmd);

void do_nothing(client_t *client, char *line, char *cmd);
void exit_command(client_t *client, char *line, char *cmd);
void help_command(client_t *client, char *line, char *cmd);
void list_command(client_t *client, char *line, char *cmd);
void save_command(client_t *client, char *line, char *cmd);
void find_command(client_t *client, char *line, char *cmd);

#endif
