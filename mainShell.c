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
        FILE *fp;
        fp = fopen(filename, "w+"); // w+ for overwrite
        fclose(fp);

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
        int maxCommands = 5;
        char *pipeCommands[maxCommands]; //max four commands (3 pipes) for now
        int commandCount;
        inputDecode(pipeCommands, maxCommands, inputString, &commandCount);

        //source for fork() parent-child relationship: https://stackoverflow.com/questions/33884291/pipes-dup2-and-exec

        //we only have a single command - this is the only case where we do input/output redirection
        if (commandCount == 1)
        {
            pid_t pid1 = fork();
            if (pid1 == -1)
            {
                perror("fork failed");
                exit(EXIT_FAILURE);
            }

            //parent - runs exec
            if (pid1 == 0)
            {
                singleCommand(pipeCommands[0], inputRedirect, outputRedirect, filename);
                exit(EXIT_SUCCESS);
            }

            //child - waits
            else
            {
                int status;
                waitpid(pid1, &status, 0);
            }
        }

        else if (commandCount == 2)
        {
            int pipe1[2], pipe2[2];
            int pid0, pid1, pid2;
            // char *argp = {"./addone", "first_argument", "second_argument", NULL};

            pipe(pipe1);
            pid0 = fork();
            if (pid0 == 0)
            {
                // close the read end of pipe1:
                close(pipe1[0]);
                // redirect stdout to the write end of pipe1:
                dup2(pipe1[1], 1);
                singleCommand(pipeCommands[0], inputRedirect, outputRedirect, filename);

                // execvp("./addone", argp);
            }

            pid1 = fork();
            if (pid1 == 0)
            {
                // close the write end of pipe1:
                close(pipe1[1]);
                // redirect stdin to the read end of pipe1:
                dup2(pipe1[0], 0);

                singleCommand(pipeCommands[1], inputRedirect, outputRedirect, filename);

            }


            
            close(pipe1[0]);
            close(pipe1[1]);
            waitpid(pid0, NULL, 0);
            waitpid(pid1, NULL, 0);
            // // Pipes start
            // int pipefds_1[2];
            // if (pipe(pipefds_1) == -1)
            // {
            //     perror("pipe failed");
            //     exit(EXIT_FAILURE);
            // }

            // pid_t pid1 = fork();
            // if (pid1 == -1)
            // {
            //     perror("fork failed");
            //     exit(EXIT_FAILURE);
            // }

            // //parent - runs 1st exec
            // if (pid1 == 0)
            // {
            //     dup2(pipefds_1[1], STDOUT_FILENO); //replace stdout with the write end of the pipe
            //     close(pipefds_1[0]);               //close read to pipe
            //     singleCommand(pipeCommands[0], inputRedirect, outputRedirect, filename);
            //     exit(EXIT_SUCCESS);
            // }

            // else
            // {
            //     pid_t pid2 = fork();
            //     if (pid2 == -1)
            //     {
            //         perror("fork failed");
            //         exit(EXIT_FAILURE);
            //     }

            //     //child1 - runs 2nd exec
            //     if (pid2 == 0)
            //     {
            //         dup2(pipefds_1[0], STDIN_FILENO); //replace stdin with the read end of the pipe
            //         close(pipefds_1[1]);              //close write to pipe
            //         singleCommand(pipeCommands[1], inputRedirect, outputRedirect, filename);
            //         exit(EXIT_SUCCESS);
            //     }

            //     //child2 - waits
            //     else
            //     {
            //         close(pipefds_1[0]);
            //         close(pipefds_1[1]);
            //         int status;
            //         waitpid(pid2, &status, 0);
            //     }
            // }
        }

        else if (commandCount == 3)
        {

            int pipe1[2], pipe2[2];
            int pid0, pid1, pid2;
            // char *argp = {"./addone", "first_argument", "second_argument", NULL};

            pipe(pipe1);
            pid0 = fork();
            if (pid0 == 0)
            {
                // close the read end of pipe1:
                close(pipe1[0]);
                // redirect stdout to the write end of pipe1:
                dup2(pipe1[1], 1);
                singleCommand(pipeCommands[0], inputRedirect, outputRedirect, filename);

                // execvp("./addone", argp);
            }

            pipe(pipe2);
            pid1 = fork();
            if (pid1 == 0)
            {
                // close the write end of pipe1:
                close(pipe1[1]);
                // close the read end of pipe2:
                close(pipe2[0]);
                // redirect stdin to the read end of pipe1:
                dup2(pipe1[0], 0);
                // redirect stdout to the write end of pipe2:
                dup2(pipe2[1], 1);

                singleCommand(pipeCommands[1], inputRedirect, outputRedirect, filename);

            }

            pid2 = fork();
            if (pid2 == 0)
            {
                // close unused pipe1:
                close(pipe1[0]);
                close(pipe1[1]);
                // close the write end of pipe2:
                close(pipe2[1]);
                // redirect stdin to the read end of pipe2:
                dup2(pipe2[0], 0);

                singleCommand(pipeCommands[2], inputRedirect, outputRedirect, filename);

            }

            
            close(pipe1[0]);
            close(pipe1[1]);
            close(pipe2[0]);
            close(pipe2[1]);
            waitpid(pid0, NULL, 0);
            waitpid(pid1, NULL, 0);
            waitpid(pid2, NULL, 0);
         
        }

        else if (commandCount == 4)
        {
            int pipe1[2], pipe2[2], pipe3[2];
            int pid0, pid1, pid2, pid3;
            // char *argp = {"./addone", "first_argument", "second_argument", NULL};

            pipe(pipe1);
            pid0 = fork();
            if (pid0 == 0)
            {
                // close the read end of pipe1:
                close(pipe1[0]);
                // redirect stdout to the write end of pipe1:
                dup2(pipe1[1], 1);
                singleCommand(pipeCommands[0], inputRedirect, outputRedirect, filename);

            }

            pipe(pipe2);
            pid1 = fork();
            if (pid1 == 0)
            {
                // close the write end of pipe1:
                close(pipe1[1]);
                // close the read end of pipe2:
                close(pipe2[0]);
                // redirect stdin to the read end of pipe1:
                dup2(pipe1[0], 0);
                // redirect stdout to the write end of pipe2:
                dup2(pipe2[1], 1);

                singleCommand(pipeCommands[1], inputRedirect, outputRedirect, filename);

            }

            pipe(pipe3);
            pid2 = fork();
            if (pid2 == 0)
            {
                // close unused pipe1:
                close(pipe1[0]);
                close(pipe1[1]);
                // close the write end of pipe2:
                close(pipe2[1]);
                // close the read end of pipe3:
                close(pipe3[0]);
                // redirect stdin to the read end of pipe2:
                dup2(pipe2[0], 0);
                // redirect stdout to the write end of pipe3:
                dup2(pipe3[1], 1);

                singleCommand(pipeCommands[2], inputRedirect, outputRedirect, filename);

            }

            pid3 = fork();
            if (pid3 == 0)
            {
                // close unused pipe1:
                close(pipe1[0]);
                close(pipe1[1]);

                // close unused pipe2:
                close(pipe2[0]);
                close(pipe2[1]);
                // close the write end of pipe3:
                close(pipe3[1]);
                // redirect stdin to the read end of pipe3:
                dup2(pipe3[0], 0);

                singleCommand(pipeCommands[3], inputRedirect, outputRedirect, filename);

            }

            
            close(pipe1[0]);
            close(pipe1[1]);
            close(pipe2[0]);
            close(pipe2[1]);
            close(pipe3[0]);
            close(pipe3[1]);
            waitpid(pid0, NULL, 0);
            waitpid(pid1, NULL, 0);
            waitpid(pid2, NULL, 0);
            waitpid(pid3, NULL, 0);
        }

        else
        {
            printf("Too many piped commands, please limit to 3 pipes\n");
            continue;
        }
    }

    return 0;
}