all: server client

server: mainShell.c commands.c inputHandling.c
	gcc mainShell.c commands.c inputHandling.c -o server

client: client.c
	gcc client.c -o client

clean:
	rm client server