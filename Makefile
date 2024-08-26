CC = gcc
CFLAGS = -Wall -Werror -O2
all: ParentProcess PrimaryLikesServer

ParentProcess: ParentProcess.c
	$(CC) $(CFLAGS) -o ParentProcess ParentProcess.c
PrimaryLikesServer: PrimaryLikesServer.c
	$(CC) $(CFLAGS) -o PrimaryLikesServer PrimaryLikesServer.c
clean:
	rm -rf ParentProcess
	rm -rf PrimaryLikesServer
