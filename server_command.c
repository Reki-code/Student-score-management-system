#include "server_command.h"
#include "sds.h"
#include "sem.h"
#include "split.h"
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
                          "学生成绩\nsave\t\t保存成绩\nfind\t\t查询成绩\n";
  unsigned long len = strlen(help_info);
  write_large(chat->connfd, help_info, len);
}
void list_command(server_t *server, chatting_t *chat) {
  cJSON *record = server->record;
  sds rsp = sdsempty();
  char line_buffer[MAX];
  P(&server->record_sem);
  cJSON *student;
  cJSON_ArrayForEach(student, record) {
    cJSON *id = cJSON_GetObjectItem(student, "id");
    sprintf(line_buffer, "学号: %d\n", id->valueint);
    rsp = sdscat(rsp, line_buffer);
    cJSON *score = cJSON_GetObjectItem(student, "score");
    cJSON *item;
    cJSON_ArrayForEach(item, score) {
      cJSON *child = item->child;
      sprintf(line_buffer, "  %s:\t%d\n", child->string, child->valueint);
      rsp = sdscat(rsp, line_buffer);
    }
  }
  V(&server->record_sem);
  write_large(chat->connfd, rsp, sdslen(rsp));
}
void save_command(server_t *server, chatting_t *chat) {
  int connfd = chat->connfd;
  char buff[MAX];
  bzero(buff, MAX);
  read(connfd, buff, sizeof(buff));
  sds rsp = sdsempty();

  char buf[1024];
  size_t argc;
  char *argv[20];
  strcpy(buf, buff);           // 0     1      2   3
  argc = split(buf, argv, 20); // save, class, id, score
  if (argc == 4) {
    cJSON *record = server->record;
    bool has_id = false;
    P(&server->record_sem);
    cJSON *student;
    cJSON_ArrayForEach(student, record) {
      cJSON *id = cJSON_GetObjectItem(student, "id");
      if (strcmp(argv[2], id->valuestring) == 0) {
        has_id = true;
        cJSON *score = cJSON_GetObjectItem(student, "score");
        cJSON *item;
        cJSON_ArrayForEach(item, score) {
          cJSON *child = item->child;
          if (strcmp(argv[1], child->string) == 0) {
            // TODO: write back
            child->valuestring = argv[3];
          }
          sprintf(buf, "  %s:\t%d\n", child->string, child->valueint);
          rsp = sdscat(rsp, buf);
        }
      }
    }
    if (has_id == false) {
      //TODO: create id
    }
    V(&server->record_sem);
  } else {
    rsp = sdscat(rsp, "wrong command");
  }
  write_large(connfd, rsp, sdslen(rsp));

}
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
