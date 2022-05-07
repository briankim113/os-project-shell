#include "functions.h"

struct Info {
    int socket;
    int time_left;
};

void switchCommand(int, char *[]);
void singleCommand(char *, int, int, char *, char *);
void shell(char *, int *, int *, char *, char *);
void *foo(void *);
void *schedule();

struct Info queue[5]; //max number of client threads - seems fine when we go over?
int idx = 0; //queue index

int main()
{
    //create socket
    int server_socket;
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    //check for fail error
    if (server_socket == -1)
    {
        printf("socket creation failed..\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("socket creation success\n");
    }

    //reuse addr to address bind failure
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
    {
        printf("setsockopt(SO_REUSEADDR) failed\n");
    }

    //define server address structure
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;

    //bind the socket to our specified IP and port
    if (bind(server_socket,
             (struct sockaddr *)&server_address,
             sizeof(server_address)) < 0)
    {
        printf("socket bind failed..\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("socket bind success\n");
    }

    //after it is bound, we can listen for connections
    if (listen(server_socket, 5) < 0)
    {
        printf("Listen failed..\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("socket listen success\n");
    }

    //after listen, we need a while loop so we can create multithreading
    //https://stackoverflow.com/questions/55753640/proper-way-to-accept-multiple-clients

    //scheduler thread
    pthread_t schedule_t;
    pthread_create(&schedule_t, NULL, schedule, NULL);

    //main thread - accept connections from new clients
    while (1)
    {
        int addrlen = sizeof(server_address);
        int client_socket = accept(server_socket,
                                   (struct sockaddr *)&server_address,
                                   (socklen_t *)&addrlen);
        if (client_socket < 0)
        {
            printf("accept failed..\n");
            exit(EXIT_FAILURE);
        }
        else
        {
            printf("socket accept success\n");
        }

        pthread_t tid;        
        pthread_create(&tid, NULL, foo, &client_socket);
        // pthread_create(&threads[thr_idx++], NULL, foo, &client_socket);

        //join is not necessary due to concurrency
        //close(client_sd) is done inside the thread function
    }

    //we should really never reach here because server continues to run until we forcefully terminate
    printf("server_socket closing...\n");
    close(server_socket);

    return 0;
}

/*
checks the commandNum and calls the appropriate single command with args
*/
void switchCommand(int commandNum, char *args[])
{
    switch (commandNum)
    {
    case 0:
        lsCommand(args);
        break;
    case 1:
        pwdCommand(args);
        break;
    case 2:
        wcCommand(args);
        break;
    case 3:
        psCommand(args);
        break;
    case 4:
        topCommand(args);
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
void singleCommand(char *pipeCommand, int inputRedirect, int outputRedirect, char *filename, char *sendmsg)
{
    char *args[maxArgs];
    int commandNum = parseCommand(args, maxArgs, pipeCommand);

    //-1 means this command is not valid
    if (commandNum == -1)
    {
        // strcpy(sendmsg, "Command not recognized, please try again\n");
        exit(EXIT_FAILURE);
        // return;
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

void shell(char *inputString, int *inputRedirect, int *outputRedirect, char *filename, char *sendmsg)
{
    //first detect redirection '<' or '>'
    redirection(inputString, inputRedirect, outputRedirect, filename);

    char *pipeCommands[maxCmds]; //max four commands (3 pipes)
    int commandCount = 0;
    inputDecode(pipeCommands, maxCmds, inputString, &commandCount);

    //source for fork() parent-child relationship: https://stackoverflow.com/questions/33884291/pipes-dup2-and-exec

    /*******************
    we will go through the commands and figure out what msg we need to send back to client
    *******************/
    bzero(sendmsg, sizeof(char) * maxInput);

    //we only have a single command - this is the only case where we do input/output redirection
    if (commandCount == 1)
    {
        int sendpipe[2];
        pipe(sendpipe);

        int pid0 = fork();
        if (pid0 == -1)
        {
            perror("fork failed");
            exit(EXIT_FAILURE);
        }

        //run exec and exit
        if (pid0 == 0)
        {
            close(sendpipe[0]);   // close the read end of sendpipe
            dup2(sendpipe[1], 1); // redirect stdout to the write end of sendpipe
            singleCommand(pipeCommands[0], *inputRedirect, *outputRedirect, filename, sendmsg);
        }

        waitpid(pid0, NULL, 0);
        //done with exec

        close(sendpipe[1]);                                  //close the write end of sendpipe
        read(sendpipe[0], sendmsg, sizeof(char) * maxInput); //read from the read end of sendpipe
        // printf("%s\n", sendmsg);
    }

    //1 pipe, 2 commands
    else if (commandCount == 2)
    {
        int pipe1[2];
        int pid0, pid1;

        int sendpipe[2];
        pipe(sendpipe);

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
            close(pipe1[0]);   // close the read end of pipe1
            dup2(pipe1[1], 1); // redirect stdout to the write end of pipe1
            singleCommand(pipeCommands[0], *inputRedirect, *outputRedirect, filename, sendmsg);
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
            close(pipe1[1]);   // close the write end of pipe1
            dup2(pipe1[0], 0); // redirect stdin to the read end of pipe1

            close(sendpipe[0]);   // close the read end of sendpipe
            dup2(sendpipe[1], 1); // redirect stdout to the write end of sendpipe
            singleCommand(pipeCommands[1], *inputRedirect, *outputRedirect, filename, sendmsg);
        }

        close(pipe1[0]);
        close(pipe1[1]);
        close(sendpipe[1]);                                  //close the write end of sendpipe
        read(sendpipe[0], sendmsg, sizeof(char) * maxInput); //read from the read end of sendpipe
        waitpid(pid0, NULL, 0);
        waitpid(pid1, NULL, 0);
        //done with both exec
    }

    else if (commandCount == 3)
    {
        int pipe1[2], pipe2[2];
        int pid0, pid1, pid2;

        int sendpipe[2];
        pipe(sendpipe);

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
            close(pipe1[0]);   // close the read end of pipe1
            dup2(pipe1[1], 1); // redirect stdout to the write end of pipe1
            singleCommand(pipeCommands[0], *inputRedirect, *outputRedirect, filename, sendmsg);
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
            close(pipe1[1]);   // close the write end of pipe1
            close(pipe2[0]);   // close the read end of pipe2
            dup2(pipe1[0], 0); // redirect stdin to the read end of pipe1
            dup2(pipe2[1], 1); // redirect stdout to the write end of pipe2
            singleCommand(pipeCommands[1], *inputRedirect, *outputRedirect, filename, sendmsg);
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
            close(pipe2[1]);   // close the write end of pipe2
            dup2(pipe2[0], 0); // redirect stdin to the read end of pipe2

            close(sendpipe[0]);   // close the read end of sendpipe
            dup2(sendpipe[1], 1); // redirect stdout to the write end of sendpipe
            singleCommand(pipeCommands[2], *inputRedirect, *outputRedirect, filename, sendmsg);
        }

        close(pipe1[0]);
        close(pipe1[1]);
        close(pipe2[0]);
        close(pipe2[1]);
        close(sendpipe[1]);                                  //close the write end of sendpipe
        read(sendpipe[0], sendmsg, sizeof(char) * maxInput); //read from the read end of sendpipe
        waitpid(pid0, NULL, 0);
        waitpid(pid1, NULL, 0);
        waitpid(pid2, NULL, 0);
        //done with all three exec
    }

    else if (commandCount == 4)
    {
        int pipe1[2], pipe2[2], pipe3[2];
        int pid0, pid1, pid2, pid3;

        int sendpipe[2];
        pipe(sendpipe);

        pipe(pipe1);
        pid0 = fork();

        if (pid0 == -1)
        {
            perror("fork failed");
            exit(EXIT_FAILURE);
        }

        if (pid0 == 0)
        {
            close(pipe1[0]);   // close the read end of pipe1
            dup2(pipe1[1], 1); // redirect stdout to the write end of pipe1
            singleCommand(pipeCommands[0], *inputRedirect, *outputRedirect, filename, sendmsg);
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
            close(pipe1[1]);   // close the write end of pipe1
            close(pipe2[0]);   // close the read end of pipe2
            dup2(pipe1[0], 0); // redirect stdin to the read end of pipe1
            dup2(pipe2[1], 1); // redirect stdout to the write end of pipe2
            singleCommand(pipeCommands[1], *inputRedirect, *outputRedirect, filename, sendmsg);
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
            close(pipe2[1]);   // close the write end of pipe2
            close(pipe3[0]);   // close the read end of pipe3
            dup2(pipe2[0], 0); // redirect stdin to the read end of pipe2
            dup2(pipe3[1], 1); // redirect stdout to the write end of pipe3
            singleCommand(pipeCommands[2], *inputRedirect, *outputRedirect, filename, sendmsg);
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
            close(pipe3[1]);      // close the write end of pipe3
            dup2(pipe3[0], 0);    // redirect stdin to the read end of pipe3
            close(sendpipe[0]);   // close the read end of sendpipe
            dup2(sendpipe[1], 1); // redirect stdout to the write end of sendpipe
            singleCommand(pipeCommands[3], *inputRedirect, *outputRedirect, filename, sendmsg);
        }

        close(pipe1[0]);
        close(pipe1[1]);
        close(pipe2[0]);
        close(pipe2[1]);
        close(pipe3[0]);
        close(pipe3[1]);
        close(sendpipe[1]);                                  //close the write end of sendpipe
        read(sendpipe[0], sendmsg, sizeof(char) * maxInput); //read from the read end of sendpipe
        waitpid(pid0, NULL, 0);
        waitpid(pid1, NULL, 0);
        waitpid(pid2, NULL, 0);
        waitpid(pid3, NULL, 0);
        //done with all four exec
    }

    else //commandCount is not modified from inputDecode and is still 0
    {
        strcpy(sendmsg, "Too many piped commands, please limit to 3 pipes\n");
    }
}

void *schedule()
{

    sem_t clientrunning;
    sem_init(&clientrunning, 0, 0); //not SHARED, initial value 0

    for (int i=0; i<5; i++){
        queue[i].socket = -1; //to initially indicate as NULL
    }

    while (1) {
        while (idx == 0) ; //infinite wait
        printf("passed infinite wait\n");

        //only calculate shortest time left if there is no client running
        sem_wait(&clientrunning);
        printf("passed sem_wait clientrunning\n");

        //there is some thread in the queue
        //determine which thread should run, shortest time_left?
        int min_socket, min_idx;
        int min_time = INT32_MAX;

        for (int i=0; i<5; i++){
            if (queue[i].socket == -1) continue; //skip

            if (queue[i].time_left < min_time) {
                min_time = queue[i].time_left;
                min_socket = queue[i].socket;
                min_idx = i;
            }
        }

        //we have the socket with the shortest time_left
        printf("SRJ socket %d with time %d\n", min_socket, min_time);

        //now execute this thread by releasing its semaphore
        

        //release lock
        sem_post(&clientrunning);
    }
    
    return 0;
}

void *foo(void *arg)
{
    int client_socket = *(int *)arg;
    while (1)
    {
        char inputString[maxInput];
        char filename[maxInput];
        char sendmsg[maxInput];
        int inputRedirect = 0, outputRedirect = 0;

        //if we receive proper instructions to our server, instead of signal interrupt
        if (recv(client_socket, &inputString, sizeof(inputString), 0))
        {

            //critical section
            struct Info info;
            info.socket = client_socket;
            info.time_left = 10;

            queue[idx] = info;
            idx++;
            //critical section

            printf("%s\n", inputString);

            //check if input is executable
            if (inputString[0] == '.')
            {
                bzero(sendmsg, sizeof(char) * maxInput);
                int sendpipe[2];
                pipe(sendpipe);

                int pid0 = fork();
                if (pid0 == -1)
                {
                    perror("fork failed");
                    exit(EXIT_FAILURE);
                }

                //run exec and exit
                if (pid0 == 0)
                {
                    char tmp[maxInput];
                    strcpy(tmp, inputString);

                    char programName[maxInput];
                    char* token = strtok(tmp, " ");
                    strcpy(programName, token);

                    // printf("programName: %s\n", programName);

                    char arg1[maxInput] = ""; //only one argument for now
                    while (token != NULL) {
                        // printf("token!=NULL\n");
                        token = strtok(NULL, " ");
                        if (token != NULL) {
                            strcpy(arg1, token);
                            // printf("%s\n", arg1);
                        }
                    }

                    // printf("arg1: %s\n", arg1);
                    
                    close(sendpipe[0]);   // close the read end of sendpipe
                    dup2(sendpipe[1], 1); // redirect stdout to the write end of sendpipe

                    if (strcmp(arg1, "") == 0) {
                        // printf("empty arg1\n");
                        execl(programName, programName, NULL);
                    } else {
                        // printf("non-empty arg1\n");
                        execl(programName, programName, arg1, NULL);
                    }

                    perror("executable"); //if we reached here, there is an error so we must exit
                    exit(EXIT_FAILURE);
                }

                // waitpid(pid0, NULL, 0);
                // done with exec - not sure why even when it's commented out, the printf is sent after exec is done

                close(sendpipe[1]);                                  //close the write end of sendpipe
                read(sendpipe[0], sendmsg, sizeof(char) * maxInput); //read from the read end of sendpipe
            }

            //run the shell script
            else {
                shell(inputString, &inputRedirect, &outputRedirect, filename, sendmsg);
            }

            //send the result back to client
            send(client_socket, sendmsg, sizeof(sendmsg), 0);

            //critical section
            struct Info nullinfo;
            nullinfo.socket = -1;
            queue[idx] = nullinfo;
            idx--;
            //critical section
        }

        //signal interrupt
        else
        {
            break;
        }
    }
    printf("client socket #%d closing...\n\n", client_socket);
    close(client_socket);
    return NULL;
}