CC := clang 
ARGS := -Wall -O2 -I. -Wno-unused-function

all : server client

server : server.c thpool.o split.o csv.o server_command.o sem.o cJSON.o record_ops.o key_index.o
	$(CC) $(ARGS) -o $@ $^ -pthread -lmap -lsds

client : client.c
	$(CC) $(ARGS) -o $@ $^ -lmap -lsds -lclient_command

clean :
	rm - f *.o server client * ~
