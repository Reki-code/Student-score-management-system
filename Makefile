CC := clang 
ARGS := -Wall -O2 -I. -Wno-unused-function -L./lib/  -Wl,-rpath,./lib/
CLEANEXTS = o so

.PHONY : all
all : server client

server : server.c thpool.o split.o csv.o server_command.o sem.o cJSON.o record_ops.o key_index.o
	$(CC) $(ARGS) -o $@ $^ -pthread -lmap -lsds

client : client.c
	$(CC) $(ARGS) -o $@ $^ -lmap -lsds -lclient_command

# build dynamic lib
libmap.so : map.c
	$(CC) -shared -fpic -o $@ $^

libsds.so : sds.c
	$(CC) -shared -fpic -o $@ $^

libclient_command.so : client_command.c
	$(CC) -shared -fpic -o $@ $^

# install dynamic lib
.PHONY : install
install : libmap.so libsds.so libclient_command.so
	mkdir -p ./lib
	cp -p $^ lib/

.PHONY : clean
clean :
	for fileext in $(CLEANEXTS); do rm -f *.$$fileext; done 
	rm -f server client
