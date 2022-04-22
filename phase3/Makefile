all: server client helloworld

server: mainShell.c commands.c inputHandling.c
	gcc mainShell.c commands.c inputHandling.c -o server -lpthread

client: client.c
	gcc client.c -o client

helloworld: helloworld.c
	gcc helloworld.c -o helloworld

clean:
	rm client server