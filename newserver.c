#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

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
    int n;
    char **IPs = get_system_IPs();
    printf("Choose the network in which the server must be hosted:\n");
    for (int i = 0; IPs[i] != NULL; ++i)
    {
        printf("[%d] %s", i + 1, IPs[i]);
        n = i;
    }
    printf("\nEnter [1-%d]:", n + 1);
    scanf("%d", &n);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    return 1;
}