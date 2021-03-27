#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#define PORT 5000
#define DISCONNECT_MESSAGE "quit\n"

char name[25];

void clean_exit_on_sig(int sig_num)
{
    printf("\n Signal %d received because of input buffer overflow\n", sig_num);
    exit(1);
}

void *send_message(void *socket)
{
    signal(SIGSEGV, clean_exit_on_sig);
    int new_socket = (intptr_t)socket;
    char message[1024] = {0};
    char f_message[1024] = {0};

    while (1)
    {
        int length = 0;
        memset(f_message, 0, 1024);
        memset(message, 0, 1024);
        fgets(message, 1024, stdin);

        if (!strcmp(message, DISCONNECT_MESSAGE))
        {
            exit(0);
        }
        if (!strcmp(message, "\n"))
        {
            continue;
        }
        for (int i = 0; message[i] != '\n'; i++)
        {
            ++length;
        }
        message[length] = '\0';
        strcat(f_message, message);
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
        memset(buffer, 0, 1024);
        valread = read(new_socket, buffer, 1024);
        if (valread == 0)
        {
            printf("\n\t-------------{{Server DISCONNECTED}}-------------\n\n");
            exit(0);
        }
        if (strcmp(buffer, DISCONNECT_MESSAGE) == 0)
        {
            exit(0);
        }
        printf("\t\t\t%s\n", buffer);
    }
}

int main(int argc, char const *argv[])
{
    signal(SIGSEGV, clean_exit_on_sig);
    char pass[20];
    strncpy(pass, argv[1], 20);
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    printf("Enter your username: ");
    scanf("%s", name);
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, pass, &serv_addr.sin_addr) <= 0)
    {
        printf("\nInvalid address / Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }
    printf("\n\t-------------{{CONNECTED to Server}}-------------\n\n");
    send(sock, name, strlen(name), 0);

    pthread_t recieve_thread;
    pthread_create(&recieve_thread, NULL, rec_message, (void *)(intptr_t)sock);
    pthread_t send_thread;
    pthread_create(&send_thread, NULL, send_message, (void *)(intptr_t)sock);
    char recv[1024] = {0};
    read(sock, recv, 1024);
    printf("%s\n", recv);
    pthread_join(send_thread, NULL);
    pthread_join(recieve_thread, NULL);
    printf("Connection closed");
    return 0;
}