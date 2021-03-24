#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 5000
#define NUMBER_OF_CLIENTS 3

char **get_system_IPs()
{
    FILE *fp, *outputfile;
    char *cmd = "ip -4 addr | grep -oE '\\b([0-9]{1,3}\\.){3}[0-9]{1,3}/[0-9]{1,2}\\b' | grep -oE '\\b([0-9]{1,3}\\.){3}[0-9]{1,3}\\b'";
    fp = popen(cmd, "r");
    char ip[20];
    char **sub_str = malloc(100 * ((20 * sizeof(char))));
    int i = 0;
    while (fgets(ip, sizeof(ip), fp) != NULL)
    {
        sub_str[i] = malloc(20 * sizeof(char));
        strncpy(sub_str[i], ip, 20);
        ++i;
    }
    char **IPs = (char **)realloc(sub_str, (i + 1) * ((20 * sizeof(char))));
    pclose(fp);
    return IPs;
}

int main()
{
    int n, new_socket, server_fd, valread;
    int opt = 1;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

    // Select Network
    char IP[20];
    char **IPs = get_system_IPs();
    int length = 0;

    printf("Choose the network in which the server must be hosted:\n");
    for (int i = 0; IPs[i] != NULL; ++i)
    {
        printf("[%d] %s", i + 1, IPs[i]);
        n = i;
    }
    printf("\nEnter [1-%d]:", n + 1);
    scanf("%d", &n);
    for (int i = 0; IPs[n - 1][i] != '\n'; i++)
    {
        IP[i] = IPs[n - 1][i];
        ++length;
    }
    IP[length] = '\0';

    // sockets
    char *hello = "Hello from server";

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(IP);
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, NUMBER_OF_CLIENTS) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    valread = read(new_socket, buffer, 1024);
    printf("%s\n", buffer);
    send(new_socket, hello, strlen(hello), 0);
    printf("Hello message sent\n");

    return 0;
}