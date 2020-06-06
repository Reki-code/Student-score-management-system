#include "server_command.h"
#include "sds.h"
#include "sem.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
static ssize_t write_large(int, const void *, size_t);

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

void do_nothing(server_t *server, chatting_t *chat) { printf("do nothing\n"); }
void exit_command(server_t *server, chatting_t *chat) {
  chat->running = false;
  close(chat->connfd);
}
void help_command(server_t *server, chatting_t *chat) {
  const char *help_info = "学生成绩管理系统\nhelp\t\t获取帮助信息\nlist\t\t打印"
                          "学生成绩\nsave\t\t保存成绩\nfind\t\t查询成绩";
  unsigned long len = strlen(help_info);
  write_large(chat->connfd, help_info, len);
}
void list_command(server_t *server, chatting_t *chat) {
  CSV_BUFFER *record = server->record_csv_buffer;
  P(&server->record_sem);
  int height = csv_get_height(record);
  int width = csv_get_width(record, 0); // 应该一样宽
  char **field = malloc(sizeof(char *) * width);
  for (int col = 0; col < width; col++) {
    field[col] = malloc(sizeof(char) * 25);
  }
  sds rsp = sdsempty();
  for (int row = 0; row < height; row++) {
    for (int col = 0; col < width; col++) {
      csv_get_field(field[col], 25, record, row, col);
    }
    rsp = sdscatsds(rsp, sdscat(sdsjoin(field, width, "\t"), "\n"));
  }
  V(&server->record_sem);
  write_large(chat->connfd, rsp, sdslen(rsp));
}
void save_command(server_t *server, chatting_t *chat) {}
void find_command(server_t *server, chatting_t *chat) {}
static ssize_t write_large(int connfd, const void *msg, size_t len) {
  char buff[MAX];
  bzero(buff, MAX);
  sprintf(buff, "%ld", len);
  write(connfd, buff, MAX);
  ssize_t nwrite;
  while (len > 0) {
    bzero(buff, MAX);
    nwrite = write(connfd, msg, len);
    len -= nwrite;
  }
  return len;
}
