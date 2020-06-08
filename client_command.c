#include "client_command.h"
#include "sds.h"
#include "socket_conf.h"
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
static ssize_t read_large(int connfd, sds *msg);
static void do_nothing(client_t *client, char *line, char *cmd);
static void exit_command(client_t *client, char *line, char *cmd);
static void help_command(client_t *client, char *line, char *cmd);
static void list_command(client_t *client, char *line, char *cmd);
static void save_command(client_t *client, char *line, char *cmd);
static void find_command(client_t *client, char *line, char *cmd);

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

static void do_nothing(client_t *client, char *line, char *cmd) {
  printf("wrong command\n");
}
static void exit_command(client_t *client, char *line, char *cmd) {
  int sockfd = client->sockfd;
  client->running = false;
  write(sockfd, "exit", 5);
}
static void help_command(client_t *client, char *line, char *cmd) {
  int sockfd = client->sockfd;
  write(sockfd, "help", 5);
  sds msg;
  read_large(sockfd, &msg);
  printf("%s", msg);
  sdsfree(msg);
}
static void list_command(client_t *client, char *line, char *cmd) {
  int sockfd = client->sockfd;
  write(sockfd, "list", 5);
  sds msg;
  read_large(sockfd, &msg);
  printf("%s", msg);
  sdsfree(msg);
};
static void save_command(client_t *client, char *line, char *cmd) {
  int sockfd = client->sockfd;
  write(sockfd, "save", 5);
  char buff[MAX];
  sprintf(buff, "%s", line);
  write(sockfd, buff, MAX);
  sds rsp;
  read_large(sockfd, &rsp);
  printf("%s", rsp);
};
static void find_command(client_t *client, char *line, char *cmd) {
  int sockfd = client->sockfd;
  write(sockfd, "find", 5);
  char buff[MAX];
  sprintf(buff, "%s", line);
  write(sockfd, buff, MAX);
  sds rsp;
  read_large(sockfd, &rsp);
  printf("%s", rsp);
};
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
