CC := clang 
ARGS := -Wall -O2 -I.

all : server client

server : server.c thpool.o split.o
	$(CC) $(ARGS) -o $@ $^ -pthread

client : client.c map.o client_command.o
	$(CC) $(ARGS) -o $@ $^

clean :
	rm - f *.o server client * ~
