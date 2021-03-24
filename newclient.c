#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#define PORT 5000

#define DISCONNECT_MESSAGE "quit"

void *send_message(void *socket)
{
    int new_socket = (intptr_t)socket;
    char message[1024] = "hi";
    char f_message[1024];
    while (1)
    {
        scanf("%s", message);
        if (!strcmp(message, DISCONNECT_MESSAGE))
        {
            exit(0);
        }
        strcat(f_message, "[Client]");
        strcat(f_message, message);
        printf("%s\n", f_message);
        send(new_socket, f_message, strlen(f_message), 0);
    }
}
void *rec_message(void *socket)
{
    int new_socket = (intptr_t)socket;
    char buffer[1024] = {0};
    int valread;

    while (1)
    {
        valread = read(new_socket, buffer, 1024);
        if (!strcmp(buffer, DISCONNECT_MESSAGE))
        {
            return (0);
        }
        printf("%s\n", buffer);
    }
}

int main(int argc, char const *argv[])
{
    char pass[20];
    strncpy(pass, argv[1], 20);
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char *hello = "Hello from client";
    char buffer[1024] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, pass, &serv_addr.sin_addr) <= 0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }

    pthread_t recieve_thread;
    pthread_create(&recieve_thread, NULL, rec_message, (void *)(intptr_t)sock);
    pthread_t send_thread;
    pthread_create(&send_thread, NULL, send_message, (void *)(intptr_t)sock);
    pthread_join(send_thread, NULL);
    pthread_join(recieve_thread, NULL);
    printf("Connection closed");
    return 0;
}