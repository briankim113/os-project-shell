#include "functions.h"

void lsCommand(){
    pid_t child = fork();
    if (child < 0){
        perror("Failed fork child");
        exit(EXIT_FAILURE);
    }

    else { //fork child succeeded
        if (child == 0) { //currently child
            execl("/bin/ls", "ls", 0, NULL);
            perror("lsCommand"); //if we reached here, there is an error so we must exit
            exit(EXIT_FAILURE);
        }
        else {
            wait(NULL);
        }
    }
    return;
}

void pwdCommand(){
    pid_t child = fork();
    if (child < 0){
        perror("Failed fork child");
        exit(EXIT_FAILURE);
    }

    else { //fork child succeeded
        if (child == 0) { //currently child
            execl("/bin/pwd", "pwd", 0, NULL);
            perror("pwdCommand"); //if we reached here, there is an error so we must exit
            exit(EXIT_FAILURE);
        }
        else {
            wait(NULL);
        }
    }
    return;
}

void wcCommand(const char* inputName){
    pid_t child = fork();
    if (child < 0){
        perror("Failed fork child");
        exit(EXIT_FAILURE);
    }

    else { //fork child succeeded
        if (child == 0) { //currently child
            execl("/bin/wc", "wc", inputName, NULL);
            perror("wcCommand"); //if we reached here, there is an error so we must exit
            exit(EXIT_FAILURE);
        }
        else {
            wait(NULL);
        }
    }
    return;
}