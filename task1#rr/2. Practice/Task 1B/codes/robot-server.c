/*
*****************************************************************************************
*
*        		===============================================
*           		Rapid Rescuer (RR) Theme (eYRC 2019-20)
*        		===============================================
*
*  This script is to implement Task 1B of Rapid Rescuer (RR) Theme (eYRC 2019-20).
*  
*  This software is made available on an "AS IS WHERE IS BASIS".
*  Licensee/end user indemnifies and will keep e-Yantra indemnified from
*  any and all claim(s) that emanate from the use of the Software or 
*  breach of the terms of this agreement.
*  
*  e-Yantra - An MHRD project under National Mission on Education using ICT (NMEICT)
*
*****************************************************************************************
*/

/*
* Team ID:			[ Team-ID ]
* Author List:		[ Names of team members worked on this file separated by Comma: Name1, Name2, ... ]
* Filename:			task_1a.py
* Functions:		readImage, solveMaze
* 					[ Comma separated list of functions in this file ]
* Global variables:	CELL_SIZE
* 					[ List of global variables defined in this file ]
*/


// Include necessary header files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <arpa/inet.h>


// Constants defined
#define SERVER_PORT 3333
#define RX_BUFFER_SIZE 1024
#define TX_BUFFER_SIZE 1024

#define MAXCHAR 1000				// max characters to read from txt file

// Global variables
struct sockaddr_in dest_addr;
struct sockaddr_in source_addr;

char rx_buffer[RX_BUFFER_SIZE];		// buffer to store data from client
char tx_buffer[RX_BUFFER_SIZE];		// buffer to store data to be sent to client

char ipv4_addr_str[128];			// buffer to store IPv4 addresses as string
char ipv4_addr_str_client[128];		// buffer to store IPv4 addresses as string

int listen_sock;

char line_data[MAXCHAR];
int line_ptr = 3;

FILE *input_fp, *output_fp;


/*
* Function Name:	socket_create
* Inputs:			dest_addr [ structure type for destination address ]
* 					source_addr [ structure type for source address ]
* Outputs: 			my_sock [ socket value, if connection is properly created ]
* Purpose: 			the function creates the socket connection with the server
* Example call: 	int sock = socket_create(dest_addr, source_addr);
*/
int socket_create(struct sockaddr_in dest_addr, struct sockaddr_in source_addr) {

	int opt = 1, new_socket;
	int addrlen = sizeof(dest_addr);

	dest_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(SERVER_PORT);

	int my_sock;
	printf("Self IP = %d.%d.%d.%d\n",INADDR_ANY,INADDR_ANY,INADDR_ANY,INADDR_ANY);
	if ((my_sock = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		perror("socket failed");
		exit(EXIT_FAILURE);
	} else {
		printf("Socket created\n");
	}

	if (setsockopt(my_sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

	if (bind(my_sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr))<0) {
		perror("bind failed");
		exit(EXIT_FAILURE);
	} else {
		printf("Socket bound, port %d\n", SERVER_PORT);
	}

	if (listen(my_sock, 3) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	} else {
		printf("Socket listening\n");
	}

	if ((new_socket = accept(my_sock, (struct sockaddr *)&dest_addr, (socklen_t*)&addrlen))<0) {
		perror("accept");
		exit(EXIT_FAILURE);
	} else {
		printf("Socket accepted\n");
	}

	return new_socket;
}


/*
* Function Name:	receive_from_send_to_client
* Inputs:			sock [ socket value, if connection is properly created ]
* Outputs: 			None
* Purpose: 			the function receives the data from server and updates the 'rx_buffer'
*					variable with it, sends the obstacle position based on obstacle_pos.txt
*					file and sends this information to the client in the provided format.
* Example call: 	receive_from_send_to_client(sock);
*/
int receive_from_send_to_client(int sock) {

	for(int i=0; i<strlen(rx_buffer); ++i) rx_buffer[i]='\0';		//clear rx_buffer
	if(recv(sock,rx_buffer,RX_BUFFER_SIZE,0)==0)
		return 1;

	int image_num = rx_buffer[0];
	for(int i=0;i<strlen(rx_buffer);i++) rx_buffer[i]=rx_buffer[i+2];

	printf("----------------------------\n");
	printf("Received %ld bytes from  :\nData Received = %s\n", strlen(rx_buffer)+2, rx_buffer);
	int i=0;
	tx_buffer[i++]='@';
	tx_buffer[i++]='$';
	tx_buffer[i++]='@';
	tx_buffer[i++]='\0';
/*
	if(image_num==(int)line_data[0]) {
		while(1) {
			if(line_data[line_ptr]=='\n') {
				line_ptr = 3;
				break;
			}
			else if(line_data[line_ptr]==';') {
				tx_buffer[i++]='@';
				tx_buffer[i++]='\0';
				break;
			}
			else {
				tx_buffer[i++]=line_data[line_ptr];
			}
			line_ptr++;
		}
	}
*/
	printf("Transmitted %ld bytes: %s\n",strlen(tx_buffer),tx_buffer);
	send(sock,tx_buffer,strlen(tx_buffer),0);

	return 0;
}


int main() {
	
    char *input_file_name = "obstacle_pos.txt";
	char *output_file_name = "data_from_client.txt";

	// Create socket and accept connection from client
	int sock = socket_create(dest_addr, source_addr);

	input_fp = fopen(input_file_name, "r");

	if (input_fp == NULL){
		printf("Could not open file %s\n",input_file_name);
		return 1;
	}

	fgets(line_data, MAXCHAR, input_fp);

	output_fp = fopen(output_file_name, "w");

	if (output_fp == NULL){
		printf("Could not open file %s\n",output_file_name);
		return 1;
	}

	while (1) {

		// Receive and send data from client and get the new shortest path
		receive_from_send_to_client(sock);

		// NOTE: YOU ARE NOT ALLOWED TO MAKE ANY CHANGE HERE
		fputs(rx_buffer, output_fp);
		fputs("\n", output_fp);

	}

	return 0;
}

