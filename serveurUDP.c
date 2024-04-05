#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define NMAX 100 

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int sockfd;
    struct sockaddr_in servaddr, cliaddr;

    int port = atoi(argv[1]);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);

    bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr));

    printf("Serveur en attente...\n");

    while (1) {
        int n;
        unsigned int len = sizeof(cliaddr);
        recvfrom(sockfd, &n, sizeof(n), 0, (struct sockaddr *)&cliaddr, &len);

        printf("Client a envoyé : %d\n", n);

        for (int i = 0; i < n; i++) {
            int random_num = rand() % NMAX + 1;
            sendto(sockfd, &random_num, sizeof(random_num), 0, (const struct sockaddr *)&cliaddr, len);
        }
    }

    close(sockfd);
    return 0;
}
