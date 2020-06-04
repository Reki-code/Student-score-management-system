#ifndef _CLIENT_H_
#define _CLIENT_H_
#include <stdbool.h>

typedef struct client client_t;

typedef struct client {
  int sockfd;
  bool running;
} client_t;

int initialize_connection(void);
client_t *initialize_client(void);
void run(client_t *client);
void destroy_client(client_t *client);
// int get_command(message_queue_t *msq, char *cmd_buffer);

#endif
