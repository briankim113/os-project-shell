#include "functions.h"

void lsCommand()
{
    execlp("ls", "ls", 0, NULL);
    perror("lsCommand"); //if we reached here, there is an error so we must exit
    exit(EXIT_FAILURE);
}

void pwdCommand()
{
    execlp("pwd", "pwd", 0, NULL);
    perror("pwdCommand"); //if we reached here, there is an error so we must exit
    exit(EXIT_FAILURE);
}

void wcCommand(const char *inputName)
{
    execlp("wc", "wc", inputName, NULL);
    perror("wcCommand"); //if we reached here, there is an error so we must exit
    exit(EXIT_FAILURE);
}


void psCommand(){
    execlp("ps", "ps", 0, NULL);
    perror("psCommand"); //if we reached here, there is an error so we must exit
    exit(EXIT_FAILURE);
}

void topCommand(){
    execlp("top", "top", 0, NULL);
    perror("topCommand"); //if we reached here, there is an error so we must exit
    exit(EXIT_FAILURE);
}

void echoCommand(const char* args){
    execlp("echo", "echo", args, NULL);
    perror("echoCommand");
    exit(EXIT_FAILURE);
}

void touchCommand(const char* fileName){
    execlp("touch", "touch", fileName, NULL);
    perror("touchCommand");
    exit(EXIT_FAILURE);
}

void lessCommand(char* const* inputArg)
{
    execvp("less", inputArg);
    perror("lessCommand"); //if we reached here, there is an error so we must exit
    exit(EXIT_FAILURE);
}

void grepCommand(char* const* inputArg)
{
    execvp("grep", inputArg);
    perror("grepCommand"); //if we reached here, there is an error so we must exit
    exit(EXIT_FAILURE);
}

