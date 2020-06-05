#ifndef _SERVER_H_
#define _SERVER_H_
#include "csv.h"
#include "map.h"
#include "thpool.h"
#include <stdbool.h>
#define MAX_THREAD 5

typedef struct server {
  int sockfd;
  bool running;
  threadpool workers;
  CSV_BUFFER *password_csv_buffer;
  map commands;
} server_t;

typedef struct chatting {
  int connfd;
  bool running;
} chatting_t;

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
void (*get_command(server_t *client, char *cmd))(server_t *, chatting_t *);

#endif
