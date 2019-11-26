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
#include<unistd.h>

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

FILE *input_fp, *output_fp;


/*
* Function Name:	socket_create
* Inputs:			dest_addr [ structure type for destination address ]
* 					source_addr [ structure type for source address ]
* Outputs: 			my_sock [ socket value, if connection is properly created ]
* Purpose: 			the function creates the socket connection with the server
* Example call: 	int sock = socket_create(dest_addr, source_addr);
*/
int socket_create(struct sockaddr_in dest_addr, struct sockaddr_in source_addr){

	int addr_family;
	int ip_protocol;

	dest_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(SERVER_PORT);
	addr_family = AF_INET;
	ip_protocol = IPPROTO_IP;

	int my_sock;
	int server_fd;
	int opt = 1;

	if ((server_fd = socket(addr_family, SOCK_STREAM, ip_protocol)) == 0)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	} else {
		printf("Socket created\n");
	}

	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,&opt, sizeof(opt)))
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

	if (bind(server_fd, (struct sockaddr *)&dest_addr, sizeof(dest_addr))<0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	} else {
		printf("Socket bound, port %d\n", SERVER_PORT);
	}

	if (listen(server_fd, 3) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	} else {
		printf("Socket listening\n");
	}

	if ((my_sock = accept(server_fd, (struct sockaddr *)&source_addr,(socklen_t*)&source_addr))<0)
	{
		perror("accept");
		exit(EXIT_FAILURE);
	} else {
		printf("Socket accepted\n");
	}

	return my_sock;
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
int receive_from_send_to_client(int sock){
	int n;

	bzero(rx_buffer,RX_BUFFER_SIZE);
	n = read(sock,rx_buffer,RX_BUFFER_SIZE);
	printf("----------------------------\n");
	printf("Received %ld bytes from  :\nData Received = %s\n", strlen(rx_buffer)+2, rx_buffer);

	bzero(tx_buffer,TX_BUFFER_SIZE);

// goes here when img0 is completed
	if(rx_buffer[0]!='0') {
		bzero(line_data,MAXCHAR);
		// checks from start of file
		fseek(input_fp, 0, SEEK_SET);
		// until file end is not reached
		while(fgets(line_data, MAXCHAR, input_fp)!=NULL) {
			// when obstacles are found for image, initialise tx_buffer with '@' and goto xyz label, rest is same
			if(rx_buffer[0]==line_data[0]) {
				const char* at0 = "@";
				strcat(tx_buffer,at0);
				goto xyz;
			}
		}
		return 0;
	}

	const char* at0 = "@";
	strcat(tx_buffer,at0);

xyz:
	if(strcmp(line_data,"@$@") == 0)
	{
		tx_buffer[1] = '$';
	}
	else
	{
		const char* p = strchr(line_data,'(');
		int pos = 0;
		while(pos < 5){

			tx_buffer[pos+1] = p[pos];
			pos++;
		};
	}
	const char* at1 = "@";
	strcat(tx_buffer,at1);

	write(sock,tx_buffer,strlen(tx_buffer));
	printf("Transmitted %ld bytes: %s\n",strlen(tx_buffer),tx_buffer);

	char *new_data = strchr(line_data,';');
	if(new_data != NULL)
		strcpy(line_data,&new_data[1]);
	else
		strcpy(line_data,"@$@");

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
