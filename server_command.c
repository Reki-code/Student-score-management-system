#include "server_command.h"
#include <unistd.h>

void do_nothing(server_t *server, chatting_t *chat) {
  printf("do nothing\n");
}
void exit_command(server_t *server, chatting_t *chat) {
  chat->running = false;
  close(chat->connfd);
}
void list_command(server_t *server, chatting_t *chat) {}
void save_command(server_t *server, chatting_t *chat) {}
void find_command(server_t *server, chatting_t *chat) {}
