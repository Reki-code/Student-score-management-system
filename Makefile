CC := clang 
ARGS := -Wall -O2 -I. -Wno-unused-function

all : server client

server : server.c thpool.o split.o csv.o map.o server_command.o sds.o sem.o cJSON.o record_ops.o
	$(CC) $(ARGS) -o $@ $^ -pthread

client : client.c map.o client_command.o sds.o
	$(CC) $(ARGS) -o $@ $^

clean :
	rm - f *.o server client * ~
