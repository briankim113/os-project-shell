//libraries
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

//variables
#define maxInput 150
#define cmdListLen 3

//inputHandling.c
int isInputEmpty(char*);
void inputDecode(char* [], size_t, char*, int*);
int parseCommand(char* [], size_t, char*);

//commands.c
void lsCommand();
void pwdCommand();
void wcCommand(const char*);