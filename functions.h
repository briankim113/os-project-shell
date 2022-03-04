//libraries
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

//variables
#define maxInput 150
#define cmdListLen 10

//inputHandling.c
int isInputEmpty(char*);
void redirection(char*, int*, int*);
void inputDecode(char* [], size_t, char*, int*);
int parseCommand(char* [], size_t, char*);

//commands.c
void lsCommand();
void pwdCommand();
void wcCommand(char* const*);
void psCommand();
void topCommand();
void echoCommand();
void touchCommand(char* const*);
void lessCommand(char* const*);
void grepCommand(char* const*);
void catCommand(char* const*);