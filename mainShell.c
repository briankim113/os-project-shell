#include "functions.h"

/*
prints welcome messages
*/
void welcome()
{
    printf("Welcome to the main shell!\n");
    printf("If you wish to exit the shell, use 'exit'\n");
}

/*

*/
void switchCommand(int commandNum, char *args[])
{
    switch (commandNum)
    {
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
}

/*
run the appropriate command
intputRedirect / outputRedirect: 1 if true, 0 if false
*/
void singleCommand(char *pipeCommand, int inputRedirect, int outputRedirect, char *filename)
{
    char *args[maxArgs];
    int commandNum = parseCommand(args, maxArgs, pipeCommand);

    if (commandNum == -1)
    { //-1 means this command is not valid
        printf("Command not recognized, please try again\n");
        return;
    }

    // pid_t child = fork();
    // if (child < 0)
    // { //fork failed
    //     perror("Failed fork child");
    //     exit(EXIT_FAILURE);
    // }
    // else
    // { //fork succeeded
    //     if (child == 0)
    //     { //currently child
    //redirection - https://stackoverflow.com/questions/2605130/redirecting-exec-output-to-a-buffer-or-file

    // printf("%s %d %d %d %s\n", args[0], commandNum, inputRedirect, outputRedirect, filename);
    int saved_stdout = dup(1);
    int saved_stdin = dup(0);

    if (outputRedirect)
    {
        // printf("outputRedirect\n");

        //https://stackoverflow.com/questions/230062/whats-the-best-way-to-check-if-a-file-exists-in-c
        if (access(filename, F_OK) != 0)
        {
            // if file doesn't exist, we should create the file
            FILE *fp;
            fp = fopen(filename, "w");
            fclose(fp);
        }

        int fd = open(filename, O_WRONLY); //write
        if (fd == -1)
        {
            perror("open error for outputRedirect");
            exit(EXIT_FAILURE);
        }

        dup2(fd, 1); // make stdout go to file
        close(fd);   // fd no longer needed - the dup'ed handles are sufficient
    }

    else if (inputRedirect)
    {
        // printf("inputRedirect\n");

        //if file doesn't exist, we should NOT create it
        int fd = open(filename, O_RDONLY); //read
        if (fd == -1)
        {
            perror("open error for inputRedirect");
            exit(EXIT_FAILURE);
        }

        dup2(fd, 0); // make stdin go to file
        close(fd);   // fd no longer needed - the dup'ed handles are sufficient
    }

    switchCommand(commandNum, args);

    //undo dup2()
    if (outputRedirect)
    {
        dup2(saved_stdout, 1);
    }
    else if (inputRedirect)
    {
        dup2(saved_stdin, 0);
    }

    //     } //end of child
    //     else
    //     { //parent
    //         wait(NULL);
    //     }
    // } //end of fork
}

int main()
{
    welcome();
    char inputString[maxInput];
    char filename[maxInput];
    int inputRedirect = 0, outputRedirect = 0;

    while (1)
    {
        if (isInputEmpty(inputString))
            continue; //continue asking for actual input

        if (strcmp(inputString, "exit") == 0)
        {
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
        char *pipeCommands[maxCommands]; //max four commands (3 pipes) for now
        int commandCount;
        inputDecode(pipeCommands, maxCommands, inputString, &commandCount);

        //source for fork() parent-child relationship: https://stackoverflow.com/questions/33884291/pipes-dup2-and-exec

        //we only have a single command - this is the only case where we do input/output redirection
        if (commandCount == 1){
            pid_t pid1 = fork();
            if (pid1 == -1){
                perror("fork failed");
                exit(EXIT_FAILURE);
            }

            //parent - runs exec
            if (pid1 == 0){ 
                singleCommand(pipeCommands[0], inputRedirect, outputRedirect, filename);
                exit(EXIT_SUCCESS);
            }

            //child - waits
            else { 
                int status;
                waitpid(pid1, &status, 0);
            }

        }

        else if (commandCount == 2){
            // Pipes start
            int pipefds_1[2];
            if (pipe(pipefds_1) == -1){
                perror("pipe failed");
                exit(EXIT_FAILURE);
            }

            pid_t pid1 = fork();
            if (pid1 == -1){
                perror("fork failed");
                exit(EXIT_FAILURE);
            }

            //parent - runs 1st exec
            if (pid1 == 0){
                dup2(pipefds_1[1], STDOUT_FILENO); //replace stdout with the write end of the pipe
                close(pipefds_1[0]); //close read to pipe
                singleCommand(pipeCommands[0], inputRedirect, outputRedirect, filename);
                exit(EXIT_SUCCESS);
            }

            else {
                pid_t pid2 = fork();
                if (pid2 == -1){
                    perror("fork failed");
                    exit(EXIT_FAILURE);
                }

                //child1 - runs 2nd exec
                if (pid2 == 0) {
                    dup2(pipefds_1[0], STDIN_FILENO); //replace stdin with the read end of the pipe
                    close(pipefds_1[1]); //close write to pipe
                    singleCommand(pipeCommands[1], inputRedirect, outputRedirect, filename);
                    exit(EXIT_SUCCESS);
                }

                //child2 - waits
                else {
                    close(pipefds_1[0]);
                    close(pipefds_1[1]);
                    int status;
                    waitpid(pid2, &status, 0);
                }
            }
        }

        else if (commandCount == 3)
        {
        }

        else if (commandCount == 4)
        {
        }

        else
        {
            printf("Too many piped commands, please limit to 3 pipes\n");
            continue;
        }

        // for (int i=0; i<commandCount; i++){
        //     // printf("%d\t%s\n", i, pipeCommands[i]);

        //     char* args[5]; //max five arguments for now, example: [touch, mytext.txt]
        //     int cmdNum = parseCommand(args, 5, pipeCommands[i]); //is not working when I try with pipes

        //     // printf("%d\t%s\n", cmdNum, args[0]);

        //     //call the functions using cmdNum and pass any arguments we might have
        //     if (cmdNum == -1) { //-1 means this command is not valid
        //         printf("Command not recognized, please try again\n");
        //         break;
        //     }
        //     singleCommand(cmdNum, args, inputRedirect, outputRedirect, filename);
        // }
    }

    return 0;
}