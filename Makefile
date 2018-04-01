CC=g++
INCLUDE=-I./util
FLAGS=-O2 -Wall -pthread -std=gnu++11
CFLAGS=$(FLAGS) $(INCLUDE)

UTIL=utils/src/device.o

.PHONY: all clean

all: dropboxServer dropboxClient

dropboxServer: $(UTIL) server/src/dropboxServer.o server/src/serverCommunication.o server/src/serverUser.o
	$(CC) $(CFLAGS) -o $@ $^

dropboxClient: $(UTIL) client/src/dropboxClient.o client/src/clientCommunication.o client/src/clientUser.o
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *~ *. ./*/*.o ./*/*.out dropboxClient dropboxServer
