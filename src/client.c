#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 5000
#define SUCCESS_MESSAGE "Introduza uma nova palavra-passe"
#define ERROR_MESSAGE   "Não se encontra dentro do sistema"
#define FILE_WRITTEN    "Nova password guardada"

int port = 5000;

void get_params(char const *argv[]){
    port = atoi(argv[1]);
}

int main(int argc, char const *argv[]) {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char *buffer = malloc(64);
    char *message = malloc(64);

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    if(argc != 1){
        get_params(argv);
    }

    // Set the server address and port
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\n Invalid address/ Address not supported \n");
        return -1;
    }

    // Connect to the server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\n Connection Failed \n");
        return -1;
    }

    printf("Connected\nIntroduce your username:password\n");

    // Read user input from terminal
    while (1) {
        memset(message, 0, 64);
        gets(message, stdin);
        message[strcspn(message, "\n")] = '\0';

        // Send user input to server
        send(sock, message, strlen(message), 0);

        // If the user enters "Over", end the connection
        if (strcmp(message, "Over\0") == 0) {
            break;
        }
        // Receive response from server
        memset(buffer, 0, 64);
        read(sock, buffer, 64);
        printf("%s\n", buffer);
        if(strcmp(buffer, SUCCESS_MESSAGE)){
            gets(message, stdin);
            message[strcspn(message, "\n")] = '\0';
            // Send user input to server
            send(sock, message, strlen(message), 0);
            // Receive response from server
            memset(buffer, 0, 64);
            read(sock, buffer, 64);
            printf("%s\n", buffer);
        }

    }

    // Close the connection
    close(sock);
    return 0;
}
