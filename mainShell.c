#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "functions.h"

#define maxInput 50

void welcome(void){
    printf("Welcome to the main shell!\n");
}

int isInputEmpty(char *str){
    printf(">>> ");

    char buf[maxInput];
    fgets(buf, maxInput, stdin);
    
    if (*buf == '\n') { //just newline char
        return 1;
    }
    else { //check if all whitespace
        char *tmp = buf;
        while (*tmp != '\n') {
            if (*tmp != ' ') {
                strcpy(str, buf);
                return 0;
            }
            tmp++;
        }
        return 1;
    }
}

int main(){
    welcome();
    char inputString[maxInput];

    while (1){
        if (isInputEmpty(inputString))
            continue; //continue asking for actual input
        
        // decode(inputString);
        printf("%s", inputString);

        // lsCommand();
        pwdCommand();
        

        //the arguments that you want to pass to the executable followed by NULL.
        //NULL signifies end of the srgument list
        printf("\nProgram Terminated\n");
        return 0;
    }
}