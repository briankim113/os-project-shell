#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "functions.h"

#define maxInput 150
#define cmdListLen 3

char* cmdList[cmdListLen] = {
    "ls",
    "pwd",
    "wc"
};

/*
prints welcome messages
*/
void welcome(void){
    printf("Welcome to the main shell!\n");
    printf("If you wish to exit the shell, use 'exit'\n");
}

/*
checks if the user input is all whitespace
and also replaces the last character from '\n' to '\0'
*/
int isInputEmpty(char *str){
    printf(">>> ");

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

/*
run the appropriate command
*/
void runCommand(int commandNum, char* args[]){
    switch (commandNum) {
        case 0:
            lsCommand();
            break;
        case 1:
            pwdCommand();
            break;
        case 2:
            printf("wc call\n");
            //add current pwd and inputName to get the file
            //but what happens if user actually provides the absolute path?

            char path[maxInput];
            getcwd(path, maxInput);
            // printf("%s\n", path);
            // printf("%s\n", commandParse[1]);
            
            char name_with_extension[maxInput];
            strcpy(name_with_extension, path); /* copy name into the new var */
            strcat(name_with_extension, "/"); /* add the extension */
            strcat(name_with_extension, args[1]);
            printf("%s\n", name_with_extension);
            wcCommand(name_with_extension);
            
            break;
        default: //when -1 is called
            printf("Command not recognized, please try again\n");
            break;
    }
}

int main(){
    welcome();
    char inputString[maxInput];

    while (1){
        if (isInputEmpty(inputString))
            continue; //continue asking for actual input
        
        if (strcmp(inputString, "exit") == 0) {
            printf("goodbye!\n");
            break; //stop the shell
        }

        //otherwise decode user input and run command

        //number of commands that are piped into a single line
        char* pipeCommands[5]; //max five commands (4 pipes) for now
        int commandCount;
        inputDecode(pipeCommands, 5, inputString, &commandCount);

        //for each command, parse the arguments
        for (int i=0; i<commandCount; i++){
            // printf("%d\t%s\n", i, pipeCommands[i]);

            char* args[5]; //max five arguments for now, example: [touch, mytext.txt]
            int cmdNum = parseCommand(args, 5, pipeCommands[i]); //is not working when I try with pipes

            // printf("%d\t%s\n", cmdNum, args[0]);

            //we should either fork here... or outside of the for loop?
            //call the functions using cmdNum and pass any arguments we might have
            runCommand(cmdNum, args);
        }
    }

    return 0;
}