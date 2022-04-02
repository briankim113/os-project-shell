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
checks the commandNum and calls the appropriate single command with args
*/
void switchCommand(int commandNum, char *args[])
{
    switch (commandNum)
    {
    case 0: lsCommand(args); break;
    case 1: pwdCommand(args); break;
    case 2: wcCommand(args); break;
    case 3: psCommand(args); break;
    case 4: topCommand(args); break;
    case 5: echoCommand(args); break;
    case 6: touchCommand(args); break;
    case 7: lessCommand(args); break;
    case 8: grepCommand(args); break;
    case 9: catCommand(args); break;
    default: break;
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

    //-1 means this command is not valid
    if (commandNum == -1)
    { 
        printf("Command not recognized, please try again\n");
        return;
    }

    //redirection - https://stackoverflow.com/questions/2605130/redirecting-exec-output-to-a-buffer-or-file
    int saved_stdout = dup(1);
    int saved_stdin = dup(0);

    if (outputRedirect)
    {
        FILE *fp;
        fp = fopen(filename, "w+"); // w+ for overwrite
        fclose(fp);

        int fd = open(filename, O_WRONLY); //write-only
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
        //if file doesn't exist, we should NOT create it
        int fd = open(filename, O_RDONLY); //read-only
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
}

void shell(char* inputString, int* inputRedirect, int *outputRedirect, char* filename){
    //first detect redirection '<' or '>'
        redirection(inputString, inputRedirect, outputRedirect, filename);

        char *pipeCommands[maxCmds]; //max four commands (3 pipes)
        int commandCount = 0;
        inputDecode(pipeCommands, maxCmds, inputString, &commandCount);

        //source for fork() parent-child relationship: https://stackoverflow.com/questions/33884291/pipes-dup2-and-exec

        //we only have a single command - this is the only case where we do input/output redirection
        if (commandCount == 1)
        {
            int pid0 = fork();
            if (pid0 == -1)
            {
                perror("fork failed");
                exit(EXIT_FAILURE);
            }

            //run exec and exit
            if (pid0 == 0)
            {
                singleCommand(pipeCommands[0], *inputRedirect, *outputRedirect, filename);
            }
            
            waitpid(pid0, NULL, 0);
            //done with exec
        }

        //1 pipe, 2 commands
        else if (commandCount == 2)
        {
            int pipe1[2];
            int pid0, pid1;

            pipe(pipe1);
            pid0 = fork();

            if (pid0 == -1)
            {
                perror("fork failed");
                exit(EXIT_FAILURE);
            }

            //run 1st exec and exit
            if (pid0 == 0)
            {
                close(pipe1[0]); // close the read end of pipe1
                dup2(pipe1[1], 1); // redirect stdout to the write end of pipe1
                singleCommand(pipeCommands[0], *inputRedirect, *outputRedirect, filename);
            }

            pid1 = fork();

            if (pid1 == -1)
            {
                perror("fork failed");
                exit(EXIT_FAILURE);
            }

            //run 2nd exec and exit
            if (pid1 == 0)
            {
                close(pipe1[1]); // close the write end of pipe1
                dup2(pipe1[0], 0); // redirect stdin to the read end of pipe1
                singleCommand(pipeCommands[1], *inputRedirect, *outputRedirect, filename);
            }

            close(pipe1[0]);
            close(pipe1[1]);
            waitpid(pid0, NULL, 0);
            waitpid(pid1, NULL, 0);
            //done with both exec
        }

        else if (commandCount == 3)
        {
            int pipe1[2], pipe2[2];
            int pid0, pid1, pid2;

            pipe(pipe1);
            pid0 = fork();

            if (pid0 == -1)
            {
                perror("fork failed");
                exit(EXIT_FAILURE);
            }

            //run 1st exec and exit
            if (pid0 == 0)
            {
                close(pipe1[0]); // close the read end of pipe1
                dup2(pipe1[1], 1); // redirect stdout to the write end of pipe1
                singleCommand(pipeCommands[0], *inputRedirect, *outputRedirect, filename);
            }

            pipe(pipe2);
            pid1 = fork();

            if (pid1 == -1)
            {
                perror("fork failed");
                exit(EXIT_FAILURE);
            }

            //run 2nd exec and exit
            if (pid1 == 0)
            {
                close(pipe1[1]); // close the write end of pipe1
                close(pipe2[0]); // close the read end of pipe2
                dup2(pipe1[0], 0); // redirect stdin to the read end of pipe1
                dup2(pipe2[1], 1); // redirect stdout to the write end of pipe2
                singleCommand(pipeCommands[1], *inputRedirect, *outputRedirect, filename);
            }

            pid2 = fork();

            if (pid2 == -1)
            {
                perror("fork failed");
                exit(EXIT_FAILURE);
            }

            //run 3rd exec and exit
            if (pid2 == 0)
            {
                close(pipe1[0]); // close unused pipe1 for both ends
                close(pipe1[1]); 
                close(pipe2[1]); // close the write end of pipe2
                dup2(pipe2[0], 0); // redirect stdin to the read end of pipe2
                singleCommand(pipeCommands[2], *inputRedirect, *outputRedirect, filename);
            }
            
            close(pipe1[0]);
            close(pipe1[1]);
            close(pipe2[0]);
            close(pipe2[1]);
            waitpid(pid0, NULL, 0);
            waitpid(pid1, NULL, 0);
            waitpid(pid2, NULL, 0);
            //done with all three exec
        }

        else if (commandCount == 4)
        {
            int pipe1[2], pipe2[2], pipe3[2];
            int pid0, pid1, pid2, pid3;

            pipe(pipe1);
            pid0 = fork();

            if (pid0 == -1)
            {
                perror("fork failed");
                exit(EXIT_FAILURE);
            }

            if (pid0 == 0)
            {
                close(pipe1[0]); // close the read end of pipe1
                dup2(pipe1[1], 1); // redirect stdout to the write end of pipe1
                singleCommand(pipeCommands[0], *inputRedirect, *outputRedirect, filename);
            }

            pipe(pipe2);
            pid1 = fork();

            if (pid1 == -1)
            {
                perror("fork failed");
                exit(EXIT_FAILURE);
            }

            if (pid1 == 0)
            {
                close(pipe1[1]); // close the write end of pipe1
                close(pipe2[0]); // close the read end of pipe2
                dup2(pipe1[0], 0); // redirect stdin to the read end of pipe1
                dup2(pipe2[1], 1); // redirect stdout to the write end of pipe2
                singleCommand(pipeCommands[1], *inputRedirect, *outputRedirect, filename);
            }

            pipe(pipe3);
            pid2 = fork();

            if (pid2 == -1)
            {
                perror("fork failed");
                exit(EXIT_FAILURE);
            }

            if (pid2 == 0)
            {
                close(pipe1[0]); // close unused pipe1 for both ends
                close(pipe1[1]);
                close(pipe2[1]); // close the write end of pipe2
                close(pipe3[0]); // close the read end of pipe3
                dup2(pipe2[0], 0); // redirect stdin to the read end of pipe2
                dup2(pipe3[1], 1); // redirect stdout to the write end of pipe3
                singleCommand(pipeCommands[2], *inputRedirect, *outputRedirect, filename);
            }

            pid3 = fork();

            if (pid3 == -1)
            {
                perror("fork failed");
                exit(EXIT_FAILURE);
            }

            if (pid3 == 0)
            {
                close(pipe1[0]); // close unused pipe1 for both ends
                close(pipe1[1]);
                close(pipe2[0]); // close unused pipe2 for both ends
                close(pipe2[1]);
                close(pipe3[1]); // close the write end of pipe3
                dup2(pipe3[0], 0); // redirect stdin to the read end of pipe3
                singleCommand(pipeCommands[3], *inputRedirect, *outputRedirect, filename);
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
            //done with all four exec
        }

        else //commandCount is not modified from inputDecode and is still 0
        {
            printf("Too many piped commands, please limit to 3 pipes\n");
            // continue;
        }
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

        shell(inputString, &inputRedirect, &outputRedirect, filename);
        //otherwise decode user input and run command
    }

    return 0;
}