#include "client_command.h"
#include "sds.h"
#include "socket_conf.h"
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
static ssize_t read_large(int connfd, sds *msg);

void setup_command_map(map *map) {
  *map = map_create();
  map_set(*map, "nothing", do_nothing);
  map_set(*map, "exit", exit_command);
  map_set(*map, "help", help_command);
  map_set(*map, "list", list_command);
  map_set(*map, "save", save_command);
  map_set(*map, "find", find_command);
}
void (*get_command(client_t *client, char *cmd))(client_t *, char *line,
                                                 char *cmd) {
  void (*command)(client_t *, char *, char *);
  map map = client->commands;
  if (map_contains(map, cmd)) {
    command = map_get(map, cmd);
  } else {
    command = map_get(map, "nothing");
  }
  return command;
}

void do_nothing(client_t *client, char *line, char *cmd) {
  printf("wrong command\n");
}
void exit_command(client_t *client, char *line, char *cmd) {
  int sockfd = client->sockfd;
  client->running = false;
  write(sockfd, "exit", 5);
}
void help_command(client_t *client, char *line, char *cmd) {
  int sockfd = client->sockfd;
  write(sockfd, "help", 5);
  sds msg;
  read_large(sockfd, &msg);
  printf("%s\n", msg);
  sdsfree(msg);
}
void list_command(client_t *client, char *line, char *cmd) {
  int sockfd = client->sockfd;
  write(sockfd, "list", 5);
  sds msg;
  read_large(sockfd, &msg);
  printf("%s\n", msg);
  sdsfree(msg);
};
void save_command(client_t *client, char *line, char *cmd){
  int sockfd = client->sockfd;
  write(sockfd, "save", 5);
  char buff[MAX];
  sprintf(buff, "%s", line);
  printf("write %s\n", line);
  write(sockfd, buff, MAX);
};
void find_command(client_t *client, char *line, char *cmd){};
static ssize_t read_large(int connfd, sds *msg) {
  char buff[MAX];
  bzero(buff, MAX);
  read(connfd, buff, MAX);
  char *remaining;
  size_t length;
  length = strtol(buff, &remaining, 10);
  *msg = sdsempty();
  ssize_t nread;
  while (length > 0) {
    bzero(buff, MAX);
    nread = read(connfd, buff, MAX);
    length -= nread;
    sdscatlen(*msg, buff, nread);
  }
  return nread;
}
