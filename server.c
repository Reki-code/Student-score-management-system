#include "server.h"
#include "server_command.h"
#include "split.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#define PORT 8080
#define SA struct sockaddr

volatile sig_atomic_t running;
void sig_handler(int signo) {
  if (signo == SIGINT) {
    running = false;
  }
}

int main() {
  server_t *server;
  server = initialize_server();
  if (signal(SIGINT, sig_handler) == SIG_ERR) {
    perror("sigal");
  }
  run(server);
  destory_server(server);
}

void run(server_t *server) {
  int sockfd = server->sockfd;
  socklen_t len;
  int connfd;
  struct sockaddr_in cli;

  while (running) {
    if ((listen(sockfd, 5)) != 0) {
      printf("Listen failed...\n");
      exit(0);
    } else
      printf("Server listening..\n");
    len = sizeof(cli);

    // Accept the data packet from client and verification
    connfd = accept(sockfd, (SA *)&cli, &len);
    if (connfd < 0) {
      printf("server acccept failed...\n");
      exit(0);
    } else {
      printf("server acccept the client...\n");
    }
    // Function for chatting between client and server
    struct chatting_server *chat;
    chat = malloc(sizeof(struct chatting_server));
    chat->server = server;
    chat->connfd = connfd;
    thpool_add_work(server->workers, chatting, (void *)chat);
  }
}

bool verify_password(server_t *server, char *name, char *password) {
  CSV_BUFFER *password_csv_buffer = server->password_csv_buffer;

  bool login = false;
  char buff[MAX];
  int height = csv_get_height(password_csv_buffer);
  for (int row = 1; row < height; row++) {
    csv_get_field(buff, MAX, password_csv_buffer, row, 0);
    if (strcmp(buff, name) == 0) {
      csv_get_field(buff, MAX, password_csv_buffer, row, 1);
      if (strcmp(buff, password) == 0) {
        login = true;
        break;
      }
    }
  }
  return login;
}
void chatting(void *chat_ptr) {
  struct chatting_server *chat = (struct chatting_server *)chat_ptr;
  int connfd = chat->connfd;
  char buff[MAX];
  bool islogin = false;
  do {
    read(connfd, buff, sizeof(buff));
    // check password
    char buf[1024];
    size_t argc;
    char *argv[20];
    strcpy(buf, buff);
    argc = split(buf, argv, 20);
    if (argc == 2) {
      islogin = verify_password(chat->server, argv[0], argv[1]);
    }
    // send rsp to client
    bzero(buff, MAX);
    sprintf(buff, "%s", islogin ? "success" : "failed");
    write(connfd, buff, sizeof(buff));
  } while (islogin == false);

  chatting_t chatting;
  chatting.connfd = connfd;
  chatting.running = true;
  while (chatting.running) {
    bzero(buff, MAX);
    read(connfd, buff, sizeof(buff));
    // split buff
    char buf[1024];
    size_t argc;
    char *argv[20];
    strcpy(buf, buff);
    argc = split(buf, argv, 20);
    // find_command
    void (*command)(server_t *, chatting_t *);
    server_t *server = chat->server;
    command = get_command(server, argv[0]);
    // command exectue
    command(server, &chatting);
  }
}

int initialize_connection(void) {
  int sockfd;

  struct sockaddr_in servaddr;
  // socket create and verification
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  int option = 1;
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
  if (sockfd == -1) {
    printf("socket creation failed...\n");
    exit(0);
  } else
    printf("Socket successfully created..\n");
  bzero(&servaddr, sizeof(servaddr));

  // assign IP, PORT
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(PORT);
  // Binding newly created socket to given IP and verification
  if ((bind(sockfd, (SA *)&servaddr, sizeof(servaddr))) != 0) {
    printf("socket bind failed...\n");
    perror("");
    exit(0);
  } else {
    printf("Socket successfully binded..\n");
  }
  return sockfd;
}
server_t *initialize_server(void) {
  server_t *server;
  server = malloc(sizeof(server_t));
  server->sockfd = initialize_connection();
  running = true;
  server->workers = thpool_init(MAX_THREAD);
  int fd = open("record.json", O_RDONLY);
  int len = lseek(fd, 0, SEEK_END);
  char *data = mmap(0, len, PROT_READ, MAP_PRIVATE, fd, 0);
  close(fd);
  server->record = cJSON_Parse(data);
  munmap(data, len);
  Sem_init(&server->record_sem, 0, 1);
  server->password_csv_buffer = csv_create_buffer();
  csv_load(server->password_csv_buffer, "password.csv");
  setup_command_map(&server->commands);
  return server;
}

void destory_server(server_t *server) {
  thpool_wait(server->workers);
  thpool_destroy(server->workers);
  close(server->sockfd);
  cJSON_Delete(server->record);
  csv_destroy_buffer();
  map_destroy(server->commands);
}
int send_str(int sock, const char *str) {
  int n, len = strlen(str);
  while (len > 0) {
    n = write(sock, str, len);
    if (n < 0)
      return n;
    str += n;
    len -= n;
  }
  return 0;
}
