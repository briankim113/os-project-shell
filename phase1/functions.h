//libraries
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <ctype.h>

//variables
#define maxInput 150
#define cmdListLen 10
#define maxCmds 5
#define maxArgs 5

//inputHandling.c
int isInputEmpty(char*);
void redirection(char*, int*, int*, char*);
void inputDecode(char* [], size_t, char*, int*);
int parseCommand(char* [], size_t, char*);

//commands.c
void lsCommand(char* const*);
void pwdCommand(char* const*);
void wcCommand(char* const*);
void psCommand(char* const*);
void topCommand(char* const*);
void echoCommand(char* const*);
void touchCommand(char* const*);
void lessCommand(char* const*);
void grepCommand(char* const*);
void catCommand(char* const*);