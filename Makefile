CC := clang
ARGS = -Wall -O2 -I .

all: server client

server: server.c
	$(CC) $(ARGS) -o server server.c -lpthread

client: client.c
	$(CC) $(ARGS) -o client client.c

clean:
	rm -f *.o server client *~
