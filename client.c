#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>

#define BUFFSIZE 2046

int main()
{
    int sockfd;
    int len;
    struct sockaddr_un address;
    int result;
    int check = -1;
    int max_sd;
    int activity = 0;
    int check1;
    size_t MaxMessage;
    fd_set ServerSTDIN;
    char *Message = NULL;
    char FullMessage[65] = {0};
    char ReplyFromServer[65] = {0};
    char Username[10] = {0};
    printf("Enter username under 10 characters!\n");
    scanf("%s", Username);
    printf("\n");
    FD_ZERO(&ServerSTDIN);
    FD_SET(0, &ServerSTDIN); // add standard in to the fd set
    max_sd = 0;
    /*  Create a socket for the client.  */
    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    /*  Name the socket, as agreed with the server.  */
    address.sun_family = AF_UNIX;
    strcpy(address.sun_path, "server_socket");
    len = sizeof(address);
    //socket descriptor
    /*  Now connect our socket to the server's socket.  */
    result = connect(sockfd, (struct sockaddr *)&address, len);
    if (result == -1)
    {
        perror("ERROR CONNECTING TO SERVER");
        exit(1);
    }
    /*  We can now read/write via sockfd.  */
    // Fork, one process handles input from server one sends output to server
    //  error if the fork fails
    switch (fork())
    {
        // error case
    case -1:
        printf("Error forking client");
        exit(-1);
        // The first process
    case 0:
        while (1)
        {
            fflush(stdin);
            fflush(stdout);
            strcpy(ReplyFromServer, "");
            check = read(sockfd, ReplyFromServer, BUFFSIZE);
            if (check <= 0)
            {
                break;
            }
            fprintf(stdout, "%s", ReplyFromServer);
            memset(ReplyFromServer, 0, 65);
            printf("\n");
        }
    // The second
    default:
        while (1)
        {
            // Weclome message, should only print once
            printf("Welcome to the chat room!\n");
            while ((check = getline(&Message, &MaxMessage, stdin)) > 1 && Message != NULL)
            {
                // Terminate if quit message sent
                if (strcmp(Message, "quit\n") == 0)
                {
                    printf("Disconnected from server!\n");
                    close(sockfd);
                    exit(0);
                }
                // concatonate username on to message
                strcat(FullMessage, Username);
                strcat(FullMessage, ": ");
                strcat(FullMessage, Message);
                check = write(sockfd, FullMessage, BUFFSIZE);
                if (check < 0)
                {
                    perror("Error writing to server!");
                    exit(-1);
                }
                fflush(stdin);
                strcpy(FullMessage, "");
                strcpy(Message, "");
            }
        }
    }
    close(sockfd);
    exit(0);
}
