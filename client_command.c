#include "client_command.h"
#include <strings.h>
#include <unistd.h>

void (*find_command_hander(client_t *client, char *cmd))(client_t *client) {
  return do_nothing;
}
void do_nothing(client_t *client) {}
void exit_command(client_t *client) {
  char buff[MAX];
  int sockfd = client->sockfd;
  client->running = false;
  write(sockfd, buff, sizeof(buff));
}
void list_command(client_t *client){};
void save_command(client_t *client){};
void find_command(client_t *client){};
void login_command(client_t *client){};
