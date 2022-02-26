#include "functions.h"

char* cmdList[cmdListLen] = {
    "ls",
    "pwd",
    "wc",
    "ps",
    "top",
    "echo",
    "touch",
    "less",
    "grep",
    "cat"
};

/*
checks if the user input is all whitespace
and also replaces the last character from '\n' to '\0'
*/

int isInputEmpty(char *str){
    printf("\n>>> ");

    char buf[maxInput];
    fgets(buf, maxInput, stdin);
    buf[strlen(buf)-1] = '\0';
    
    if (*buf == '\0') { 
        return 1;
    }
    else {
        char *tmp = buf;
        while (*tmp != '\0') {
            if (*tmp != ' ') {
                strcpy(str, buf);
                return 0;
            }
            tmp++;
        }
        return 1;
    }
}

/*
fill the array with commands separated by pipes and count the number of commands
note that commandCount is number of pipes + 1
*/
void inputDecode(char* commands[], size_t size, char* input, int* commandCount){
    for (int i = 0; i < size; i++){
        commands[i] = strsep(&input, "|");
        if (commands[i] == NULL){
            *commandCount = i;
            break;
        }
    }
}

/*
parse a single command, fill the arguments array, and return the associated cmdNum
*/
int parseCommand(char* arguments[], size_t size, char* command){
    int cmdNum = -1; //none for now

    //comdNum
    arguments[0] = strsep(&command, " ");
    for (int i = 0; i < cmdListLen; i++) {
        if (strcmp(arguments[0], cmdList[i]) == 0) {
            cmdNum = i;
            break;
        }
    }
    
    // example arguments[]: [touch, text.txt]
    for (int i = 1; i < size; i++){
        arguments[i] = strsep(&command, " ");
        if (arguments[i] == NULL){
            break;
        }
    }

    return cmdNum;
}