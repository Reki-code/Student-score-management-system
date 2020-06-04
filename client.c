#include "client.h"
#define MAX 80 
#define PORT 8080 
#define SA struct sockaddr 
#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <unistd.h>
#include <sys/socket.h> 
#include <arpa/inet.h>

int main() 
{ 
  client_t *client;
  client = initialize_client();
  run(client);
  destroy_client(client);
} 

void run(client_t *client) 
{ 
  int sockfd = client->sockfd;
  char buff[MAX]; 
  int n; 
  while (client->running) { 
    bzero(buff, sizeof(buff)); 
    printf("> "); 
    n = 0; 
    while ((buff[n++] = getchar()) != '\n') {
      ; 
    }
    write(sockfd, buff, sizeof(buff)); 
    bzero(buff, sizeof(buff)); 
    read(sockfd, buff, sizeof(buff)); 
    printf("From Server : %s", buff); 
    if ((strncmp(buff, "exit", 4)) == 0) { 
      client->running = false;
      printf("Client Exit...\n"); 
    } 
  } 
} 
client_t *initialize_client(void) {
  client_t *client = malloc(sizeof(client_t));
  client->sockfd = initialize_connection();
  client->running = true;
  return client;
}

void destroy_client(client_t *client) {
  close(client->sockfd); 
}

int initialize_connection(void) {
  int sockfd; 
  struct sockaddr_in servaddr; 
  sockfd = socket(AF_INET, SOCK_STREAM, 0); 
  if (sockfd == -1) { 
    printf("socket creation failed...\n"); 
    exit(0); 
  } 
  else
    printf("Socket successfully created..\n"); 
  bzero(&servaddr, sizeof(servaddr)); 

  // assign IP, PORT 
  servaddr.sin_family = AF_INET; 
  servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
  servaddr.sin_port = htons(PORT); 

  // connect the client socket to server socket 
  if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) { 
    printf("connection with the server failed...\n"); 
    exit(0); 
  } 
  else
    printf("connected to the server..\n"); 
  return sockfd;
}