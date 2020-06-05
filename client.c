#include "client.h"
#include "client_command.h"
#define MAX 80
#define PORT 8080
#define SA struct sockaddr
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {
  client_t *client;
  client = initialize_client();
  run(client);
  destroy_client(client);
}
int get_input(char *buffer, char *cmd);
int get_input(char *buffer, char *cmd) {
  printf("> ");
  char *line = NULL;
  size_t len = 0;
  ssize_t read = 0;
  read = getline(&line, &len, stdin);
  if (read == -1) {
    perror("getline");
    exit(1);
  }
  line[read - 1] = '\0';
  strncpy(buffer, line, read);

  char ch;
  for (int i = 0; i < 32; i++) {
    ch = line[i];
    if (ch == ' ' || ch == '\0') {
      break;
    }
    cmd[i] = ch;
  }

  free(line);
  return read;
}

void run(client_t *client) {
  char buff[MAX];
  char cmd[32];

  bool login = false;
  printf("欢迎使用学生成绩管理系统\n");
  printf("请输入用户名和秘密(空格分开): ");
  do {
    char *buf = NULL;
    int nread;
    size_t len;
    nread = getline(&buf, &len, stdin);
    if (nread == -1) {
      perror("getline");
    }
    buf[nread - 1] = '\0';
    write(client->sockfd, buf, nread);
    free(buf);

    read(client->sockfd, buff, sizeof(buff));
    if (buff[0] == 's') {
      login = true;
    } else {
      printf("用户名或密码错误\n重新输入:");
    }
  } while (login == false);

  while (client->running) {
    bzero(cmd, sizeof(cmd));
    get_input(buff, cmd);
    void (*command)(client_t *);
    command = get_command(client, cmd);
    command(client);
  }
}
client_t *initialize_client(void) {
  client_t *client = malloc(sizeof(client_t));
  client->sockfd = initialize_connection();
  client->running = true;
  setup_command_map(&client->commands);
  return client;
}

void destroy_client(client_t *client) {
  close(client->sockfd);
  map_destroy(client->commands);
}

int initialize_connection(void) {
  int sockfd;
  struct sockaddr_in servaddr;
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) {
    printf("socket creation failed...\n");
    exit(0);
  } else
    printf("Socket successfully created..\n");
  bzero(&servaddr, sizeof(servaddr));

  // assign IP, PORT
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  servaddr.sin_port = htons(PORT);

  // connect the client socket to server socket
  if (connect(sockfd, (SA *)&servaddr, sizeof(servaddr)) != 0) {
    printf("connection with the server failed...\n");
    exit(0);
  } else
    printf("connected to the server..\n");
  return sockfd;
}
