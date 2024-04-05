#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#define NMAX 100 

int main(int argc, char *argv[]) {
	srand(time(NULL));
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <adresse_serveur> <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int sockfd;
    struct sockaddr_in servaddr;

    int port = atoi(argv[2]);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    memset(&servaddr, 0, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

    
    int n = rand() % NMAX + 1;

    sendto(sockfd, &n, sizeof(n), 0, (const struct sockaddr *)&servaddr, sizeof(servaddr));

    printf("Client a envoyé : %d\n", n);

    
    printf("Réception des nombres du serveur :\n");
    for (int i = 0; i < n; i++) {
        int received_num;
        recvfrom(sockfd, &received_num, sizeof(received_num), 0, NULL, NULL);
        printf("%d\n", received_num);
    }

    close(sockfd);
    return 0;
}
