#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "client.h"

#define PORT 5000
#define SUCCESS_MESSAGE "Introduza uma nova palavra-passe"
#define ERROR_MESSAGE   "Não se encontra dentro do sistema"
#define FILE_WRITTEN    "Nova password guardada"
#define EXIT_MESSAGE    "Sair da aplicação"

int port = 5000;

void get_params(char const *argv[])
{
    port = atoi(argv[1]);
}

int client(int sock){
    char *buffer;
    char *message;

    while (1)
    {
        message = malloc(64);
        bzero(message, 64);
        scanf("%s", message);
        //message[strcspn(message, "\n")] = '\0';
        printf("Sending %s to server with size %ld.\n", message, strlen(message));

        // Envia input para o servidor
        write(sock, message, strlen(message));

        // Se o utilizador introduz Over então sai do ciclo
        if (strncmp(message, "Over", 4) == 0)
        {
            return 0;
        }

        // Recebe resposta do servidor
        buffer = malloc(64);
        bzero(buffer, 64);
        read(sock, buffer, 64);
        printf("%s\n", buffer);

        // Se for sucesso altera a palavra passe
        if (strcmp(buffer, SUCCESS_MESSAGE))
        {
            free(message);
            message = malloc(64);
            bzero(message, 64);
            scanf("%s", message);
            //message[strcspn(message, "\n")] = '\0';
            // Send user input to server
            send(sock, message, strlen(message), 0);
            // Receive response from server
            free(buffer);
            buffer = malloc(64);
            bzero(buffer, 64);
            read(sock, buffer, 64);
            printf("%s\n", buffer);
        }
        free(message);
        free(buffer);
    }
    return -1;
}

int main(int argc, char const *argv[])
{
    int sock = 0;
    struct sockaddr_in serv_addr;
    
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);

    // Cria socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    // Verifica se existe porta
    if (argc != 1)
    {
        get_params(argv);
    }

    // Faz set do endereço e da porta
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        printf("\n Invalid address/ Address not supported \n");
        return -1;
    }

    // Conexão ao servidor
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\n Connection Failed \n");
        return -1;
    }

    printf("Connected\n");
    printf("Introduce your username:password\n");

    // Lê input do terminal
    client(sock);

    // Close the connection
    close(sock);
    return 0;
}
