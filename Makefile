CC = gcc
CFLAGS = -Wall -g
MP3_LIBS = -lmpg123 -lasound

MP3_PLAYER = mp3_player
HELLO_WORLD = hello_world

all: $(MP3_PLAYER) $(HELLO_WORLD)

$(MP3_PLAYER): mp3_player.c
	$(CC) $(CFLAGS) -o $(MP3_PLAYER) mp3_player.c $(MP3_LIBS)

$(HELLO_WORLD): hello_world.c
	$(CC) $(CFLAGS) -o $(HELLO_WORLD) hello_world.c

clean:
	rm -f $(MP3_PLAYER) $(HELLO_WORLD)

install-deps:
	sudo apt-get update
	sudo apt-get install -y libmpg123-dev libasound2-dev

.PHONY: clean install-deps all