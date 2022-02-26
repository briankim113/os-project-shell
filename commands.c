#include "functions.h"

void lsCommand(){
    execl("/bin/ls", "ls", 0, NULL);
    perror("lsCommand"); //if we reached here, there is an error so we must exit
    exit(EXIT_FAILURE);
}

void pwdCommand(){
    execl("/bin/pwd", "pwd", 0, NULL);
    perror("pwdCommand"); //if we reached here, there is an error so we must exit
    exit(EXIT_FAILURE);
}

void wcCommand(const char* inputName){
    execl("/usr/bin/wc", "wc", inputName, NULL);
    perror("wcCommand"); //if we reached here, there is an error so we must exit
    exit(EXIT_FAILURE);
}