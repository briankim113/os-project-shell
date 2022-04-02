#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 

#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/in.h>

#define PORT 9002


int main()
{
	//create socket
	int server_socket;
	server_socket = socket(AF_INET , SOCK_STREAM,0);

	//check for fail error
	if (server_socket == -1) {
        printf("socket creation failed..\n");
        exit(EXIT_FAILURE);
    }
    
	//define server address structure
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(PORT);
	server_address.sin_addr.s_addr = INADDR_ANY;


	//bind the socket to our specified IP and port
	if (bind(server_socket , 
		(struct sockaddr *) &server_address,
		sizeof(server_address)) < 0)
	{
		printf("socket bind failed..\n");
        exit(EXIT_FAILURE);
	}
	 

	//after it is bound, we can listen for connections
	if(listen(server_socket,5)<0){
		printf("Listen failed..\n");
        exit(EXIT_FAILURE);
	}
	 


	int addrlen = sizeof(server_address);	

	int client_socket;
	
	client_socket = accept(server_socket,
		(struct sockaddr *) &server_address,
		(socklen_t *) &addrlen);

	if(client_socket<0){
		printf("accept failed..\n");
        exit(EXIT_FAILURE);
	}
	

    //receive "hello" message from client
	char hello_msg[1024];
	recv(client_socket , &hello_msg , sizeof(hello_msg),0);

	//print out the data we get back
	printf("Received: %s\n" , hello_msg);

	//send "hello back" to client
	char hello_back_msg[]="Hello back\n";
    send(client_socket , hello_back_msg , sizeof(hello_back_msg),0);

    //receive array of integers
    printf("Waiting for client to send an array..\n");

    int msg[1024];
    int n = recv(client_socket,msg,1024*sizeof(int),0);
    printf("Number of bytes read = %d; thus %ld is the size of the array\n",n,n/sizeof(int));
    printf("Array received: \n");
    for(int j=0;j<n/sizeof(int);j++){
      printf("%d ",msg[j]);
    }

    //calculate sum
    int sum=0;
    printf("\nFinished reading\nComputing sum:") ;
    for(int k=0;k<n/sizeof(int);k++){
      sum+=msg[k];
    }
    printf("\nSum= %d\n", sum);

    //send sum back to client
    send(client_socket , &sum , sizeof(int),0);

    printf("Finished sending sum...\n");

	//close the socket
	close(server_socket); 



	return 0;
}