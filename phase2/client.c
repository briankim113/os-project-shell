#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#include "functions.h"
#define PORT 9002 //or 8080 or any other unused port value

/*
prints welcome messages
*/
void welcome()
{
    printf("Welcome to the main shell!\n");
    printf("If you wish to exit the shell, use 'exit'\n");
}

int main(){

	//create a socket
	int network_socket;
	network_socket = socket(AF_INET , SOCK_STREAM, 0);

	//check for fail error
	if (network_socket == -1) {
        printf("socket creation failed..\n");
        exit(EXIT_FAILURE);
    }
    

	//connect to another socket on the other side
	
	//specify an address for the socket we want to connect to
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(PORT);
	server_address.sin_addr.s_addr = INADDR_ANY;


	//connect
	int connection_status = 
	connect(network_socket, 
			(struct sockaddr *) &server_address,
			sizeof(server_address));

	//check for errors with the connection
	if(connection_status == -1){
		printf("There was an error making a connection to the remote socket \n\n");
		exit(EXIT_FAILURE);
	}

    //********** shell begins here ****************
	
    welcome();
    char inputString[maxInput];
    char response[maxInput]; //with TCP & SOCK_STREAM, buffer size for recv doesn't seem to matter anyway - https://stackoverflow.com/questions/2862071/how-large-should-my-recv-buffer-be-when-calling-recv-in-the-socket-library
    
    while (1)
    {
        //get the user input and save it inside inputString
        if (isInputEmpty(inputString))
            continue; //if empty, continue asking for actual input

        //if "exit", then stop the shell
        if (strcmp(inputString, "exit") == 0)
        {
            printf("goodbye!\n");
            break;
        }

        //send the input without any cleaning to the server directly, where it will be handled
        if (send(network_socket, inputString, sizeof(inputString), 0) <= 0){
            perror("send error ");
        }

        //receive response back from the server
        if (recv(network_socket, response, sizeof(response), 0) <= 0){
            perror("recv error ");
        }

        //print the response
        printf("%s\n", response);
    }

	//close socket after we are done
	close(network_socket);
	return 0;
}
