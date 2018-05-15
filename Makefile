CC = g++
INCLUDE = -I./util
FLAGS = -g -Wall -pthread -std=gnu++11
CFLAGS = $(FLAGS) $(INCLUDE)
UTILS_C = utils/src/device.o \
					utils/src/ui.o \
					utils/src/udpUtils.o \
					utils/src/dropboxUtils.o \
					utils/fileSystem/src/folder.o \
					utils/fileSystem/src/file.o \
					utils/src/ui.o \
					utils/src/process.o

UTILS_S = utils/src/device.o \
					utils/src/ui.o \
					utils/src/udpUtils.o \
					utils/src/dropboxUtils.o \
					utils/fileSystem/src/folder.o \
					utils/fileSystem/src/file.o \

CLIENT_O = client/src/dropboxClient.o \
					 client/src/clientUser.o

SERVER_O = server/src/dropboxServer.o \
					 server/src/serverCommunication.o \
					 server/src/serverUser.o

.PHONY: all clean

all: dropboxServer dropboxClient

dropboxServer: $(UTILS_S) $(SERVER_O)
	$(CC) $(CFLAGS) -o $@ $^

dropboxClient: $(UTILS_C) $(CLIENT_O)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

clean: removeObjects
	rm -f dropboxClient dropboxServer

removeObjects:
	rm -f $(UTILS_S) $(UTILS_C) $(SERVER_O) $(CLIENT_O)

run-client:
	./dropboxClient testuser localhost 8080

run-server:
	./dropboxServer 8080
