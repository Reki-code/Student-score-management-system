#ifndef _CLIENT_COMMAND_H_
#define _CLIENT_COMMAND_H_
#include "client.h"

void setup_command_map(map *map);
void (*get_command(client_t *client, char *cmd))(client_t *, char *line,
                                                 char *cmd);

#endif
