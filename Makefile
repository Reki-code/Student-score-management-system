CC := clang 
ARGS := -Wall -O2 -I.

all : server client

server : server.c thpool.o
	$(CC) $(ARGS) -o $@ $^ -pthread

client : client.c 
	$(CC) $(ARGS) -o $@ $^

clean :
	rm - f *.o server client * ~
