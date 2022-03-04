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
intputRedirect / outputRedirect: 1 if true, 0 if false
*/
void runCommand(int commandNum, char* args[], int inputRedirect, int outputRedirect, char* filename){
    pid_t child = fork();
    if (child < 0){ //fork failed
        perror("Failed fork child");
        exit(EXIT_FAILURE);
    }
    else { //fork succeeded
        if (child == 0) { //currently child
            //redirection - https://stackoverflow.com/questions/2605130/redirecting-exec-output-to-a-buffer-or-file
            
            printf("%s %d %d %d %s\n", args[0], commandNum, inputRedirect, outputRedirect, filename);
            int saved_stdout = dup(1);
            int saved_stdin = dup(0);

            if (outputRedirect) {
                // printf("outputRedirect\n");

                //https://stackoverflow.com/questions/230062/whats-the-best-way-to-check-if-a-file-exists-in-c
                if (access( filename, F_OK ) != 0 ) {
                    // if file doesn't exist, we should create the file
                    FILE * fp;
                    fp = fopen(filename, "w");
                    fclose(fp);
                }

                int fd = open(filename, O_WRONLY); //write
                if (fd == -1) {
                    perror("open error for outputRedirect");
                    exit(EXIT_FAILURE);
                }

                dup2(fd, 1);   // make stdout go to file
                close(fd);     // fd no longer needed - the dup'ed handles are sufficient                
            }

            else if (inputRedirect) {
                // printf("inputRedirect\n");

                //if file doesn't exist, we should NOT create it
                int fd = open(filename, O_RDONLY); //read
                if (fd == -1) {
                    perror("open error for inputRedirect");
                    exit(EXIT_FAILURE);
                }

                dup2(fd, 0);   // make stdin go to file
                close(fd);     // fd no longer needed - the dup'ed handles are sufficient
            }

            switch (commandNum) {
                case 0:
                    lsCommand();
                    break;
                case 1:
                    pwdCommand();
                    break;
                case 2:
                    wcCommand(args);
                    break;
                case 3:
                    psCommand();
                    break;
                case 4:
                    topCommand();
                    break;
                case 5:
                    echoCommand(args);
                    break;
                case 6:
                    touchCommand(args);
                    break;
                case 7:
                    lessCommand(args);
                    break;
                case 8:
                    grepCommand(args);
                    break;
                case 9:
                    catCommand(args);
                    break;
                default:
                    break;
            }

            //undo dup2()
            dup2(saved_stdout, 1);
            dup2(saved_stdin, 0);

        } //end of child
        else { //parent
            wait(NULL);
        }
    } //end of fork
}

int main(){
    welcome();
    char inputString[maxInput];
    char filename[maxInput];
    int inputRedirect = 0, outputRedirect = 0;

    while (1){
        if (isInputEmpty(inputString))
            continue; //continue asking for actual input
        
        if (strcmp(inputString, "exit") == 0) {
            printf("goodbye!\n");
            break; //stop the shell
        }

        //otherwise decode user input and run command

        //first detect redirection
        redirection(inputString, &inputRedirect, &outputRedirect, filename);

        // printf("%d %d %s %s\n", inputRedirect, outputRedirect, inputString, filename);
        // printf("%s\n", filename);

        //number of commands that are piped into a single line
        int maxCommands = 4;
        char* pipeCommands[maxCommands]; //max four commands (3 pipes) for now
        int commandCount;
        inputDecode(pipeCommands, maxCommands, inputString, &commandCount);

        //for each command, parse the arguments
        for (int i=0; i<commandCount; i++){
            // printf("%d\t%s\n", i, pipeCommands[i]);

            char* args[5]; //max five arguments for now, example: [touch, mytext.txt]
            int cmdNum = parseCommand(args, 5, pipeCommands[i]); //is not working when I try with pipes

            // printf("%d\t%s\n", cmdNum, args[0]);

            //call the functions using cmdNum and pass any arguments we might have
            if (cmdNum == -1) { //-1 means this command is not valid
                printf("Command not recognized, please try again\n");
                break;
            }
            runCommand(cmdNum, args, inputRedirect, outputRedirect, filename);
        }
    }

    return 0;
}