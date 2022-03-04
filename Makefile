all: mainShell.c commands.c inputHandling.c
	gcc mainShell.c commands.c inputHandling.c -o shell
	# clearS
	
# mainShell: mainShell.c
# 	gcc mainShell.c -o mainShell

# commands: commands.c
# 	gcc commands.c -o commands

clean:
	rm *.o shell