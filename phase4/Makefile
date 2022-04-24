all: server client helloworld sleep

server: mainShell.c commands.c inputHandling.c
	gcc mainShell.c commands.c inputHandling.c -o server -lpthread

client: client.c
	gcc client.c -o client

helloworld: helloworld.c
	gcc helloworld.c -o helloworld

sleep: sleep.c
	gcc sleep.c -o sleep

clean:
	rm client server helloworld sleep