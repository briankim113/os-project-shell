#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "functions.h"

#define maxInput 150

void welcome(void){
    printf("Welcome to the main shell!\n");
    printf("If you wish to exit the shell, use 'exit'\n");
}

int isInputEmpty(char *str){
    printf(">>> ");

    char buf[maxInput];
    fgets(buf, maxInput, stdin);
    
    if (*buf == '\n') { 
        return 1;
    }
    else { 
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

// Determine the command and call the command
void singleCommand(char *command)
{
    // printf("%s", command);
    // commandParse Example [touch, text]

    // To-Do: should we free up the char* every time we call it at the end of function?
    char* commandParse[5];
    for (int i = 0; i < 5; i++)
    {
        commandParse[i] = strsep(&command, " ");
        if (commandParse[i] == NULL){
            // printf("%d\n", i);
            // printf("%s\n",commandParse[i-1]);
            break;}
    }
    int commandLength = 3;
    char* commandList[commandLength];
    int commandNum = 0;
    commandList[0] = "ls\n";
    commandList[1] = "pwd\n";
    commandList[2] = "wc";
    //need to figure out a way to get rid of \n and create a \0 null character for all arguments

    
    for (int i = 0; i < commandLength; i++) {
        // printf("command: %s, commandList: %s\n", commandParse[0],commandList[i]);
        if (strcmp(commandParse[0], commandList[i]) == 0) {
            commandNum=i+1;
            break;
        }
    }

    // Call appropriate command
    switch (commandNum) {
        case 1:
            // printf("ls call");
            lsCommand();
            break;
        case 2:
            // printf("pwd call");
            pwdCommand();
            break;
        case 3:
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
            strcat(name_with_extension, commandParse[1]);
            name_with_extension[strlen(name_with_extension)-1] = '\0'; //get rid of newline char and use \0
            printf("%s", name_with_extension);
            sleep(3);
            wcCommand(name_with_extension);
            
            break;
        default:
            // printf("Out of range");
            break;
    }
}

void inputDecode(char *inputString)
{
    // Create an array of commands separated by pipes
    // Count the number of pipes
    char* pipedCommands[5];
    int pipeCount;
    int i;
    for (i = 0; i < 5; i++)
    {
        pipedCommands[i] = strsep(&inputString, "|");
        if (pipedCommands[i] == NULL){
            pipeCount = i-1;
            break;}
    }
    if (pipeCount==0){
        singleCommand(pipedCommands[0]);
    }    
}



int main()
{
    welcome();
    char inputString[maxInput];

    while (1){
        if (isInputEmpty(inputString))
            continue; //continue asking for actual input
        
        //decode user input
        inputDecode(inputString);
        
        //exit is giving a weird behavior sometimes
        if (strcmp(inputString, "exit\n") == 0) {
            printf("goodbye!\n");
            return 0;
        }
    }
}