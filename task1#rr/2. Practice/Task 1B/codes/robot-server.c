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
	}



	if (listen(server_fd, 3) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}

	if ((my_sock = accept(server_fd, (struct sockaddr *)&source_addr,(socklen_t*)&source_addr))<0)
	{
		perror("accept");
		exit(EXIT_FAILURE);
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


	bzero(rx_buffer,sizeof(rx_buffer));
	n = read(sock,rx_buffer,sizeof(rx_buffer));

	//printf("Readings by server : %s\n",rx_buffer);

	//strcpy(tx_buffer,line_data);
	bzero(tx_buffer,sizeof(tx_buffer));


	const char* at0 = "@";


	strcat(tx_buffer,at0);
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
			//printf("\n character at pos+1 tx_buffer is : %c\n",tx_buffer[pos+1]);
			pos++;
		};
	}
	const char* at1 = "@";
	strcat(tx_buffer,at1);
	//printf("\nPosition of ( %s\n",strchr(line_data,'('));

	//printf("\n character at position 4 : %c\n",tx_buffer[4]+'\0');
	// substraction with null character is done because buffer return ascii in integer form.
	//which can be converterd back by simply subctracting null charcter to the integer in c

	//printf("\nWriting before buffer(line_data) : %s\n",tx_buffer);

	//printf("\nwriting by server %s\n",tx_buffer);
	write(sock,tx_buffer,sizeof(tx_buffer));
	//printf("\nwriting to buffer : %s\n",tx_buffer);
	//updation in line_data
	/*
	if(strcmp(line_data,"@$@") == 0)
	{
		printf("\nStream end\n");
		return 0;
	}
	*/

	char *new_data = strchr(line_data,';');
//	printf("\nnew_data  : %s\n",new_data);
	if(new_data != NULL)
	{
		strcpy(line_data,&new_data[1]);
		//printf("\nData in line_data is : %s \n",line_data);
	}
	else
	{
		strcpy(line_data,"@$@");
		//printf("\nData in line_data is : %s \n",line_data);

	}



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
