#ifndef _SERVER_COMMAND_H_
#define _SERVER_COMMAND_H_
#include "server.h"

void setup_command_map(map *map);
void (*get_command(server_t *client, char *cmd))(server_t *, chatting_t *);

#endif
