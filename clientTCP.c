#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define PORT 8080

void printMenu() {
    printf("Services disponibles :\n");
    printf("1. Obtenir la date et l'heure\n");
    printf("2. Liste des fichiers du répertoire\n");
    printf("3. Contenu d'un fichier\n");
    printf("4. Durée écoulée depuis la connexion\n");
    printf("0. Quitter\n");
}

int authenticate() {
    char username[50];
    char password[50];
    char storedUsername[] = "unix"; 
    char storedPassword[] = "unix"; 

    printf("Veuillez vous connecter :\n");
    printf("Nom d'utilisateur : ");
    scanf("%s", username);
    printf("Mot de passe : ");
    scanf("%s", password);

    if (strcmp(username, storedUsername) == 0 && strcmp(password, storedPassword) == 0) {
        printf("Authentification réussie.\n");
        return 1; 
    } else {
        printf("Échec de l'authentification.\n");
        return 0; 
    }
}

int main() {
    int client_socket;
    struct sockaddr_in server_address;
    int choice;
    int authenticated = 0;

    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        exit(EXIT_FAILURE);
    }

    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }
    
    while (!authenticated) {
        authenticated = authenticate();
        if (authenticated == 1) 
        {
        do {
        printMenu();
        printf("Choix : ");
        scanf("%d", &choice);

        write(client_socket, &choice, sizeof(int));

        switch (choice) {
            case 1: {
                char dateTime[100];
                read(client_socket, dateTime, sizeof(dateTime));
                printf("Date et heure du serveur : %s\n", dateTime);
                break;
            }
            case 2: {
                char fileList[1024];
                read(client_socket, fileList, sizeof(fileList));
                printf("Liste des fichiers :\n%s\n", fileList);
                break;
            }
            case 3: {
                char filename[256];
                printf("Entrez le nom du fichier : ");
                scanf("%s", filename);
                write(client_socket, filename, sizeof(filename));

                char fileContent[1024];
                read(client_socket, fileContent, sizeof(fileContent));
                printf("Contenu du fichier :\n%s\n", fileContent);
                break;
            }
            case 4: {
                char duration[50];
                read(client_socket, duration, sizeof(duration));
                printf("Durée depuis la connexion : %s\n", duration);
                break;
            }
            default:
                break;
        }
    } while (choice != 0);
    }
    }

    

    close(client_socket);
    return 0;
}
