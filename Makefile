CC=g++
FLAGS=-O2 -Wall -pthread -std=gnu++11
CFLAGS=$(FLAGS) $(INCLUDE)

.PHONY: all clean

all: dropboxServer dropboxClient

dropboxServer: $(UTIL) server/src/dropboxServer.o
	$(CC) $(CFLAGS) -o $@ $^

dropboxClient: $(UTIL) client/src/dropboxClient.o
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *~ *. ./*/*.o dropboxClient dropboxServer
