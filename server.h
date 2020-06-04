#ifndef _SERVER_H_
#define _SERVER_H_
#include "thpool.h"
#include <pthread.h>
#include <stdbool.h>
#define MAX_THREAD 5

typedef struct server {
  int sockfd;
  bool running;
  threadpool workers;
} server_t;

server_t *initialize_server(void);
void destory_server(server_t *server);
int initialize_connection(void);
void run(server_t *server);

struct chatting_server {
  server_t *server;
  int connfd;
};
void chatting(void *chat);
int send_str(int sock, const char *str);

#endif
