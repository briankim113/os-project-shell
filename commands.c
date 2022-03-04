#include "functions.h"

void lsCommand(char* const* inputArg){
    execvp("ls", inputArg);
    perror("lsCommand"); //if we reached here, there is an error so we must exit
    exit(EXIT_FAILURE);
}

void pwdCommand(char* const* inputArg){
    execvp("pwd", inputArg);
    perror("pwdCommand"); //if we reached here, there is an error so we must exit
    exit(EXIT_FAILURE);
}

void wcCommand(char* const* inputArg){
    execvp("wc", inputArg);
    perror("wcCommand"); //if we reached here, there is an error so we must exit
    exit(EXIT_FAILURE);
}

void psCommand(char* const* inputArg){
    execvp("ps", inputArg);
    perror("psCommand"); //if we reached here, there is an error so we must exit
    exit(EXIT_FAILURE);
}

void topCommand(char* const* inputArg){
    execvp("top", inputArg);
    perror("topCommand"); //if we reached here, there is an error so we must exit
    exit(EXIT_FAILURE);
}

void echoCommand(char* const* inputArg){
    execvp("echo", inputArg);
    perror("echoCommand");
    exit(EXIT_FAILURE);
}

void touchCommand(char* const* inputArg){
    execvp("touch", inputArg);
    perror("touchCommand");
    exit(EXIT_FAILURE);
}

void lessCommand(char* const* inputArg){
    execvp("less", inputArg);
    perror("lessCommand"); //if we reached here, there is an error so we must exit
    exit(EXIT_FAILURE);
}

void grepCommand(char* const* inputArg){
    execvp("grep", inputArg);
    perror("grepCommand"); //if we reached here, there is an error so we must exit
    exit(EXIT_FAILURE);
}

void catCommand(char* const* inputArg){
    execvp("cat", inputArg);
    perror("catCommand");
    exit(EXIT_FAILURE);
}
