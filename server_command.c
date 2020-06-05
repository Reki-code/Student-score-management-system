#include "server_command.h"
#include <unistd.h>
#include <string.h>

void setup_command_map(map *map) {
  *map = map_create();
  map_set(*map, "nothing", do_nothing);
  map_set(*map, "exit", exit_command);
  map_set(*map, "help", help_command);
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
void help_command(server_t *server, chatting_t *chat) {
  const char *help_info = "学生成绩管理系统\nhelp\t\t获取帮助信息\nlist\t\t打印学生成绩\nsave\t\t保存成绩\nfind\t\t查询成绩\n";
  unsigned long len = strlen(help_info);
  write(chat->connfd, help_info, len);
}
void list_command(server_t *server, chatting_t *chat) {}
void save_command(server_t *server, chatting_t *chat) {}
void find_command(server_t *server, chatting_t *chat) {}
