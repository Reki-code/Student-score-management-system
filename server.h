#ifndef _SERVER_H_
#define _SERVER_H_
#include <stdbool.h>

typedef struct server {
  int sockfd;
  bool running;
} server_t;

server_t *initialize_server(void);
void destory_server(server_t *server);
int initialize_connection(void);
void run(server_t *server);
void chatting(int connfd, server_t *server);

#endif
