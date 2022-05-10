all: server client sleep

server: mainShell.c commands.c inputHandling.c
	gcc mainShell.c commands.c inputHandling.c -o server -lpthread

client: client.c
	gcc client.c -o client

sleep: sleep.c
	gcc sleep.c -o sleep

clean:
	rm client server sleep