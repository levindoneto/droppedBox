CC = g++
INCLUDE = -I./util
FLAGS = -O2 -Wall -lpthread -std=gnu++11
CFLAGS = $(FLAGS) $(INCLUDE)
UTILS = utils/src/device.o utils/src/communication.o utils/src/process.o\
 utils/src/dropboxUtils.o utils/fileSystem/src/folder.o utils/fileSystem/src/file.o

.PHONY: all clean

all: dropboxServer dropboxClient

dropboxServer: $(UTILS) server/src/dropboxServer.o server/src/serverCommunication.o server/src/serverUser.o
	$(CC) $(CFLAGS) -o $@ $^

dropboxClient: $(UTILS) client/src/dropboxClient.o client/src/clientCommunication.o client/src/clientUser.o
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *~ *. ./*/*.o ./*/*.out dropboxClient dropboxServer
