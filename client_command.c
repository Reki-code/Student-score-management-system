#include "client_command.h"
#include <strings.h>
#include <unistd.h>

void setup_command_map(map *map) {
  *map = map_create();
  map_set(*map, "nothing", do_nothing);
  map_set(*map, "exit", exit_command);
  map_set(*map, "list", list_command);
  map_set(*map, "save", save_command);
  map_set(*map, "find", find_command);
}
void (*get_command(client_t *client, char *cmd))(client_t *) {
  void (*command)(client_t *);
  map map = client->commands;
  if (map_contains(map, cmd)) {
    command = map_get(map, cmd);
  } else {
    command = map_get(map, "nothing");
  }
  return command;
}

void do_nothing(client_t *client) {}
void exit_command(client_t *client) {
  int sockfd = client->sockfd;
  client->running = false;
  write(sockfd, "exit", 5);
}
void list_command(client_t *client){};
void save_command(client_t *client){};
void find_command(client_t *client){};
