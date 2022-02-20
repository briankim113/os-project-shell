#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "functions.h"

// To-Do: commands all require a fork b/c exec exits completely upon successs

void lsCommand(void)
{
    pid_t child = fork();
    if (child < 0){
        perror("Failed fork child");
        exit(EXIT_FAILURE);
    }

    else { //fork child succeeded
        if (child == 0) { //currently child
            execl("/bin/ls", "ls", 0, NULL); //exits
        }
        else {
            wait(NULL);
        }
    }
    return;
}

void pwdCommand(void)
{
    pid_t child = fork();
    if (child < 0){
        perror("Failed fork child");
        exit(EXIT_FAILURE);
    }

    else { //fork child succeeded
        if (child == 0) { //currently child
            execl("/bin/pwd", "pwd", 0, NULL);
        }
        else {
            wait(NULL);
        }
    }
    return;
}