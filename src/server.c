#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "server.h"

#define PORT 5000
#define SUCCESS_MESSAGE "Introduza uma nova palavra-passe"
#define ERROR_MESSAGE   "Não se encontra dentro do sistema"
#define FILE_WRITTEN    "Nova password guardada"

int port = 5000;


char* handle_operation(char *username_password)
{
    char* return_value = malloc(64);
    strcpy(return_value, username_password);
    // Returns first token
    char *token = strtok(username_password, ":");
   
    // Keep printing tokens while one of the
    // delimiters present in str[].
    while (token != NULL)
    {
        //printf(token + '\n');
        token = strtok(NULL, "-");
    }
    return return_value;
}

void get_params(char const *argv[]){
    port = atoi(argv[1]);
}

int has_newline(char *str) {
    int length = strlen(str);
    if (length > 0 && str[length-1] == '\n') {
        return 1;
    } else {
        return 0;
    }
}

int delete_line(char *filename, int line_number) {
    FILE *fp, *temp;
    int count = 1;
    char buffer[512];

    // open the input file in read mode and create a temporary file in write mode
    fp = fopen(filename, "r");
    temp = fopen("temp.txt", "w");

    // check if files were opened successfully
    if (fp == NULL || temp == NULL) {
        printf("Failed to open file %s\n", filename);
        return -1;
    }

    // read the original file line by line and copy all lines to the temporary file except the line to be deleted
    while (fgets(buffer, 512, fp)) {
        if (count != line_number) {
            if(has_newline(buffer)){
                fprintf(temp, "%s", buffer);  
            }else{
                fprintf(temp, "%s\n", buffer);
            }
        }
        count++;
    }

    // close both files
    fclose(fp);
    fclose(temp);

    // delete the original file and rename the temporary file to the original file name
    remove(filename);
    rename("temp.txt", filename);

    return 0;
}

int main(int argc, char const *argv[])
{
    int socket_fd, new_socket, read_size;
    struct sockaddr_in server_address, client_address;
    char *buffer = malloc(64);
    char *password = malloc(64);

    // Create server socket
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Socket creation error");
        return -1;
    }

    // get args from system.in
    if(argc != 1){
        get_params(argv);
    }

    // Bind socket to address and port
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(port);
    if (bind(socket_fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        perror("Bind error");
        return -1;
    }

    // Listen for incoming connections
    if (listen(socket_fd, 3) < 0)
    {
        perror("Listen error");
        return -1;
    }

    // Accept incoming connection
    int address_len = sizeof(client_address);
    if ((new_socket = accept(socket_fd, (struct sockaddr *)&client_address, (socklen_t *)&address_len)) < 0)
    {
        perror("Accept error");
        return -1;
    }
    FILE *file;
    

    // Handle incoming messages from client
    while (1)
    {
        memset(buffer, 0, 64);
        if((read_size = read(new_socket, buffer, 1024)) > 0){
            if (strcmp(buffer, "Over") == 0){
                break;
            }
            printf("String recebida: %s\n",buffer);

            char str[124];
            /* opening file for reading */
            file = fopen("output.txt", "r+");
            if(file == NULL) {
                perror("Error opening file");
                return(-1);
            }
            int has_found = 0;
            int line_number = 1;
            while(fgets (str, 124, file)) {
                str[strcspn(str, "\n")] = '\0';
                printf("Linha %d -> %s\n", line_number, str);
                if(strcmp(str, buffer) == 0){
                    has_found = 1;
                    break;
                }
                line_number += 1;
            }
            fclose(file);

            if(has_found==1){
                memset(password, 0, 1024);
                send(new_socket, SUCCESS_MESSAGE, strlen(SUCCESS_MESSAGE), 0);
                if((read_size = read(new_socket, password, 1024)) > 0){

                    delete_line("output.txt", line_number);

                    handle_operation(buffer);

                    strcat(buffer, ":");

                    strcat(buffer, password);

                    printf("username:password a escrever: %s\n", buffer);
                    file = fopen("output.txt", "a+");

                    if (file == NULL)
                    {
                        printf("Error opening file!\n");
                        return 1;
                    }
                    //fputs(buffer, file);
                    fprintf(file, buffer);

                    fclose(file);
                    printf("Line written to file!\n");
                    send(new_socket, FILE_WRITTEN, strlen(FILE_WRITTEN), 0);
                }
            }else if(has_found==0){
                send(new_socket, ERROR_MESSAGE, strlen(ERROR_MESSAGE), 0);
            }   
        }
    }

    // Close sockets
    close(new_socket);
    close(socket_fd);


    return 0;
}