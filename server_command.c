#include "server_command.h"
#include "key_index.h"
#include "record_ops.h"
#include "sds.h"
#include "sem.h"
#include "split.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
static ssize_t write_large(int, void *, size_t);
static void do_nothing(server_t *server, chatting_t *chat);
static void exit_command(server_t *server, chatting_t *chat);
static void help_command(server_t *server, chatting_t *chat);
static void list_command(server_t *server, chatting_t *chat);
static void save_command(server_t *server, chatting_t *chat);
static void find_command(server_t *server, chatting_t *chat);
static void sort_command(server_t *server, chatting_t *chat);

void setup_command_map(map *map) {
  *map = map_create();
  map_set(*map, "nothing", do_nothing);
  map_set(*map, "exit", exit_command);
  map_set(*map, "help", help_command);
  map_set(*map, "list", list_command);
  map_set(*map, "save", save_command);
  map_set(*map, "find", find_command);
  map_set(*map, "sort", sort_command);
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

static void do_nothing(server_t *server, chatting_t *chat) {
  printf("do nothing\n");
}
static void exit_command(server_t *server, chatting_t *chat) {
  chat->running = false;
  close(chat->connfd);
}
static void help_command(server_t *server, chatting_t *chat) {
  char *help_info = "学生成绩管理系统\nhelp\t\t获取帮助信息\nlist\t\t打印"
                    "学生成绩\nsave\t\t保存成绩\nfind\t\t查询成绩\n";
  unsigned long len = strlen(help_info);
  write_large(chat->connfd, help_info, len);
}
static void list_command(server_t *server, chatting_t *chat) {
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
      sprintf(line_buffer, "  %s:\t%d\n", item->string, item->valueint);
      rsp = sdscat(rsp, line_buffer);
    }
  }
  V(&server->record_sem);
  write_large(chat->connfd, rsp, sdslen(rsp));
}
static void save_command(server_t *server, chatting_t *chat) {
  int connfd = chat->connfd;
  char buff[MAX];
  bzero(buff, MAX);
  read(connfd, buff, sizeof(buff));
  bool success = false;
  int argc;
  sds *argv = sdssplitargs(buff, &argc);
  /* agrv = {0: save, 1: math, 2: 170901131, 3: 100} */
  if (argc == 4) {
    int id_n = strtol(argv[2], NULL, 10);
    bool has_id = false;
    cJSON *record = server->record;
    P(&server->record_sem);
    cJSON *student;
    cJSON_ArrayForEach(student, record) {
      cJSON *id = cJSON_GetObjectItem(student, "id");
      if (id->valueint == id_n) {
        has_id = true;
        cJSON *score = cJSON_GetObjectItem(student, "score");
        cJSON *score_item = cJSON_GetObjectItem(score, argv[1]);
        if (score_item == NULL) {
          success = score_add_class(score, argv[1], argv[3]);
        } else {
          int score_int = strtol(argv[3], NULL, 10);
          success = cJSON_SetNumberValue(score_item, score_int);
        }
        break;
      }
    }
    if (has_id == false) {
      success = record_add_student(record, argv[2], argv[1], argv[3]);
    }
    V(&server->record_sem);
  }
  sdsfreesplitres(argv, argc);
  bzero(buff, sizeof(buff));
  sprintf(buff, "%s\n", success ? "保存成功" : "保存失败");

  write_large(connfd, buff, sizeof(buff));
}
static void find_command(server_t *server, chatting_t *chat) {
  int connfd = chat->connfd;
  char buff[MAX];
  bzero(buff, MAX);
  read(connfd, buff, sizeof(buff));
  int argc;
  sds *argv = sdssplitargs(buff, &argc);

  bool has_id = false;
  sds rsp = sdsempty();
  /* agrv = {0: find, 1: id, 2: 170901131 } */
  if (argc == 3) {
    int id_n = strtol(argv[2], NULL, 10);
    cJSON *record = server->record;
    P(&server->record_sem);
    cJSON *student;
    cJSON_ArrayForEach(student, record) {
      cJSON *id = cJSON_GetObjectItem(student, "id");
      if (id->valueint == id_n) {
        has_id = true;
        sprintf(buff, "学号: %d\n", id->valueint);
        rsp = sdscat(rsp, buff);
        cJSON *score = cJSON_GetObjectItem(student, "score");
        cJSON *item;
        cJSON_ArrayForEach(item, score) {
          sprintf(buff, "  %s:\t%d\n", item->string, item->valueint);
          rsp = sdscat(rsp, buff);
        }
      }
    }
    V(&server->record_sem);
  }
  if (has_id) {
    write_large(connfd, rsp, sdslen(rsp));
  } else {
    bzero(buff, sizeof(buff));
    sprintf(buff, "%s\n", "未能找到指定学号");
    write_large(connfd, buff, sizeof(buff));
  }
}
static void sort_command(server_t *server, chatting_t *chat) {
  int connfd = chat->connfd;
  char buff[MAX];
  bzero(buff, MAX);
  read(connfd, buff, sizeof(buff));
  int argc;
  sds *argv = sdssplitargs(buff, &argc);
  sds rsp = sdsempty();
  /* agrv = {0: sort, 1: class } */
  if (argc == 2) {
    cJSON *record = server->record;
    int nstudent = 0;
    P(&server->record_sem);
    cJSON *student;
    int size = cJSON_GetArraySize(record);
    key_index_t *ki = malloc(sizeof(key_index_t) * size);
    for (int i = 0; i < size; i++) {
      student = cJSON_GetArrayItem(record, i);
      cJSON *scores = cJSON_GetObjectItem(student, "score");
      cJSON *score = cJSON_GetObjectItem(scores, argv[1]);
      if (score != NULL) {
        ki[nstudent].index = i;
        ki[nstudent].key = score->valueint;
        nstudent++;
      }
    }
    if (nstudent != 0) {
      key_index_qsort(ki, nstudent);
      for (int i = 0; i < nstudent; i++) {
        int index = ki[i].index;
        student = cJSON_GetArrayItem(record, index);
        cJSON *id = cJSON_GetObjectItem(student, "id");
        sprintf(buff, "学号: %d\n", id->valueint);
        rsp = sdscat(rsp, buff);
        cJSON *scores = cJSON_GetObjectItem(student, "score");
        cJSON *score = cJSON_GetObjectItem(scores, argv[1]);
        sprintf(buff, "  %s: %d\n", score->string, score->valueint);
        rsp = sdscat(rsp, buff);
      }
    }
    V(&server->record_sem);
  } else {
    sprintf(buff, "wrong command\n");
    rsp = sdscat(rsp, buff);
  }
  sdsfreesplitres(argv, argc);
  write_large(connfd, rsp, sdslen(rsp));
}
static ssize_t write_large(int connfd, void *msg, size_t len) {
  char buff[MAX];
  bzero(buff, MAX);
  sprintf(buff, "%ld", len);
  write(connfd, buff, MAX);
  ssize_t nwrite;
  while (len > 0) {
    nwrite = write(connfd, msg, len);
    sdsrange(msg, nwrite, -1);
    len -= nwrite;
  }
  return len;
}
