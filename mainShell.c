#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "functions.h"
// #include<readline/readline.h>
// #include<readline/history.h>
#define maxInput 50

void welcome(void)
{

    printf("Welcome to the main Shell!");
}

int takeInput(char *str)
{
    char buf[maxInput];

    printf("\n>>> ");
    scanf("%s", buf);
    // buf = readline("\n>>> ");
    printf("step1");
    if (strlen(buf) != 0)
    {
        strcpy(str, buf);
        return 0;
    }
    else
    {
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

    char inputString[maxInput];
    while (1)
    {
        welcome();

        if (takeInput(inputString))
            continue;

        inputDecode(inputString);
        // printf("hi\n");
        // printf("%s", inputString);

        // lsCommand();

        //the arguments that you want to pass to the executable followed by NULL.
        //NULL signifies end of the srgument list
        printf("\nProgram Terminated\n");
        return 0;
    }
}