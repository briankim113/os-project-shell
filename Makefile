all: mainShell.c commands.c
	gcc mainShell.c commands.c -o shell
	
# mainShell: mainShell.c
# 	gcc mainShell.c -o mainShell

# commands: commands.c
# 	gcc commands.c -o commands

clean:
	rm shell