#include "functions.h"

char* cmdList[cmdListLen] = {"ls", "pwd", "wc", "ps", "top", "echo", "touch", "less", "grep", "cat"};

/*
strip leading and trailing whitespaces
for stripLeadWhiteSpace, we need to pass the actual & to the char*, hence char**
for stripTrailWhiteSpace, we simply pass char*
*/
void stripLeadWhiteSpace(char** str){
    //lead whitespace - https://stackoverflow.com/questions/9713180/function-to-remove-leading-whitespace-does-not-change-string-in-caller
    int i;
    for (i = 0; (*str)[i] == ' '; i++) {}
    *str = *str + i;
}

void stripTrailWhiteSpace(char* str){
    //trail whitespace
    int i = strlen(str)-1;
    while (str[i] == ' ') i--;
    str[i+1] = '\0';
}

/*
detect redirection, set redirect integers to 1 if true
*/
void redirection(char* inputString, int* inputRedirect, int* outputRedirect, char* filename){
    char* outputPtr = strchr(inputString, '>');
    char* inputPtr = strchr(inputString, '<');

    char* temp = inputString;

    if (outputPtr != NULL){ //output exists, input does NOT
        *outputRedirect = 1;
        *inputRedirect = 0;

        //now inputString only contains the command and temp contains only the filename
        inputString = strsep(&temp, ">");

        //get rid of whitespace from temp and inputString
        stripLeadWhiteSpace(&temp);
        stripLeadWhiteSpace(&inputString);
        stripTrailWhiteSpace(temp);
        stripTrailWhiteSpace(inputString);

        //copy temp content into filename
        strcpy(filename, temp);
    }

    else if (inputPtr != NULL){ //input exists, output does NOT
        *inputRedirect = 1;
        *outputRedirect = 0;

        //now inputString only contains the command and temp contains only the filename
        inputString = strsep(&temp, "<");

        //get rid of whitespace from temp and inputString
        stripLeadWhiteSpace(&temp);
        stripLeadWhiteSpace(&inputString);
        stripTrailWhiteSpace(temp);
        stripTrailWhiteSpace(inputString);

        //copy temp content into filename
        strcpy(filename, temp);
    }

    else { //both does not exist
        *inputRedirect = 0;
        *outputRedirect = 0;
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
        stripLeadWhiteSpace(&commands[i]);
        stripTrailWhiteSpace(commands[i]);
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