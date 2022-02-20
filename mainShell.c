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
    printf("If you wish to exit the shell, use 'exit'\n");
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

int singleCommand(char *command)
{
    char* commandParse[5];
    for (int i = 0; i < 5; i++)
    {
        commandParse[i] = strsep(&command, " ");
        if (commandParse[i] == NULL){
            // printf("%d\n", i);
            // printf("%s\n",commandParse[i-1]);
            break;}
    }
    int commandLength = 1;
    char* commandList[commandLength];
    int commandNum;
    commandList[0] = "ls";
    
    for (int i = 0; i < commandLength; i++) {
        // printf("command: %s\n commandList: %s\n", commandParse[0],commandList[i]);
        if (strcmp(commandParse[0], commandList[i]) == 0) {
            commandNum=i+1;
            break;
        }
    }
    switch (commandNum) {
            case 1:
                printf("ls call");
                // sleep(10);
                lsCommand();
                break;
            default:
                printf("Out of range");
                break;
        } 

}

int inputDecode(char *inputString)
{
    char* pipedCommands[5];
    int pipeCount;
    int i;
    for (i = 0; i < 5; i++)
    {
        pipedCommands[i] = strsep(&inputString, "|");
        if (pipedCommands[i] == NULL){
            pipeCount = i-1;
            // printf("%d\n", i);
            // printf("%s\n",pipedCommands[i-1]);
            break;}
    }
    if (pipeCount==0){
        singleCommand(pipedCommands[0]);
    }
     // returns zero if no pipe is found.
    
}



int main()
{
    welcome();
    char inputString[maxInput];

    while (1){
        if (isInputEmpty(inputString))
            continue; //continue asking for actual input
        
        inputDecode(inputString);
        printf("%s", inputString);
        
        if (strcmp(inputString, "exit\n") == 0) {
            printf("goodbye!\n");
            return 0;
        }
    }
}