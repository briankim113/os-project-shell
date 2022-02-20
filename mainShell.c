#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/wait.h>
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

int inputDecode(char *inputString){

}

int main()
{

    char inputString[maxInput];
    while(1)
    {
        welcome();

        if (takeInput(inputString))
            continue;
        
        decode(inputString);
        printf("hi\n");
        printf("%s",inputString);

        lsCommand();

        

        //the arguments that you want to pass to the executable followed by NULL.
        //NULL signifies end of the srgument list
        printf("\nProgram Terminated\n");
        return 0;
    }
}