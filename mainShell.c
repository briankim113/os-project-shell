#include "functions.h"

/*
prints welcome messages
*/
void welcome(void){
    printf("Welcome to the main shell!\n");
    printf("If you wish to exit the shell, use 'exit'\n");
}

/*
run the appropriate command
*/
void runCommand(int commandNum, char* args[]){
    pid_t child = fork();
    if (child < 0){ //fork failed
        perror("Failed fork child");
        exit(EXIT_FAILURE);
    }
    else { //fork succeeded
        if (child == 0) { //currently child
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
                // case 3:
                //     // char* arr = args.Skip(1).ToArray();  
                //     grepCommand(args);
                //     break;
                default: //when -1 is called
                    printf("Command not recognized, please try again\n");
                    break;
            }
        } //end of child
        else { //parent
            wait(NULL);
        }
    } //end of fork
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