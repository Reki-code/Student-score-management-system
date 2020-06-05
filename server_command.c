#include "server_command.h"
#include <unistd.h>

void setup_command_map(map *map) {
  *map = map_create();
  map_set(*map, "nothing", do_nothing);
  map_set(*map, "exit", exit_command);
  map_set(*map, "list", list_command);
  map_set(*map, "save", save_command);
  map_set(*map, "find", find_command);
}
void (*get_command(server_t *server, char *cmd))(server_t *, chatting_t *) {
  void (*command)(server_t *, chatting_t *);
  map map = server->commands;
  if (map_contains(map, cmd)) {
    command = map_get(map, cmd);
  } else {
    command = map_get(map, "nothing");
  }
  return command;
}
void do_nothing(server_t *server, chatting_t *chat) {
  printf("do nothing\n");
}
void exit_command(server_t *server, chatting_t *chat) {
  chat->running = false;
  close(chat->connfd);
}
void list_command(server_t *server, chatting_t *chat) {}
void save_command(server_t *server, chatting_t *chat) {}
void find_command(server_t *server, chatting_t *chat) {}
