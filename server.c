#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#define MAX_CLIENTS 4
int main()
{
	int opt = 1;
	int master_socket, addrlen, new_socket, client_socket[MAX_CLIENTS],max_clients = MAX_CLIENTS, activity, i, valread, sd;
	int max_sd;
	int server_len, client_len;
	struct sockaddr_un server_address, address;
	struct sockaddr_un client_address;
	char buffer[1025];
	char *Usernames[4];
	int ReplyAllFD;
	int clientsConnected = 0;
	fd_set readfds;
	//initialise all client_socket[] to 0 so not checked
	for (i = 0; i < max_clients; i++)
	{
		client_socket[i] = 0;
	}
	/*  Remove any old socket and create an unnamed socket for the server.  */
	unlink("server_socket");
	master_socket = socket(AF_UNIX, SOCK_STREAM, 0); // add a check here
	if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,sizeof(opt)) < 0)
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	/*  Name the socket.  */
	server_address.sun_family = AF_UNIX;
	strcpy(server_address.sun_path, "server_socket");
	server_len = sizeof(server_address);
	if (bind(master_socket, (struct sockaddr *)&server_address, server_len))
	{
		perror("bind failed");
		exit(-1);
	}
	/*  Create a connection queue and wait for clients.  */
	printf("Server listening for client connection...\n");
	if (listen(master_socket, 3) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}
	addrlen = sizeof(address);
	puts("Waiting for connections ...");
	while (1)
	{
		//clear the socket set
		FD_ZERO(&readfds);
		//add master socket to set
		FD_SET(master_socket, &readfds);
		max_sd = master_socket;
		//add child sockets to set
		for (i = 0; i < max_clients; i++)
		{
			//socket descriptor
			sd = client_socket[i];
			//if valid socket descriptor then add to read list
			if (sd > 0)
				FD_SET(sd, &readfds);
			//highest file descriptor number, need it for the select function
			if (sd > max_sd)
				max_sd = sd;
		}
		//wait for an activity on one of the sockets , timeout is NULL ,
		//so wait indefinitely
		activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);
		if (activity < 0)
		{
			printf("select error");
		}
		//If something happened on the master socket ,
		//then its an incoming connection
		if (FD_ISSET(master_socket, &readfds))
		{
			if ((new_socket = accept(master_socket,(struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
			{
				perror("accept");
				exit(EXIT_FAILURE);
			}
			//add new socket to array of sockets
			for (i = 0; i < max_clients; i++)
			{
				//if position is empty
				if (client_socket[i] == 0)
				{
					client_socket[i] = new_socket;
					clientsConnected++;
					printf("Adding to list of sockets as %d\n", i);
					break;
				}
			}
		}
		//else its operation on one of the clients
		for (i = 0; i < max_clients; i++)
		{
			sd = client_socket[i];
			if (FD_ISSET(sd, &readfds))
			{
				//Check if it was for closing , and also read the
				//incoming message
				if ((valread = read(sd, buffer, 1024)) == 0)
				{
					//Close the socket and mark as 0 in list for reuse
					close(sd);
					client_socket[i] = 0;
				}
				else
				{
					// Write to all clients
					int j;
					for (j = 0; j < clientsConnected; j++)
					{
						write(client_socket[j], buffer, strlen(buffer));
						// flush the buffer to prevent messages overwritting eachother
						fflush(stdout);
						fflush(stdin);
					}
				}
			}
		}
	}
	return 0;
}
