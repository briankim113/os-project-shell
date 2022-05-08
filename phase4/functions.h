//libraries
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

// #include <sys/syscall.h>
// #define gettid() syscall(SYS_gettid)

//variables
#define maxInput 4096
#define cmdListLen 10
#define maxCmds 5
#define maxArgs 5
#define PORT 9002 //or 8080 or any other unused port value

//inputHandling.c
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