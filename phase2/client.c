#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 

#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/in.h>

#define PORT 9002 //or 8080 or any other unused port value

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
	

	//now that we have the connect, we either send or receive data

    //send "hello" message
    char hello_msg[]="hello\n";
    send(network_socket , hello_msg , sizeof(hello_msg),0);

	//receive "hello back" from server
	char hello_back_msg[1024];
	recv(network_socket , &hello_back_msg , sizeof(hello_back_msg),0);

	//print out the data we get back
	printf("Received: %s\n" , hello_back_msg);


	//Read array of integers from user and send it to server
	int size;
	printf("\nPlease enter the size of the array:\n");
	scanf("%d",&size);
	printf("\nPlease enter %d numbers:\n",size);
	int arr[size];
	for(int i=0;i<size;i++){
	  scanf("%d",&arr[i]);
	}

	//send array to server
	send(network_socket, arr, sizeof(int) * size,0);

	//receive sum of aray elements from server
	int received_sum;
	recv(network_socket , &received_sum , sizeof(int),0);

	printf("\nClient received Sum= %d\n",received_sum);


	//close socket after we are done
	close(network_socket);



	return 0;
}
