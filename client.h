#ifndef _CLIENT_H_
#define _CLIENT_H_
#include "map.h"
#include <stdbool.h>

typedef struct client client_t;

typedef struct client {
  int sockfd;
  bool running;
  map commands;
} client_t;

int initialize_connection(void);
client_t *initialize_client(void);
void run(client_t *client);
void destroy_client(client_t *client);

#endif
